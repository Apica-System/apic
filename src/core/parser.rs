use apica_common::bytecodes::{ApicaEntrypointBytecode, ApicaSpecificationBytecode, ApicaTypeBytecode};
use apica_common::values::bool::ValueBool;
use apica_common::values::null::ValueNull;
use apica_common::values::string::ValueString;
use apica_common::values::u32::ValueU32;
use apica_common::values::value::Value;
use bitflags::bitflags;
use crate::core::lexer::Lexer;
use crate::nodes::_break::NodeBreak;
use crate::nodes::_continue::NodeContinue;
use crate::nodes::_return::NodeReturn;
use crate::nodes::_while::NodeWhile;
use crate::nodes::bad::NodeBad;
use crate::nodes::binary_op::NodeBinaryOp;
use crate::nodes::compound::NodeCompound;
use crate::nodes::data::bool::NodeDataBool;
use crate::nodes::data::specs::NodeDataSpecifications;
use crate::nodes::data::string::NodeDataString;
use crate::nodes::data::u32::NodeDataU32;
use crate::nodes::entrypoint::NodeEntrypoint;
use crate::nodes::eof::NodeEndOfFile;
use crate::nodes::function_call::NodeFunctionCall;
use crate::nodes::global_scope::NodeGlobalScope;
use crate::nodes::if_else::NodeIfElse;
use crate::nodes::literal::NodeLiteral;
use crate::nodes::node::Node;
use crate::nodes::package_call::NodePackageCall;
use crate::nodes::parameter::NodeParameter;
use crate::nodes::parenthesized::NodeParenthesized;
use crate::nodes::ternary_op::NodeTernaryOp;
use crate::nodes::unary_op::NodeUnaryOp;
use crate::nodes::var_const_call::NodeVarConstCall;
use crate::nodes::var_const_decl::NodeVarConstDecl;
use crate::utils::builtins::{APICA_SPECIFICATIONS, APICA_TYPES};
use crate::utils::decoder::{decode_binary_integer, decode_hexadecimal_integer, decode_integer, decode_octal_integer, decode_string};
use crate::utils::diagnostic::{Diagnostic, DiagnosticKind};
use crate::utils::diagnostic_bag::DiagnosticBag;
use crate::utils::position::Position;
use crate::utils::source::SourceText;
use crate::utils::token::{get_binary_precedence, get_unary_precedence, Token, TokenKind};

bitflags! {
    #[derive(Copy, Clone)]
    pub struct ParserModifier : u8 {
        const None =            0b0000_0000;
        const FullStatement =   0b0000_0001;
        const FunctionScope =   0b0000_0010;
        const LoopScope =       0b0000_0100;
        const GlobalScope =     0b0000_1000;
        const InnerScope =      0b0001_0000;
    }
}

pub struct Parser<'a> {
    diag_bag: &'a mut DiagnosticBag,
    source: &'a SourceText,
    index: usize,
    tokens: Vec<Token>,
}

impl<'a> Parser<'a> {
    pub fn init(diag_bag: &'a mut DiagnosticBag, source: &'a SourceText) -> Parser<'a> {
        let tokens = Parser::perform_lexer(diag_bag, source);
        Parser { diag_bag, source, index: 0, tokens }
    }

    pub fn parse(&mut self) -> NodeCompound {
        let mut nodes = vec![];

        let modifier = ParserModifier::FullStatement | ParserModifier::GlobalScope;
        let mut node = self.parse_statement(modifier);
        while !matches!(node, Node::EndOfFile(_)) {
            nodes.push(node);
            node = self.parse_statement(modifier);
        }

        nodes.push(node);
        NodeCompound::init(Position::init(0, 0), nodes)
    }

    fn parse_statement(&mut self, modifier: ParserModifier) -> Node {
        self.skip_new_lines();
        let current_kind = self.get().get_kind();
        match current_kind {
            TokenKind::LeftBrace => return self.parse_compound(modifier),
            TokenKind::Entrypoint => return self.parse_entrypoint(modifier),
            TokenKind::Specifications => return self.parse_specifications(modifier),
            TokenKind::Var => return self.parse_var_const_decl(modifier, false),
            TokenKind::Const => return self.parse_var_const_decl(modifier, true),
            TokenKind::Global => return self.parse_global_scope(modifier),
            TokenKind::If => return self.parse_if_statement(modifier),
            TokenKind::While => return self.parse_while_statement(modifier),

            _ => {},
        }

        let mut result: Option<Node> = self.parse_controller(modifier);
        if result.is_none() {
            result = Some(self.parse_binary_unary_expression(0));
        }

        result.unwrap()
    }

    fn parse_controller(&mut self, modifier: ParserModifier) -> Option<Node> {
        let current = Token::init_from(self.get());
        if *current.get_kind() == TokenKind::Return && modifier.contains(ParserModifier::FunctionScope) {
            self.advance();
            let mut expression: Option<Node> = None;
            let next = self.get().get_kind();
            if *next != TokenKind::NewLine && *next != TokenKind::SemiColon && *next != TokenKind::EndOfFile {
                let new_modifier = (modifier - ParserModifier::GlobalScope) | ParserModifier::InnerScope;
                expression = Some(self.parse_statement(new_modifier));
            }

            return Some(Node::Return(Box::new(NodeReturn::init(Position::init_from(current.get_position()), expression))));
        } else if modifier.contains(ParserModifier::LoopScope) {
            if *current.get_kind() == TokenKind::Break {
                self.advance();
                return Some(Node::Break(NodeBreak::init(Position::init_from(current.get_position()))));
            } else if *current.get_kind() == TokenKind::Continue {
                self.advance();
                return Some(Node::Continue(NodeContinue::init(Position::init_from(current.get_position()))));
            }
        }

        None
    }

    fn parse_binary_unary_expression(&mut self, precedence: u8) -> Node {
        self.skip_new_lines();

        let mut left: Node = {
            let unary_precedence = get_unary_precedence(self.get().get_kind());
            if unary_precedence != 0 && unary_precedence >= precedence {
                let current = self.get_and_advance();
                let operand = self.parse_binary_unary_expression(unary_precedence);
                Node::UnaryOp(Box::new(NodeUnaryOp::init(
                    Position::init_from(current.get_position()),
                    *current.get_kind(),
                    operand,
                )))
            } else {
                self.parse_primary_expression()
            }
        };

        loop {
            let current = Token::init_from(self.get());
            let binary_precedence = get_binary_precedence(current.get_kind());
            if binary_precedence == 0 || binary_precedence <= precedence {
                break;
            }

            self.advance();

            if binary_precedence == 14 {
                return Node::BinaryOp(Box::new(NodeBinaryOp::init(
                    Position::init_from(current.get_position()),
                    *current.get_kind(),
                    left,
                    None,
                )));
            }

            let right = self.parse_binary_unary_expression(binary_precedence);
            left = Node::BinaryOp(Box::new(NodeBinaryOp::init(
                Position::init_from(current.get_position()),
                *current.get_kind(),
                left,
                Some(right),
            )));
        }

        left
    }


    fn parse_primary_expression(&mut self) -> Node {
        let current = self.get_and_advance();
        match current.get_kind() {
            TokenKind::EndOfFile => Node::EndOfFile(NodeEndOfFile::init(Position::init_from(current.get_position()))),
            TokenKind::Bad => Node::Bad(NodeBad::init(Position::init_from(current.get_position()))),

            TokenKind::Null => Node::Literal(NodeLiteral::init(
                Position::init_from(current.get_position()),
                Value::Null(ValueNull::init()),
            )),

            TokenKind::False => Node::Literal(NodeLiteral::init(
                Position::init_from(current.get_position()),
                Value::Bool(ValueBool::init_with(false)),
            )),

            TokenKind::True => Node::Literal(NodeLiteral::init(
                Position::init_from(current.get_position()),
                Value::Bool(ValueBool::init_with(true)),
            )),

            TokenKind::Integer => self.create_integer(current),
            TokenKind::Binary => self.create_binary(current),
            TokenKind::Octal => self.create_octal(current),
            TokenKind::Hexadecimal => self.create_hexadecimal(current),
            TokenKind::String => self.create_string(current),

            TokenKind::LeftParenthesis => {
                let expression = self.parse_statement(ParserModifier::InnerScope);
                let end_token = Token::init_from(self.get());
                self.match_token(TokenKind::RightParenthesis, String::from("ParserError: Expected `)` to end a parenthesized expression"));

                Node::Parenthesized(Box::new(NodeParenthesized::init(
                    Position::init(current.get_position().get_start(), end_token.get_position().get_end() - current.get_position().get_start()),
                    expression,
                )))
            },

            TokenKind::Question => self.parse_ternary_expression(current.get_position().get_start()),

            TokenKind::Identifier => match self.get().get_kind() {
                TokenKind::DoubleColon => self.parse_package_call(current),
                TokenKind::LeftParenthesis => self.parse_function_call(current),

                _ => self.parse_var_const_call(current),
            },

            _ => {
                self.diag_bag.add(Diagnostic::init_complete(
                    DiagnosticKind::Error,
                    String::from("ParserError: Incorrect token found"),
                    Position::init_from(current.get_position()),
                ));

                Node::Bad(NodeBad::init(Position::init_from(current.get_position())))
            },
        }
    }

    fn parse_compound(&mut self, modifier: ParserModifier) -> Node {
        let start_pos = self.get_and_advance();
        self.skip_new_lines();
        let mut nodes = vec![];

        let new_modifier = (modifier - ParserModifier::GlobalScope) | ParserModifier::InnerScope | ParserModifier::FullStatement;
        let mut current = Token::init_from(self.get());
        while *current.get_kind() != TokenKind::RightBrace && *current.get_kind() != TokenKind::EndOfFile {
            nodes.push(self.parse_statement(new_modifier));
            self.skip_new_lines();
            current = Token::init_from(self.get());
        }

        self.match_token(TokenKind::RightBrace, String::from("ParserError: Expected `}` to end a block of statements"));
        Node::Compound(NodeCompound::init(
            Position::init(start_pos.get_position().get_start(), current.get_position().get_end() - start_pos.get_position().get_start()),
            nodes,
        ))
    }

    fn parse_entrypoint(&mut self, modifier: ParserModifier) -> Node {
        self.advance();
        self.skip_new_lines();
        let entry_token = self.get_and_advance();
        let entry_name = self.source.get_text_from_position(entry_token.get_position());
        let entry_bytecode = match entry_name.as_str() {
            "init" => Some(ApicaEntrypointBytecode::Init),
            "update" => Some(ApicaEntrypointBytecode::Update),
            "quit" => Some(ApicaEntrypointBytecode::Quit),
            _ => None,
        };

        if entry_bytecode.is_none() {
            self.diag_bag.add(Diagnostic::init_complete(
                DiagnosticKind::Error,
                String::from("ParserError: Unknown entrypoint identifier found (accepted are `init`, `update` and `quit`)"),
                Position::init_from(entry_token.get_position()),
            ));
            return Node::Bad(NodeBad::init(Position::init_from(entry_token.get_position())));
        }

        self.skip_new_lines();
        let brace_token = self.get();
        if *brace_token.get_kind() != TokenKind::LeftBrace {
            self.diag_bag.add(Diagnostic::init_complete(
                DiagnosticKind::Error,
                String::from("ParserError: All entrypoint should be followed by a block of statements"),
                Position::init_from(entry_token.get_position()),
            ));

            return Node::Bad(NodeBad::init(Position::init_from(entry_token.get_position())));
        }

        let body = self.parse_statement(ParserModifier::InnerScope);
        if modifier.contains(ParserModifier::InnerScope) {
            self.diag_bag.add(Diagnostic::init_complete(
                DiagnosticKind::Error,
                String::from("ParserError: Cannot declare an entrypoint out of the main global scope"),
                Position::init_from(entry_token.get_position()),
            ));

            return Node::Bad(NodeBad::init(Position::init_from(entry_token.get_position())));
        }

        Node::Entrypoint(Box::new(NodeEntrypoint::init(
            Position::init_from(entry_token.get_position()),
            entry_bytecode.unwrap(),
            body
        )))
    }

    fn parse_var_const_decl(&mut self, _: ParserModifier, is_const: bool) -> Node {
        self.advance();
        self.skip_new_lines();
        let identifier_token = Token::init_from(self.get());

        if *identifier_token.get_kind() != TokenKind::Identifier {
            self.diag_bag.add(Diagnostic::init_complete(
                DiagnosticKind::Error,
                String::from("ParserError: `const` and `var` keywords should be followed by an identifier to initialize a constant or a variable"),
                Position::init_from(identifier_token.get_position()),
            ));

            return Node::Bad(NodeBad::init(Position::init_from(identifier_token.get_position())));
        }

        self.advance();
        self.skip_new_lines();
        let id_name = self.source.get_text_from_position(identifier_token.get_position());
        let mut value_kind = ApicaTypeBytecode::Any;
        let decl_type = self.get();
        if *decl_type.get_kind() == TokenKind::Colon {
            self.advance();
            let value_type = Token::init_from(self.get());
            if *value_type.get_kind() != TokenKind::Identifier {
                self.diag_bag.add(Diagnostic::init_complete(
                    DiagnosticKind::Error,
                    String::from("ParserError: `const` and `var` keywords need a valid type after `:` (type declaration)"),
                    Position::init_from(value_type.get_position()),
                ));

                return Node::Bad(NodeBad::init(Position::init_from(value_type.get_position())));
            }

            self.advance();
            let type_text = self.source.get_text_from_position(value_type.get_position());
            if let Some(kind) = APICA_TYPES.get(type_text.as_str()) {
                value_kind = *kind;
            } else {
                self.diag_bag.add(Diagnostic::init_complete(
                    DiagnosticKind::Error,
                    String::from("ParserError: `const` and `var` keywords need a valid type after `:` (type declaration)"),
                    Position::init_from(value_type.get_position()),
                ));

                return Node::Bad(NodeBad::init(Position::init_from(value_type.get_position())));
            }
        }

        self.skip_new_lines();
        let assign_token = self.get();
        let expression = if *assign_token.get_kind() == TokenKind::Equals {
            self.advance();
            self.parse_statement(ParserModifier::InnerScope)
        } else {
            Node::Literal(NodeLiteral::init(Position::init_from(identifier_token.get_position()), Value::Null(ValueNull::init())))
        };

        Node::VarConstDecl(Box::new(NodeVarConstDecl::init(
            Position::init_from(identifier_token.get_position()),
            String::from(id_name),
            is_const,
            value_kind,
            expression,
        )))
    }

    fn parse_global_scope(&mut self, modifier: ParserModifier) -> Node {
        let global_token = self.get_and_advance();
        let scoped = self.parse_statement(modifier | ParserModifier::InnerScope);
        Node::GlobalScope(Box::new(NodeGlobalScope::init(
            Position::init_from(global_token.get_position()),
            scoped
        )))
    }

    fn parse_if_statement(&mut self, modifier: ParserModifier) -> Node {
        let if_token = self.get_and_advance();
        self.skip_new_lines();
        self.match_token(TokenKind::LeftParenthesis, String::from("ParserError: Expected `(` to begin an if condition"));

        let condition = self.parse_statement(ParserModifier::InnerScope);
        self.skip_new_lines();
        self.match_token(TokenKind::RightParenthesis, String::from("ParserError: Expected `)` to end an if condition"));

        let if_body = self.parse_statement(modifier | ParserModifier::InnerScope | ParserModifier::FullStatement);

        self.skip_new_lines();
        let else_body = if *self.get().get_kind() == TokenKind::Else {
            self.advance();
            Some(self.parse_statement(modifier | ParserModifier::InnerScope | ParserModifier::FullStatement))
        } else { None };

        Node::IfElse(Box::new(NodeIfElse::init(
            Position::init_from(if_token.get_position()),
            condition,
            if_body,
            else_body
        )))
    }

    fn parse_while_statement(&mut self, modifier: ParserModifier) -> Node {
        let while_token = self.get_and_advance();
        self.skip_new_lines();
        self.match_token(TokenKind::LeftParenthesis, String::from("ParserError: Expected `(` to begin a while condition"));

        let condition = self.parse_statement(ParserModifier::InnerScope);
        self.skip_new_lines();
        self.match_token(TokenKind::RightParenthesis, String::from("ParserError: Expected `)` to end a while condition"));

        let body = self.parse_statement(modifier | ParserModifier::InnerScope | ParserModifier::FullStatement | ParserModifier::LoopScope);
        Node::While(Box::new(NodeWhile::init(
            Position::init_from(while_token.get_position()),
            condition,
            body
        )))
    }

    fn parse_ternary_expression(&mut self, start: usize) -> Node {
        let condition = self.parse_statement(ParserModifier::InnerScope);
        self.skip_new_lines();
        self.match_token(TokenKind::Colon, String::from("ParserError: Expected `:` to separate first result from the condition of a ternary expression"));

        let first = self.parse_statement(ParserModifier::InnerScope);
        self.skip_new_lines();
        self.match_token(TokenKind::Colon, String::from("ParserError: Expected `:` to separate second result from the first result of a ternary expression"));

        let second = self.parse_statement(ParserModifier::InnerScope);
        self.skip_new_lines();

        Node::TernaryOp(Box::new(NodeTernaryOp::init(
            Position::init(start, 1),
            condition,
            first,
            second
        )))
    }

    fn parse_package_call(&mut self, token: Token) -> Node {
        self.advance();
        let name = self.source.get_text_from_position(token.get_position());
        let next = self.parse_statement(ParserModifier::InnerScope);
        if !matches!(next, Node::VarConstCall(_)) && !matches!(next, Node::FuncCall(_)) {
            self.diag_bag.add(Diagnostic::init_complete(
                DiagnosticKind::Error,
                String::from("ParserError: Package call (i.e. <package_id>:) should be followed by a var/const call or a function call"),
                Position::init_from(next.get_position()),
            ));

            return Node::Bad(NodeBad::init(Position::init_from(next.get_position())));
        }

        Node::PackageCall(Box::new(NodePackageCall::init(
            Position::init_from(token.get_position()),
            String::from(name),
            next
        )))
    }

    fn parse_function_call(&mut self, token: Token) -> Node {
        self.advance();
        let func_name = self.source.get_text_from_position(token.get_position());
        let mut actual_token = self.get();
        let mut parameters = vec![];

        while *actual_token.get_kind() != TokenKind::RightParenthesis && *actual_token.get_kind() != TokenKind::EndOfFile {
            if !parameters.is_empty() {
                self.match_token(TokenKind::Comma, String::from("ParserError: Expected a `,` between parameters in a function call"));
            }

            parameters.push(self.parse_parameter());
            actual_token = self.get();
        }

        self.match_token(TokenKind::RightParenthesis, String::from("ParserError: Expected `)` to end a function call"));
        Node::FuncCall(NodeFunctionCall::init(
            Position::init_from(token.get_position()),
            String::from(func_name),
            parameters
        ))
    }

    fn parse_parameter(&mut self) -> NodeParameter {
        let current = Token::init_from(self.get());
        let expression = self.parse_statement(ParserModifier::InnerScope);
        NodeParameter::init(Position::init_from(current.get_position()), expression)
    }

    fn parse_var_const_call(&mut self, token: Token) -> Node {
        let var_const_name = self.source.get_text_from_position(token.get_position());
        Node::VarConstCall(NodeVarConstCall::init(
            Position::init_from(token.get_position()),
            String::from(var_const_name)
        ))
    }

    fn parse_specifications(&mut self, modifier: ParserModifier) -> Node {
        let specs_token = self.get_and_advance();
        self.skip_new_lines();

        let left_brace_token = self.get();
        if *left_brace_token.get_kind() != TokenKind::LeftBrace {
            self.diag_bag.add(Diagnostic::init_complete(
                DiagnosticKind::Error,
                String::from("ParserError: `specs` should be followed by a block of specifications"),
                Position::init_from(specs_token.get_position()),
            ));

            return Node::Bad(NodeBad::init(Position::init_from(specs_token.get_position())));
        }
        self.advance();
        self.skip_new_lines();

        let mut specs = vec![];
        while let Some(spec) = self.parse_specification() {
            specs.push(spec);
            if *self.get().get_kind() != TokenKind::Comma { break; }

            self.advance();
            self.skip_new_lines();
        }

        self.skip_new_lines();
        let right_brace_token = self.get();
        if *right_brace_token.get_kind() != TokenKind::RightBrace {
            self.diag_bag.add(Diagnostic::init_complete(
                DiagnosticKind::Error,
                String::from("ParserError: `specs` block of specifications should be closed by `}`"),
                Position::init_from(right_brace_token.get_position()),
            ));

            return Node::Bad(NodeBad::init(Position::init_from(specs_token.get_position())));
        }
        self.advance();

        if modifier.contains(ParserModifier::InnerScope) {
            self.diag_bag.add(Diagnostic::init_complete(
                DiagnosticKind::Error,
                String::from("ParserError: Cannot declare an entrypoint out of the main global scope"),
                Position::init_from(specs_token.get_position()),
            ));

            return Node::Bad(NodeBad::init(Position::init_from(specs_token.get_position())));
        }

        Node::DataSpecs(NodeDataSpecifications::init(
            Position::init_from(specs_token.get_position()),
            specs
        ))
    }

    fn parse_specification(&mut self) -> Option<Node> {
        if *self.get().get_kind() == TokenKind::RightBrace {
            return None;
        }

        let spec_name = self.get_and_advance();
        if *spec_name.get_kind() != TokenKind::Identifier {
            self.diag_bag.add(Diagnostic::init_complete(
                DiagnosticKind::Error,
                String::from("ParserError: A specification attribute should begin with an identifier"),
                Position::init_from(spec_name.get_position()),
            ));
        }
        self.match_token(TokenKind::Colon, String::from("ParserError: Expected a `:` after the specification attribute"));

        let spec_name_string = self.source.get_text_from_position(spec_name.get_position());
        let spec_bytecode = APICA_SPECIFICATIONS.get(spec_name_string.as_str());
        if spec_bytecode.is_none() {
            self.diag_bag.add(Diagnostic::init_complete(
                DiagnosticKind::Error,
                format!("ParserError: An incorrect specification attribute was found `{}`", spec_name_string),
                Position::init_from(spec_name.get_position()),
            ));

            return None;
        }

        let bytecode = spec_bytecode.unwrap();
        match bytecode {
            ApicaSpecificationBytecode::Title => Some(self.parse_data_string(*bytecode, "title")),
            ApicaSpecificationBytecode::Id => Some(self.parse_data_string(*bytecode, "id")),
            ApicaSpecificationBytecode::Version => Some(self.parse_data_string(*bytecode, "version")),
            ApicaSpecificationBytecode::LoggerActivation => Some(self.parse_data_bool(*bytecode, "logger")),
            ApicaSpecificationBytecode::WindowWidth => Some(self.parse_data_u32(*bytecode, "width")),
            ApicaSpecificationBytecode::WindowHeight => Some(self.parse_data_u32(*bytecode, "height")),

            _ => {
                self.diag_bag.add(Diagnostic::init_complete(
                    DiagnosticKind::Error,
                    format!("ParserError: An incorrect specification attribute was found `{}`", spec_name_string),
                    Position::init_from(spec_name.get_position()),
                ));
                None
            },
        }
    }

    fn parse_data_string(&mut self, bytecode: ApicaSpecificationBytecode, name: &str) -> Node {
        let value = self.parse_statement(ParserModifier::InnerScope);
        let position = Position::init_from(value.get_position());
        if let Node::Literal(literal) = value {
            if let Value::String(string) = literal.get_literal() {
                return Node::DataString(NodeDataString::init(
                    Position::init_from(&position),
                    bytecode,
                    string.get_value().as_ref().unwrap().clone()
                ));
            }
        }

        self.diag_bag.add(Diagnostic::init_complete(
            DiagnosticKind::Error,
            format!("ParserError: `{name}` specification attribute should be followed by a string literal"),
            Position::init_from(&position),
        ));
        Node::Bad(NodeBad::init(Position::init_from(&position)))
    }

    fn parse_data_u32(&mut self, bytecode: ApicaSpecificationBytecode, name: &str) -> Node {
        let value = self.parse_statement(ParserModifier::InnerScope);
        let position = Position::init_from(value.get_position());
        if let Node::Literal(literal) = value {
            if let Value::U32(u32) = literal.get_literal() {
                return Node::DataU32(NodeDataU32::init(
                    Position::init_from(&position),
                    bytecode,
                    u32.get_value().unwrap(),
                ));
            }
        }

        self.diag_bag.add(Diagnostic::init_complete(
            DiagnosticKind::Error,
            format!("ParserError: `{name}` specification attribute should be followed by a u32 literal"),
            Position::init_from(&position),
        ));
        Node::Bad(NodeBad::init(Position::init_from(&position)))
    }

    fn parse_data_bool(&mut self, bytecode: ApicaSpecificationBytecode, name: &str) -> Node {
        let value = self.parse_statement(ParserModifier::InnerScope);
        let position = Position::init_from(value.get_position());
        if let Node::Literal(literal) = value {
            if let Value::Bool(bool) = literal.get_literal() {
                return Node::DataBool(NodeDataBool::init(
                    Position::init_from(&position),
                    bytecode,
                    bool.get_value().unwrap(),
                ));
            }
        }

        self.diag_bag.add(Diagnostic::init_complete(
            DiagnosticKind::Error,
            format!("ParserError: `{name}` specification attribute should be followed by a bool literal"),
            Position::init_from(&position),
        ));
        Node::Bad(NodeBad::init(Position::init_from(&position)))
    }

    fn perform_lexer(diag_bag: &'a mut DiagnosticBag, source: &'a SourceText) -> Vec<Token> {
        let mut tokens = vec![];
        let mut lexer = Lexer::init(source, diag_bag);

        let mut token = lexer.lex();
        while *token.get_kind() != TokenKind::EndOfFile {
            tokens.push(token);
            token = lexer.lex();
        }

        tokens.push(token);
        tokens
    }

    fn skip_new_lines(&mut self) {
        let mut actual = self.get().get_kind();
        while *actual == TokenKind::NewLine || *actual == TokenKind::SemiColon {
            self.advance();
            actual = self.get().get_kind();
        }
    }

    fn get(&self) -> &Token {
        if self.index >= self.tokens.len() {
            self.tokens.last().unwrap()
        } else {
            &self.tokens[self.index]
        }
    }

    fn advance(&mut self) {
        self.index += 1;
    }

    fn get_and_advance(&mut self) -> Token {
        self.advance();
        if self.index - 1 >= self.tokens.len() {
            Token::init_from(&self.tokens.last().unwrap())
        } else {
            Token::init_from(&self.tokens.get(self.index - 1).unwrap())
        }
    }

    fn match_token(&mut self, expected: TokenKind, error_text: String) {
        let actual = self.get_and_advance();
        if *actual.get_kind() != expected {
            self.diag_bag.add(Diagnostic::init_complete(
                DiagnosticKind::Error,
                error_text,
                Position::init_from(actual.get_position()),
            ))
        }
    }

    fn create_integer(&self, token: Token) -> Node {
        let text = self.source.get_text_from_position(token.get_position());
        let integer = decode_integer(&text);

        Node::Literal(NodeLiteral::init(
            Position::init_from(token.get_position()),
            Value::U32(ValueU32::init_with(integer)),
        ))
    }

    fn create_binary(&self, token: Token) -> Node {
        let text = self.source.get_text_from_position(token.get_position());
        let integer = decode_binary_integer(&text);

        Node::Literal(NodeLiteral::init(
            Position::init_from(token.get_position()),
            Value::U32(ValueU32::init_with(integer)),
        ))
    }

    fn create_octal(&self, token: Token) -> Node {
        let text = self.source.get_text_from_position(token.get_position());
        let integer = decode_octal_integer(&text);

        Node::Literal(NodeLiteral::init(
            Position::init_from(token.get_position()),
            Value::U32(ValueU32::init_with(integer)),
        ))
    }

    fn create_hexadecimal(&self, token: Token) -> Node {
        let text = self.source.get_text_from_position(token.get_position());
        let integer = decode_hexadecimal_integer(&text);

        Node::Literal(NodeLiteral::init(
            Position::init_from(token.get_position()),
            Value::U32(ValueU32::init_with(integer)),
        ))
    }

    fn create_string(&self, token: Token) -> Node {
        let text = self.source.get_text_from_position(token.get_position());
        let string = decode_string(&text);

        Node::Literal(NodeLiteral::init(
            Position::init_from(token.get_position()),
            Value::String(ValueString::init_with(string)),
        ))
    }
}
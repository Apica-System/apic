#include "core/parser.hpp"
#include "core/lexer.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/errors.hpp"
#include "utils/decoder.hpp"
#include "utils/builtins.hpp"

#include "nodes/return.hpp"
#include "nodes/break.hpp"
#include "nodes/continue.hpp"
#include "nodes/unary.hpp"
#include "nodes/binary.hpp"
#include "nodes/ternary.hpp"
#include "nodes/eof.hpp"
#include "nodes/bad.hpp"
#include "nodes/literal.hpp"
#include "nodes/var_const_call.hpp"
#include "nodes/package_call.hpp"
#include "nodes/entrypoint.hpp"
#include "nodes/func_call.hpp"
#include "nodes/var_const_decl.hpp"
#include "nodes/typeof.hpp"
#include "nodes/global_scope.hpp"
#include "nodes/if_else.hpp"
#include "nodes/while.hpp"
#include "nodes/data/specs.hpp"
#include "nodes/data/string.hpp"
#include "nodes/data/u32.hpp"
#include "nodes/data/bool.hpp"

#include "values/null.hpp"
#include "values/bool.hpp"
#include "values/u32.hpp"
#include "values/u64.hpp"
#include "values/f32.hpp"
#include "values/f64.hpp"
#include "values/char.hpp"
#include "values/string.hpp"

using namespace core;

enum ParserModifier : uint8_t {
    PM_None =           0b00000000,
    PM_FullStatement =  0b00000001,
    PM_FunctionScope =  0b00000010,
    PM_LoopScope =      0b00000100,
    PM_GlobalScope =    0b00001000,
    PM_InnerScope =     0b00010000,
};

Parser::Parser(const utils::SourceText &source)
    : source(source), index(0) {
    Lexer lexer(source);
    utils::Token token = lexer.lex();
    while (token.getKind() != utils::TokenKind::EndOfFile) {
        this->tokens.push_back(token);
        token = lexer.lex();
    }

    this->tokens.push_back(token);
}

nodes::NodeCompound *Parser::parse() {
    std::vector<nodes::Node*> nodes;

    uint8_t modifier = PM_GlobalScope | PM_FullStatement;
    nodes::Node *node = this->parseStatement(modifier);
    while (node->getKind() != nodes::NodeKind::EndOfFile) {
        nodes.push_back(node);
        node = this->parseStatement(modifier);
    }

    nodes.push_back(node);
    return new nodes::NodeCompound(utils::Position(0, 0, 0, 0), nodes);
}

void Parser::skipNewLines() {
    utils::TokenKind actual = this->get().getKind();
    while (actual == utils::TokenKind::NewLine || actual == utils::TokenKind::SemiColon) {
        this->advance();
        actual = this->get().getKind();
    }
}

utils::Token Parser::get() const {
    return this->index >= this->tokens.size()
        ? this->tokens.back()
        : this->tokens[this->index];
}

utils::Token Parser::getAt(uint64_t offset) const {
    uint64_t new_index = this->index + offset;
    return new_index >= this->tokens.size()
        ? this->tokens.back()
        : this->tokens[new_index];
}

utils::Token Parser::getAndAdvance() {
    utils::Token current = this->get();
    this->advance();
    return current;
}

void Parser::advance() {
    this->index++;
}

void Parser::matchToken(utils::TokenKind expected, const std::string &error_text) {
    utils::Token actual = this->get();
    if (actual.getKind() != expected) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            error_text,
            actual.getPosition()
        ));
    } else {
        this->advance();
    }
}

nodes::Node *Parser::parseStatement(uint8_t modifier) {
    this->skipNewLines();
    utils::TokenKind current_kind = this->get().getKind();
    switch (current_kind) {
        case utils::TokenKind::LeftBrace: return this->parseCompound(modifier);
        case utils::TokenKind::Entrypoint: return this->parseEntrypoint(modifier);
        case utils::TokenKind::Specifications: return this->parseSpecifications(modifier);
        case utils::TokenKind::Var: return this->parseVarConstDecl(false);
        case utils::TokenKind::Const: return this->parseVarConstDecl(true);
        case utils::TokenKind::Global: return this->parseGlobalScope(modifier);
        case utils::TokenKind::If: return this->parseIfStatement(modifier);
        case utils::TokenKind::While: return this->parseWhileStatement(modifier);

        default: break;
    }

    std::optional<nodes::Node*> result = this->parseController(modifier);
    if (!result)
        result = this->parseBinaryUnaryExpression(0);
    
    if (modifier & ParserModifier::PM_FullStatement) {
        utils::Token current = this->get();
        if (current.getKind() != utils::TokenKind::SemiColon && current.getKind() != utils::TokenKind::NewLine 
            && current.getKind() != utils::TokenKind::EndOfFile) {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Error,
                std::string(utils::PAR_ERROR_NOT_SEPARATED_STATS),
                current.getPosition()
            ));

            return result.value();
        }

        this->advance();
    }
    
    return result.value();
}

std::optional<nodes::Node*> Parser::parseController(uint8_t modifier) {
    utils::Token current = this->get();
    if (current.getKind() == utils::TokenKind::Return && (modifier & ParserModifier::PM_FunctionScope)) {
        this->advance();
        utils::TokenKind next_kind = this->get().getKind();
        if (next_kind != utils::TokenKind::NewLine && next_kind != utils::TokenKind::SemiColon && next_kind != utils::TokenKind::EndOfFile) {
            uint8_t new_modifier = (modifier & ~ParserModifier::PM_GlobalScope) | ParserModifier::PM_InnerScope;
            nodes::Node *expression = this->parseStatement(new_modifier);
            return new nodes::NodeReturn(current.getPosition(), expression);
        }

        return new nodes::NodeReturn(current.getPosition());
    }

    if (modifier & ParserModifier::PM_LoopScope) {
        if (current.getKind() == utils::TokenKind::Break) {
            this->advance();
            return new nodes::NodeBreak(current.getPosition());
        } else if (current.getKind() == utils::TokenKind::Continue) {
            this->advance();
            return new nodes::NodeContinue(current.getPosition());
        }
    }

    return std::nullopt;
}

nodes::Node *Parser::parseBinaryUnaryExpression(uint8_t precedence) {
    this->skipNewLines();
    uint8_t unary_precedence = utils::Token::getUnaryPrecedence(this->get().getKind());
    nodes::Node *left;

    if (unary_precedence != 0 && unary_precedence >= precedence) {
        utils::Token current = this->getAndAdvance();
        nodes::Node *operand = this->parseBinaryUnaryExpression(unary_precedence);
        left = new nodes::NodeUnaryOperation(
            current.getPosition(), current.getKind(),
            operand
        );
    } else {
        left = this->parsePrimaryExpression();
    }

    utils::Token current = this->get();
    uint8_t binary_precedence = utils::Token::getBinaryPrecedence(current.getKind());
    while (binary_precedence != 0 && binary_precedence > precedence) {
        this->advance();
        if (binary_precedence == 14) {
            return new nodes::NodeBinaryOperation(
                current.getPosition(), current.getKind(),
                left
            );
        }

        nodes::Node *right = this->parseBinaryUnaryExpression(binary_precedence);
        left = new nodes::NodeBinaryOperation(
            current.getPosition(), current.getKind(),
            left, right
        );

        current = this->get();
        binary_precedence = utils::Token::getBinaryPrecedence(current.getKind());
    }

    return left;
}

nodes::Node *Parser::parsePrimaryExpression() {
    utils::Token current = this->getAndAdvance();
    switch (current.getKind()) {
        case utils::TokenKind::EndOfFile:
            return new nodes::NodeEndOfFile(current.getPosition());
        
        case utils::TokenKind::Bad:
            return new nodes::NodeBad(current.getPosition());
        
        case utils::TokenKind::Null:
            return new nodes::NodeLiteral(
                current.getPosition(),
                new common::values::ValueNull()
            );
        
        case utils::TokenKind::False:
            return new nodes::NodeLiteral(
                current.getPosition(),
                new common::values::ValueBool(false)
            );
        
        case utils::TokenKind::True:
            return new nodes::NodeLiteral(
                current.getPosition(),
                new common::values::ValueBool(true)
            );
        
        case utils::TokenKind::Integer: return this->createInteger(current);
        case utils::TokenKind::Binary: return this->createBinaryInteger(current);
        case utils::TokenKind::Octal: return this->createOctalInteger(current);
        case utils::TokenKind::Hexadecimal: return this->createHexadecimalInteger(current);
        case utils::TokenKind::BigInteger: return this->createBigInteger(current);
        case utils::TokenKind::BigBinary: return this->createBigBinaryInteger(current);
        case utils::TokenKind::BigOctal: return this->createBigOctalInteger(current);
        case utils::TokenKind::BigHexadecimal: return this->createBigHexadecimalInteger(current);
        case utils::TokenKind::Decimal: return this->createDecimal(current);
        case utils::TokenKind::BigDecimal: return this->createBigDecimal(current);
        case utils::TokenKind::Character: return this->createCharacter(current);
        case utils::TokenKind::String: return this->createString(current);
        case utils::TokenKind::RawString: return this->createRawString(current);
        
        case utils::TokenKind::LeftParenthesis: {
            nodes::Node *expression = this->parseStatement(ParserModifier::PM_InnerScope);
            this->matchToken(utils::TokenKind::RightParenthesis, std::string(utils::PAR_ERROR_PARENTHESIS_AWAITED));

            return expression;
        }

        case utils::TokenKind::Question: return this->parseTernaryExpression(current.getPosition());

        case utils::TokenKind::Identifier: {
            switch (this->get().getKind()) {
                case utils::TokenKind::DoubleColon: return this->parsePackageCall(current);
                case utils::TokenKind::LeftParenthesis: return this->parseFunctionCall(current);

                default: return this->parseVarConstCall(current);
            }
        }

        case utils::TokenKind::Typeof: return this->parseTypeof(current.getPosition());

        default: {
            std::string error_message(utils::PAR_ERROR_INCORRECT_TOKEN);
            error_message += std::to_string(current.getKind());
            error_message += '`';

            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Error,
                error_message,
                current.getPosition()
            ));

            return new nodes::NodeBad(current.getPosition());
        }
    }
}

nodes::Node *Parser::parseCompound(uint8_t modifier) {
    utils::Position start_pos = this->getAndAdvance().getPosition();
    this->skipNewLines();
    std::vector<nodes::Node*> nodes;

    uint8_t new_modifier = (modifier & ~ParserModifier::PM_GlobalScope) | ParserModifier::PM_InnerScope | ParserModifier::PM_FullStatement;
    utils::Token current = this->get();
    while (current.getKind() != utils::TokenKind::RightBrace && current.getKind() != utils::TokenKind::EndOfFile) {
        nodes.push_back(this->parseStatement(new_modifier));
        this->skipNewLines();

        current = this->get();
    }

    this->matchToken(utils::TokenKind::RightBrace, std::string(utils::PAR_ERROR_BRACE_AWAITED));
    return new nodes::NodeCompound(
        utils::Position(start_pos, current.getPosition()),
        nodes
    );
}

nodes::Node *Parser::parseEntrypoint(uint8_t modifier) {
    this->advance();
    this->skipNewLines();
    utils::Token entry_token = this->getAndAdvance();
    std::string entry_name = this->source.getTextFromPosition(entry_token.getPosition()).value();
    std::optional<common::bytecodes::ApicaEntrypointBytecode> entry_bytecode = std::nullopt;
    if (entry_name == "init")
        entry_bytecode = common::bytecodes::ApicaEntrypointBytecode::Init;
    else if (entry_name == "update")
        entry_bytecode = common::bytecodes::ApicaEntrypointBytecode::Update;
    else if (entry_name == "quit")
        entry_bytecode = common::bytecodes::ApicaEntrypointBytecode::Quit;
    
    if (!entry_bytecode) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::PAR_ERROR_UNKNOWN_ENTRYPOINT),
            entry_token.getPosition()
        ));
    }

    this->skipNewLines();
    utils::Token brace_token = this->get();
    if (brace_token.getKind() != utils::TokenKind::LeftBrace) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::PAR_ERROR_ENTRYPOINT_WITHOUT_COMPOUND),
            brace_token.getPosition()
        ));
    }

    nodes::Node *body = this->parseStatement(ParserModifier::PM_InnerScope);
    if (modifier & ParserModifier::PM_InnerScope) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::PAR_ERROR_ENTRYPOINT_OUT_OF_MAIN_SCOPE),
            entry_token.getPosition()
        ));
    }

    return new nodes::NodeEntrypoint(
        entry_token.getPosition(),
        entry_bytecode.value_or(common::bytecodes::ApicaEntrypointBytecode::Init),
        body      
    );
}

nodes::Node *Parser::parseSpecifications(uint8_t modifier) {
    utils::Token specs_token = this->getAndAdvance();
    this->skipNewLines();

    this->matchToken(utils::TokenKind::LeftBrace, std::string(utils::PAR_ERROR_SPECS_WITHOUT_COMPOUND));
    this->skipNewLines();

    std::vector<nodes::NodeData*> specs;
    std::optional<nodes::Node*> data_spec = this->parseSpecification();
    while (data_spec) {
        if (data_spec.value()->getKind() >= nodes::NodeKind::DataSpecs)
            specs.push_back(static_cast<nodes::NodeData*>(data_spec.value()));
        else
            delete data_spec.value();
        
        if (this->get().getKind() != utils::TokenKind::Comma) 
            break;

        this->advance();
        this->skipNewLines();
        data_spec = this->parseSpecification();
    }

    this->skipNewLines();
    this->matchToken(utils::TokenKind::RightBrace, std::string(utils::PAR_ERROR_UNTERMINATED_SPECS));
    if (modifier & ParserModifier::PM_InnerScope) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::PAR_ERROR_SPECS_OUT_OF_MAIN_SCOPE),
            specs_token.getPosition()
        ));
    }

    return new nodes::NodeDataSpecifications(
        specs_token.getPosition(),
        specs
    );
}

nodes::Node *Parser::parseVarConstDecl(bool is_const) {
    this->advance();
    this->skipNewLines();
    utils::Token identifier_token = this->get();
    if (identifier_token.getKind() != utils::TokenKind::Identifier) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::PAR_ERROR_VAR_CONST_WITHOUT_ID),
            identifier_token.getPosition()
        ));
    }

    this->advance();
    this->skipNewLines();

    std::optional<std::string> value_type = std::nullopt;
    if (this->get().getKind() == utils::TokenKind::Colon) {
        this->advance();
        utils::Token value_token = this->getAndAdvance();
        if (value_token.getKind() != utils::TokenKind::Identifier) {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Error,
                std::string(utils::PAR_ERROR_VAR_CONST_INCORRECT_TYPE),
                value_token.getPosition()
            ));
        }

        value_type = this->source.getTextFromPosition(value_token.getPosition()).value();
    }

    this->skipNewLines();
    std::optional<nodes::Node*> expression = std::nullopt;
    if (this->get().getKind() == utils::TokenKind::Equals) {
        this->advance();
        expression = this->parseStatement(ParserModifier::PM_InnerScope);
    }

    std::string var_const_name = this->source.getTextFromPosition(identifier_token.getPosition()).value();
    if (!value_type) {
        return new nodes::NodeVarConstDeclaration(
            identifier_token.getPosition(),
            var_const_name,
            is_const,
            expression.value()
        );
    } else if (!expression) {
        return new nodes::NodeVarConstDeclaration(
            identifier_token.getPosition(),
            var_const_name,
            is_const,
            value_type.value()
        );
    } else {
        return new nodes::NodeVarConstDeclaration(
            identifier_token.getPosition(),
            var_const_name,
            is_const,
            value_type.value(),
            expression.value()
        );
    }
}

nodes::Node *Parser::parseGlobalScope(uint8_t modifier) {
    utils::Position global_token_pos = this->getAndAdvance().getPosition();
    nodes::Node *contained = this->parseStatement(modifier | ParserModifier::PM_InnerScope);
    return new nodes::NodeGlobalScope(
        global_token_pos,
        contained
    );
}

nodes::Node *Parser::parseIfStatement(uint8_t modifier) {
    utils::Position if_token_pos = this->getAndAdvance().getPosition();
    this->skipNewLines();
    this->matchToken(utils::TokenKind::LeftParenthesis, std::string(utils::PAR_ERROR_IF_WITHOUT_LPARENTH));

    nodes::Node *condition = this->parseStatement(ParserModifier::PM_InnerScope);
    this->skipNewLines();
    this->matchToken(utils::TokenKind::RightParenthesis, std::string(utils::PAR_ERROR_UNTERMINATED_IF_CND));

    uint8_t new_modifier = modifier | ParserModifier::PM_InnerScope | ParserModifier::PM_FullStatement;
    nodes::Node *if_body = this->parseStatement(new_modifier);

    this->skipNewLines();
    if (this->get().getKind() == utils::TokenKind::Else) {
        this->advance();
        nodes::Node *else_body = this->parseStatement(new_modifier);
        return new nodes::NodeIfElse(
            utils::Position(if_token_pos, else_body->getPosition()),
            condition, if_body,
            else_body
        );
    }

    return new nodes::NodeIfElse(
        utils::Position(if_token_pos, if_body->getPosition()),
        condition, if_body
    );
}

nodes::Node *Parser::parseWhileStatement(uint8_t modifier) {
    utils::Position while_token_pos = this->getAndAdvance().getPosition();
    this->skipNewLines();
    this->matchToken(utils::TokenKind::LeftParenthesis, std::string(utils::PAR_ERROR_WHILE_WITHOUT_LPARENTH));

    nodes::Node *condition = this->parseStatement(ParserModifier::PM_InnerScope);
    this->skipNewLines();
    this->matchToken(utils::TokenKind::RightParenthesis, std::string(utils::PAR_ERROR_UNTERMINATED_WHILE_CND));

    nodes::Node *body = this->parseStatement(modifier | ParserModifier::PM_InnerScope | ParserModifier::PM_FullStatement | ParserModifier::PM_LoopScope);
    return new nodes::NodeWhile(
        utils::Position(while_token_pos, body->getPosition()),
        condition, body
    );
}

nodes::Node *Parser::parseTypeof(const utils::Position &typeof_position) {
    this->matchToken(utils::TokenKind::Less, std::string(utils::PAR_ERROR_TYPEOF_WITHOUT_LESS_CHAR));

    utils::Token type_token = this->getAndAdvance();
    if (type_token.getKind() != utils::TokenKind::Identifier) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::PAR_ERROR_TYPEOF_INCORRECT_TYPE),
            type_token.getPosition()
        ));
    }

    this->matchToken(utils::TokenKind::Greater, std::string(utils::PAR_ERROR_UNTERMINATED_TYPEOF));
    return new nodes::NodeTypeof(
        typeof_position,
        this->source.getTextFromPosition(type_token.getPosition()).value()
    );
}

nodes::Node *Parser::parseTernaryExpression(const utils::Position &question_pos) {
    nodes::Node *condition = this->parseStatement(ParserModifier::PM_InnerScope);
    this->skipNewLines();
    this->matchToken(utils::TokenKind::Colon, std::string(utils::PAR_ERROR_TERNARY_SEP_CND_TRUE_EXPR));

    nodes::Node *true_expr = this->parseStatement(ParserModifier::PM_InnerScope);
    this->skipNewLines();
    this->matchToken(utils::TokenKind::Colon, std::string(utils::PAR_ERROR_TERNARY_SEP_TRUE_FALSE_EXPR));

    nodes::Node *false_expr = this->parseStatement(ParserModifier::PM_InnerScope);

    return new nodes::NodeTernaryOperation(
        utils::Position(question_pos, false_expr->getPosition()),
        condition,
        true_expr, false_expr
    );
}

nodes::Node *Parser::parsePackageCall(const utils::Token &token) {
    this->advance();
    nodes::Node *next = this->parseStatement(ParserModifier::PM_InnerScope);
    if (next->getKind() != nodes::NodeKind::VarConstCall && next->getKind() != nodes::NodeKind::FuncCall) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::PAR_ERROR_PACKAGE_CALL),
            next->getPosition()
        ));
    }

    return new nodes::NodePackageCall(
        token.getPosition(),
        this->source.getTextFromPosition(token.getPosition()).value(),
        next
    );
}

nodes::Node *Parser::parseVarConstCall(const utils::Token &token) const {
    return new nodes::NodeVarConstCall(
        token.getPosition(),
        this->source.getTextFromPosition(token.getPosition()).value()
    );
}

nodes::Node *Parser::parseFunctionCall(const utils::Token &token) {
    this->advance();
    utils::Token actual_token = this->get();
    std::vector<nodes::NodeParameter*> parameters;

    while (actual_token.getKind() != utils::TokenKind::RightParenthesis && actual_token.getKind() != utils::TokenKind::EndOfFile) {
        if (parameters.size())
            this->matchToken(utils::TokenKind::Comma, std::string(utils::PAR_ERROR_COMMA_BETWEEN_PARAMS));
        
        parameters.push_back(this->parseParameter());
        actual_token = this->get();
    }

    this->matchToken(utils::TokenKind::RightParenthesis, std::string(utils::PAR_ERROR_UNTERMINATED_FUNC_CALL));
    return new nodes::NodeFuncCall(
        token.getPosition(),
        this->source.getTextFromPosition(token.getPosition()).value(),
        parameters
    );
}

nodes::NodeParameter *Parser::parseParameter() {
    utils::Token first_token = this->get();
    std::optional<std::string> parameter_name = std::nullopt;
    if (this->getAt(1).getKind() == utils::TokenKind::Colon) {
        parameter_name = this->source.getTextFromPosition(first_token.getPosition()).value();
        this->advance();
        this->advance();
    }

    nodes::Node *expression = this->parseStatement(ParserModifier::PM_InnerScope);
    return parameter_name.has_value()
        ? new nodes::NodeParameter(utils::Position(first_token.getPosition(), expression->getPosition()), expression, parameter_name.value())
        : new nodes::NodeParameter(utils::Position(first_token.getPosition(), expression->getPosition()), expression);
}

std::optional<nodes::Node*> Parser::parseSpecification() {
    if (this->get().getKind() == utils::TokenKind::RightBrace)
        return std::nullopt;
    
    utils::Token spec_name_token = this->getAndAdvance();
    if (spec_name_token.getKind() != utils::TokenKind::Identifier) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::PAR_ERROR_SPEC_WITHOUT_ID),
            spec_name_token.getPosition()
        ));
    }
    this->matchToken(utils::TokenKind::Colon, std::string(utils::PAR_ERROR_SPEC_WITHOUT_COLON));

    std::string spec_name = this->source.getTextFromPosition(spec_name_token.getPosition()).value();
    auto spec_bytecode = utils::APICA_SPECIFICATIONS.find(spec_name);
    if (spec_bytecode == utils::APICA_SPECIFICATIONS.end()) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::PAR_ERROR_SPEC_UNKNOWN_ID),
            spec_name_token.getPosition()
        ));

        return new nodes::NodeBad(spec_name_token.getPosition());
    }

    switch (spec_bytecode->second) {
        case common::bytecodes::ApicaSpecificationBytecode::Title:
            return this->parseSpecificationDataString(spec_bytecode->second, spec_name);
        
        case common::bytecodes::ApicaSpecificationBytecode::Id:
            return this->parseSpecificationDataString(spec_bytecode->second, spec_name);
        
        case common::bytecodes::ApicaSpecificationBytecode::LoggerActivation:
            return this->parseSpecificationDataBool(spec_bytecode->second, spec_name);
        
        case common::bytecodes::ApicaSpecificationBytecode::WindowWidth:
            return this->parseSpecificationDataU32(spec_bytecode->second, spec_name);
        
        case common::bytecodes::ApicaSpecificationBytecode::WindowHeight:
            return this->parseSpecificationDataU32(spec_bytecode->second, spec_name);
        
        case common::bytecodes::ApicaSpecificationBytecode::Version:
            return this->parseSpecificationDataString(spec_bytecode->second, spec_name);

        default: {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Error,
                std::string(utils::PAR_ERROR_SPEC_UNKNOWN_ID),
                spec_name_token.getPosition()
            ));

            return new nodes::NodeBad(spec_name_token.getPosition());
        }
    }
}

nodes::Node *Parser::parseSpecificationDataString(common::bytecodes::ApicaSpecificationBytecode bytecode, const std::string &name) {
    nodes::Node *value = this->parseStatement(ParserModifier::PM_InnerScope);
    if (value->getKind() != nodes::NodeKind::Literal || static_cast<nodes::NodeLiteral*>(value)->getValue()->getKind() != common::bytecodes::ApicaTypeBytecode::String) {
        std::string error_message(utils::PAR_ERROR_SPEC_STRING_INCORRECT);
        error_message += name;
        error_message += '`';

        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            error_message,
            value->getPosition()
        ));

        return value;
    }

    nodes::NodeLiteral *literal = static_cast<nodes::NodeLiteral*>(value);
    nodes::NodeDataString *data_string = new nodes::NodeDataString(
        value->getPosition(),
        bytecode,
        static_cast<common::values::ValueString*>(literal->getValue())->getValue().value()
    );

    delete value;
    return data_string;
}

nodes::Node *Parser::parseSpecificationDataU32(common::bytecodes::ApicaSpecificationBytecode bytecode, const std::string &name) {
    nodes::Node *value = this->parseStatement(ParserModifier::PM_InnerScope);
    if (value->getKind() != nodes::NodeKind::Literal || static_cast<nodes::NodeLiteral*>(value)->getValue()->getKind() != common::bytecodes::ApicaTypeBytecode::U32) {
        std::string error_message(utils::PAR_ERROR_SPEC_U32_INCORRECT);
        error_message += name;
        error_message += '`';

        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            error_message,
            value->getPosition()
        ));

        return value;
    }

    nodes::NodeLiteral *literal = static_cast<nodes::NodeLiteral*>(value);
    nodes::NodeDataU32 *data_u32 = new nodes::NodeDataU32(
        value->getPosition(),
        bytecode,
        static_cast<common::values::ValueU32*>(literal->getValue())->getValue().value()
    );

    delete value;
    return data_u32;
}

nodes::Node *Parser::parseSpecificationDataBool(common::bytecodes::ApicaSpecificationBytecode bytecode, const std::string &name) {
    nodes::Node *value = this->parseStatement(ParserModifier::PM_InnerScope);
    if (value->getKind() != nodes::NodeKind::Literal || static_cast<nodes::NodeLiteral*>(value)->getValue()->getKind() != common::bytecodes::ApicaTypeBytecode::Bool) {
        std::string error_message(utils::PAR_ERROR_SPEC_BOOL_INCORRECT);
        error_message += name;
        error_message += '`';

        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            error_message,
            value->getPosition()
        ));

        return value;
    }

    nodes::NodeLiteral *literal = static_cast<nodes::NodeLiteral*>(value);
    nodes::NodeDataBool *data_bool = new nodes::NodeDataBool(
        value->getPosition(),
        bytecode,
        static_cast<common::values::ValueBool*>(literal->getValue())->getValue().value()
    );

    delete value;
    return data_bool;
}

nodes::Node *Parser::createInteger(const utils::Token &token) const {
    std::optional<std::string> text = this->source.getTextFromPosition(token.getPosition());
    uint32_t integer = utils::decodeInteger(text.value());

    return new nodes::NodeLiteral(
        token.getPosition(),
        new common::values::ValueU32(integer)
    );
}

nodes::Node *Parser::createBinaryInteger(const utils::Token &token) const {
    std::optional<std::string> text = this->source.getTextFromPosition(token.getPosition());
    uint32_t integer = utils::decodeBinary(text.value());

    return new nodes::NodeLiteral(
        token.getPosition(),
        new common::values::ValueU32(integer)
    );
}

nodes::Node *Parser::createOctalInteger(const utils::Token &token) const {
    std::optional<std::string> text = this->source.getTextFromPosition(token.getPosition());
    uint32_t integer = utils::decodeOctal(text.value());

    return new nodes::NodeLiteral(
        token.getPosition(),
        new common::values::ValueU32(integer)
    );
}

nodes::Node *Parser::createHexadecimalInteger(const utils::Token &token) const {
    std::optional<std::string> text = this->source.getTextFromPosition(token.getPosition());
    uint32_t integer = utils::decodeHexadecimal(text.value());

    return new nodes::NodeLiteral(
        token.getPosition(),
        new common::values::ValueU32(integer)
    );
}

nodes::Node *Parser::createBigInteger(const utils::Token &token) const {
    std::optional<std::string> text = this->source.getTextFromPosition(token.getPosition());
    uint64_t big_integer = utils::decodeBigInteger(text.value());

    return new nodes::NodeLiteral(
        token.getPosition(),
        new common::values::ValueU64(big_integer)
    );
}

nodes::Node *Parser::createBigBinaryInteger(const utils::Token &token) const {
    std::optional<std::string> text = this->source.getTextFromPosition(token.getPosition());
    uint64_t big_integer = utils::decodeBigBinary(text.value());

    return new nodes::NodeLiteral(
        token.getPosition(),
        new common::values::ValueU64(big_integer)
    );
}

nodes::Node *Parser::createBigOctalInteger(const utils::Token &token) const {
    std::optional<std::string> text = this->source.getTextFromPosition(token.getPosition());
    uint64_t big_integer = utils::decodeBigOctal(text.value());

    return new nodes::NodeLiteral(
        token.getPosition(),
        new common::values::ValueU64(big_integer)
    );
}

nodes::Node *Parser::createBigHexadecimalInteger(const utils::Token &token) const {
    std::optional<std::string> text = this->source.getTextFromPosition(token.getPosition());
    uint64_t big_integer = utils::decodeBigHexadecimal(text.value());

    return new nodes::NodeLiteral(
        token.getPosition(),
        new common::values::ValueU64(big_integer)
    );
}

nodes::Node *Parser::createDecimal(const utils::Token &token) const {
    std::optional<std::string> text = this->source.getTextFromPosition(token.getPosition());
    float decimal = utils::decodeF32(text.value());

    return new nodes::NodeLiteral(
        token.getPosition(),
        new common::values::ValueF32(decimal)
    );
}

nodes::Node *Parser::createBigDecimal(const utils::Token &token) const {
    std::optional<std::string> text = this->source.getTextFromPosition(token.getPosition());
    double decimal = utils::decodeF64(text.value());

    return new nodes::NodeLiteral(
        token.getPosition(),
        new common::values::ValueF64(decimal)
    );
}

nodes::Node *Parser::createCharacter(const utils::Token &token) const {
    std::optional<std::string> text = this->source.getTextFromPosition(token.getPosition());
    uint32_t character = utils::decodeCharacter(text.value());

    return new nodes::NodeLiteral(
        token.getPosition(),
        new common::values::ValueChar(character)
    );
}

nodes::Node *Parser::createString(const utils::Token &token) const {
    std::optional<std::string> text = this->source.getTextFromPosition(token.getPosition());
    std::string str = utils::decodeString(text.value());

    return new nodes::NodeLiteral(
        token.getPosition(),
        new common::values::ValueString(str)
    );
}

nodes::Node *Parser::createRawString(const utils::Token &token) const {
    std::optional<std::string> text = this->source.getTextFromPosition(token.getPosition());

    return new nodes::NodeLiteral(
        token.getPosition(),
        new common::values::ValueString(text.value())
    );
}
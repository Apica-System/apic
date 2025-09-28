#include "core/parser.hpp"

namespace core {
    Parser::Parser(utils::SourceText *source, utils::DiagnosticBag *diag_bag) {
        this->diag_bag = diag_bag;
        this->source = source;
        this->index = 0;
        
        this->PerformLexer(source);
    }

    Parser::~Parser() {
        for (utils::Token *tok : this->tokens)
            delete tok;
    }

    nodes::NodeCompound *Parser::Parse() {
        std::vector<nodes::Node*> nodes;

        nodes::Node *actual = this->ParseStatement(ParserModifier::PM_FullStatement);
        while (actual->GetKind() != nodes::NodeKind::ND_EndOfFile) {
            nodes.push_back(actual);
            actual = this->ParseStatement(ParserModifier::PM_FullStatement);
        }

        return new nodes::NodeCompound(nodes);
    }

    void Parser::PerformLexer(utils::SourceText *source) {
        Lexer *lexer = new Lexer(source, diag_bag);

        utils::Token *actual = lexer->Lex();
        while (actual->GetKind() != utils::TokenKind::TOK_EndOfFile) {
            this->tokens.push_back(actual);
            actual = lexer->Lex();
        }

        this->tokens.push_back(actual);
        delete lexer;
    }

    void Parser::SkipNewLines() {
        utils::TokenKind actual = this->Get()->GetKind();
        while (actual == utils::TokenKind::TOK_NewLine || actual == utils::TokenKind::TOK_SemiColon) {
            this->Advance();
            actual = this->Get()->GetKind();
        }
    }

    void Parser::Advance() {
        if (this->index < this->tokens.size())
            this->index++;
    }

    utils::Token *Parser::Get() {
        if (this->index >= this->tokens.size())
            return this->tokens.back();
        
        return this->tokens[this->index];
    }

    utils::Token *Parser::GetAt(apic_u64 offset) {
        apic_u64 new_index = this->index + offset;
        if (new_index >= this->tokens.size())
            return this->tokens.back();
        
        return this->tokens[new_index];
    }

    utils::Token *Parser::GetAndAdvance() {
        utils::Token *actual = this->Get();
        this->Advance();
        
        return actual;
    }

    void Parser::MatchToken(utils::TokenKind expected, const apic_string &error_text) {
        utils::Token *actual = this->GetAndAdvance();
        if (actual->GetKind() != expected) {
            this->diag_bag->Add(new utils::Diagnostic(
                utils::DiagnosticKind::DIAG_Error,
                error_text,
                new utils::Position(actual->GetPosition())
            ));
        }
    }

    nodes::Node *Parser::ParseStatement(apic_u8 modifier) {
        this->SkipNewLines();
        utils::TokenKind current_kind = this->Get()->GetKind();

        switch (current_kind) {
            case utils::TokenKind::TOK_LeftBrace: return this->ParseCompound(modifier);
            case utils::TokenKind::TOK_Entrypoint: return this->ParseEntrypoint();

            default: break;
        }

        nodes::Node *result = this->ParseController(modifier);
        if (!result)
            result = this->ParseBinaryUnaryExpression(0);
        
        nodes::NodeKind result_kind = result->GetKind();
        if (modifier & ParserModifier::PM_FullStatement && result_kind != nodes::NodeKind::ND_Bad && result_kind != nodes::NodeKind::ND_EndOfFile) {
            utils::Token *end_token = this->Get();
            utils::TokenKind end_token_kind = end_token->GetKind();
            if (end_token_kind != utils::TokenKind::TOK_NewLine && end_token_kind != utils::TokenKind::TOK_SemiColon && end_token_kind != utils::TokenKind::TOK_EndOfFile) {
                this->diag_bag->Add(new utils::Diagnostic(
                    utils::DiagnosticKind::DIAG_Error,
                    "ParserError: A new-line or `;` is expected to separate two statements",
                    new utils::Position(end_token->GetPosition())
                ));

                return result;
            }

            this->Advance();
        }

        return result;
    }

    nodes::Node *Parser::ParseCompound(apic_u8 modifier) {
        this->Advance();
        this->SkipNewLines();
        std::vector<nodes::Node*> nodes;
        apic_u8 new_modifier = modifier | ParserModifier::PM_FullStatement;

        utils::TokenKind current = this->Get()->GetKind();
        while (current != utils::TokenKind::TOK_RightBrace && current != utils::TokenKind::TOK_EndOfFile) {
            nodes.push_back(this->ParseStatement(new_modifier));
            this->SkipNewLines();
            current = this->Get()->GetKind();
        }

        this->MatchToken(utils::TokenKind::TOK_RightBrace, "ParserError: Expected `}` to end a block of statements");
        return new nodes::NodeCompound(nodes);
    }

    nodes::Node *Parser::ParseController(apic_u8 modifier) {
        utils::TokenKind current = this->Get()->GetKind();
        if (current == utils::TokenKind::TOK_Return && modifier & ParserModifier::PM_FunctionScope) {
            nodes::Node *expression = nullptr;
            utils::TokenKind next = this->GetAt(1)->GetKind();
            if (next != utils::TokenKind::TOK_NewLine && next != utils::TokenKind::TOK_SemiColon && next != utils::TokenKind::TOK_EndOfFile)
                expression = this->ParseStatement(ParserModifier::PM_None);
            
            return new nodes::NodeReturn(expression);
        } else if (modifier & ParserModifier::PM_LoopScope) {
            if (current == utils::TokenKind::TOK_Break)
                return new nodes::NodeBreak();
            else if (current == utils::TokenKind::TOK_Continue)
                return new nodes::NodeContinue();
        }

        return nullptr;
    }

    nodes::Node *Parser::ParseBinaryUnaryExpression(apic_u8 precedence) {
        this->SkipNewLines();
        nodes::Node *left = nullptr;
        apic_u8 unary_precedence = utils::Token::GetUnaryPrecedence(this->Get()->GetKind());
        if (unary_precedence && unary_precedence >= precedence) {
            utils::Token *current = this->GetAndAdvance();
            nodes::Node *operand = this->ParseBinaryUnaryExpression(unary_precedence);
            left = new nodes::NodeUnary(operand, current->GetKind());
        } else
            left = this->ParsePrimaryExpression();
        
        while (true) {
            utils::Token *current = this->Get();
            apic_u8 binary_precedence = utils::Token::GetBinaryPrecedence(current->GetKind());
            if (!binary_precedence && binary_precedence <= precedence)
                break;
            
            this->Advance();
            if (binary_precedence == 14)
                return new nodes::NodeBinary(left, nullptr, current->GetKind());

            nodes::Node *right = this->ParseBinaryUnaryExpression(binary_precedence);
            left = new nodes::NodeBinary(left, right, current->GetKind());
        }

        return left;
    }

    nodes::Node *Parser::ParsePrimaryExpression() {
        utils::Token *current = this->GetAndAdvance();
        switch (current->GetKind()) {
            case utils::TokenKind::TOK_EndOfFile:
                return new nodes::NodeEndOfFile();
            
            case utils::TokenKind::TOK_Bad:
                return new nodes::NodeBad();
            
            case utils::TokenKind::TOK_Null:
                return new nodes::NodeLiteral(new values::ValueNull());
            
            case utils::TokenKind::TOK_LeftParenthesis: {
                nodes::Node *expression = this->ParseStatement(ParserModifier::PM_None);
                this->MatchToken(utils::TokenKind::TOK_RightParenthesis, "ParserError: Expected `)` to end a parenthesized expression");
                return new nodes::NodeParenthesized(expression);
            }

            case utils::TokenKind::TOK_Question:
                return this->ParseTernaryExpression();

            case utils::TokenKind::TOK_Integer: {
                apic_string text = this->source->GetTextFromPosition(current->GetPosition());
                apic_u32 result = utils::decode_integer(text);
                return new nodes::NodeLiteral(new values::ValueU32(result));
            }

            case utils::TokenKind::TOK_Binary: {
                apic_string text = this->source->GetTextFromPosition(current->GetPosition());
                apic_u32 result = utils::decode_binary(text);
                return new nodes::NodeLiteral(new values::ValueU32(result));
            }

            case utils::TokenKind::TOK_String: {
                apic_string text = this->source->GetTextFromPosition(current->GetPosition());
                apic_string str = utils::decode_string(text);
                return new nodes::NodeLiteral(new values::ValueString(str));
            }

            case utils::TokenKind::TOK_Identifier: {
                switch (this->Get()->GetKind()) {
                    case utils::TokenKind::TOK_Colon:
                        return this->ParsePackageCall(current->GetPosition());
                    
                    case utils::TokenKind::TOK_LeftParenthesis:
                        return this->ParseFuncCall(current->GetPosition());
                    
                    default: {
                        this->diag_bag->Add(new utils::Diagnostic(
                            utils::DiagnosticKind::DIAG_Error,
                            "ParserError: Variable and constant calls are not yet supported",
                            new utils::Position(current->GetPosition())
                        ));
                        return new nodes::NodeBad();
                    }
                }
            }

            default: {
                this->diag_bag->Add(new utils::Diagnostic(
                    utils::DiagnosticKind::DIAG_Error,
                    "ParserError: Incorrect token found",
                    new utils::Position(current->GetPosition())
                ));

                return new nodes::NodeBad();
            }
        }
    }

    nodes::Node *Parser::ParseTernaryExpression() {
        nodes::Node *question = this->ParseStatement(ParserModifier::PM_None);
        this->SkipNewLines();
        this->MatchToken(utils::TokenKind::TOK_Colon, "ParserError: Expected `:` to separate first result from the condition");

        nodes::Node *first = this->ParseStatement(ParserModifier::PM_None);
        this->SkipNewLines();
        this->MatchToken(utils::TokenKind::TOK_Colon, "ParserError: Expected `:` to separate second result from the first result");

        nodes::Node *second = this->ParseStatement(ParserModifier::PM_None);
        this->SkipNewLines();

        return new nodes::NodeTernary(question, first, second);
    }

    nodes::Node *Parser::ParseEntrypoint() {
        this->Advance();
        utils::Token *entry_token = this->GetAndAdvance();
        apic_string entry_name = this->source->GetTextFromPosition(entry_token->GetPosition());
        ApicaEntrypointBytecode entry_bytecode = ApicaEntrypointBytecode::AEB_Init;
        if (entry_name == "init")
            entry_bytecode = ApicaEntrypointBytecode::AEB_Init;
        else if (entry_name == "update")
            entry_bytecode = ApicaEntrypointBytecode::AEB_Update;
        else if (entry_name == "quit")
            entry_bytecode = ApicaEntrypointBytecode::AEB_Quit;
        else {
            this->diag_bag->Add(new utils::Diagnostic(
                utils::DiagnosticKind::DIAG_Error,
                "ParserError: Unknown entrypoint identifier",
                new utils::Position(entry_token->GetPosition())
            ));
            return new nodes::NodeBad();
        }

        utils::Token *brace_token = this->Get();
        if (brace_token->GetKind() != utils::TokenKind::TOK_LeftBrace) {
            this->diag_bag->Add(new utils::Diagnostic(
                utils::DiagnosticKind::DIAG_Error,
                "ParserError: All entrypoints should be followed by a block of statement"
            ));
            return new nodes::NodeBad();
        }

        nodes::Node *body = this->ParseStatement(ParserModifier::PM_None);
        return new nodes::NodeEntrypoint(entry_bytecode, body);
    }

    nodes::Node *Parser::ParsePackageCall(utils::Position *position) {
        this->Advance();
        apic_string name = this->source->GetTextFromPosition(position);
        nodes::Node *next = this->ParseStatement(ParserModifier::PM_None);
        return new nodes::NodePackageCall(name, next);
    }

    nodes::Node *Parser::ParseFuncCall(utils::Position *position) {
        this->Advance();
        apic_string func_name = this->source->GetTextFromPosition(position);
        utils::TokenKind actual_kind = this->Get()->GetKind();
        std::vector<nodes::NodeParameter*> parameters;

        while (actual_kind != utils::TokenKind::TOK_RightParenthesis && actual_kind != utils::TokenKind::TOK_EndOfFile) {
            if (!parameters.empty())
                this->MatchToken(utils::TokenKind::TOK_Comma, "ParserError: Expected a `,` between two parameters in a func call");
            
            parameters.push_back(this->ParseParameter());
            actual_kind = this->Get()->GetKind();
        }

        this->MatchToken(utils::TokenKind::TOK_RightParenthesis, "ParserError: Expected a `)` to end the function call");
        return new nodes::NodeFuncCall(func_name, new utils::Position(position), parameters);
    }

    nodes::NodeParameter *Parser::ParseParameter() {
        utils::TokenKind current_kind = this->Get()->GetKind();
        utils::TokenKind next_kind = this->GetAt(1)->GetKind();
        apic_string *name = nullptr;

        if (current_kind == utils::TokenKind::TOK_Identifier && next_kind == utils::TokenKind::TOK_Colon) {
            name = new apic_string(this->source->GetTextFromPosition(this->Get()->GetPosition()));
            this->Advance();
            this->Advance();
        }

        nodes::Node *expression = this->ParseStatement(ParserModifier::PM_None);
        return new nodes::NodeParameter(name, expression);
    }
}
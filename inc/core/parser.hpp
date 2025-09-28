#pragma once

#include "core/lexer.hpp"

namespace core {
    enum ParserModifier : apic_u8 {
        PM_None =           0b00000000,
        PM_FullStatement =  0b00000001,
        PM_FunctionScope =  0b00000010,
        PM_LoopScope =      0b00000100
    };

    class Parser {
    public:
        Parser(utils::SourceText *source, utils::DiagnosticBag *diag_bag);
        ~Parser();

        nodes::NodeCompound *Parse();
    private:
        std::vector<utils::Token*> tokens;
        apic_u64 index;
        utils::DiagnosticBag *diag_bag;
        utils::SourceText *source;

        void PerformLexer(utils::SourceText *source);
        void SkipNewLines();
        void Advance();
        utils::Token *Get();
        utils::Token *GetAt(apic_u64 offset);
        utils::Token *GetAndAdvance();
        void MatchToken(utils::TokenKind expected, const apic_string &error_text);
    
        nodes::Node *ParseStatement(apic_u8 modifier);
        nodes::Node *ParseCompound(apic_u8 modifier);
        nodes::Node *ParseController(apic_u8 modifier);
        nodes::Node *ParseBinaryUnaryExpression(apic_u8 precedence);
        nodes::Node *ParsePrimaryExpression();
        nodes::Node *ParseTernaryExpression();
        nodes::Node *ParseEntrypoint();
        nodes::Node *ParsePackageCall(utils::Position *position);
        nodes::Node *ParseFuncCall(utils::Position *position);
        nodes::NodeParameter *ParseParameter();
    };
}
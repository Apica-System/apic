#pragma once

#include "nodes/var_const_call.hpp"

namespace core {
    extern std::unordered_map<std::string, utils::TokenKind> _keywords;
    void init_keywords();

    class Lexer {
    public:
        Lexer(utils::SourceText *source, utils::DiagnosticBag *diag_bag);
        ~Lexer();

        utils::Token *Lex();
    private:
        utils::SourceText *source;
        utils::DiagnosticBag *diag_bag;
        apic_u64 ascii_index, utf8_index;
        apic_u8 utf8_awaited;
        apic_char current_char;

        void SkipSpaces();
        void SkipSpecials();
        void SkipComments(apic_bool is_multi);
        void SkipSpecialCharacter();

        void Advance();
        utils::Token *AdvanceWith(utils::TokenKind kind, apic_u64 size);

        utils::Token *LexBasicToken(apic_char next);
        utils::Token *LexWord();
        utils::Token *LexCharacter();
        utils::Token *LexString();
        utils::Token *LexRawString();
        utils::Token *LexFormattedString();
        utils::Token *LexMacro();
        utils::Token *LexNumber();
        utils::Token *LexIncorrectNumber(apic_u64 ascii_start, apic_u64 utf8_start, const apic_string &message);
        utils::Token *LexIncorrectNumberEnding(apic_u64 ascii_start, apic_u64 utf8_start, const apic_string &message);
    };
}
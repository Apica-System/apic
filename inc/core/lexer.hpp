#pragma once

#include "utils/source.hpp"
#include "utils/token.hpp"

namespace core {
    enum LexerNumberKind : uint8_t {
        Integer, Binary, Octal,
        Hexadecimal, Decimal,

        BigInteger, BigBinary, BigOctal,
        BigHexadecimal, BigDecimal,
    };

    class Lexer final {
    public:
        Lexer(const utils::SourceText &source);

        utils::Token lex();
    private:
        const utils::SourceText &source;
        uint64_t ascii_index, utf8_index;
        uint8_t utf8_awaited_length;

        void advance();
        utils::Token advanceWith(utils::TokenKind kind, uint64_t size);
        
        void skipSpaces();
        void skipComments(bool is_multiline);
        void skipSpecialCharacter();

        char get() const;
        char getAt(uint64_t offset) const;

        utils::Token createIncorrectToken(uint64_t ascii_start, uint64_t utf8_start, const std::string &message);
        utils::Token createIncorrectNumberToken(uint64_t ascii_start, uint64_t utf8_start, const std::string &message);

        std::optional<utils::Token> lexBasicToken(char current, char next);
        utils::Token lexWord();
        utils::Token lexNumber();
        utils::Token lexMacro();
        utils::Token lexCharacter();
        utils::Token lexString();
        utils::Token lexRawString();
        utils::Token lexFormattedString();
    };
}
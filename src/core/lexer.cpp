#include "core/lexer.hpp"
#include "utils/chars.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/errors.hpp"
#include "utils/builtins.hpp"

using namespace core;

Lexer::Lexer(const utils::SourceText &source)
    : source(source), ascii_index(0), utf8_index(0) {
    this->utf8_awaited_length = utils::getUtf8Length(source.getCharAt(0));
}

utils::Token Lexer::lex() {
    this->skipSpaces();
    char current = this->get();
    if (current == '\0')
        return utils::Token(utils::TokenKind::EndOfFile, utils::Position(this->ascii_index, 1, this->utf8_index, 1));
    
    char next = this->getAt(1);
    if (current == '/') {
        if (next == '/' || next == '*') {
            this->skipComments(next == '*');        
            return this->lex();
        }

        if (next == '=') return this->advanceWith(utils::TokenKind::SlashEquals, 2);
        return this->advanceWith(utils::TokenKind::Slash, 1);
    }

    std::optional<utils::Token> basic_token = this->lexBasicToken(current, next);
    if (basic_token)
        return basic_token.value();

    if (isdigit(current)) return this->lexNumber();
    else if (utils::isIdPart(current)) return this->lexWord();

    utils::Position error_pos(this->ascii_index, 1, this->utf8_index, 1);
    this->advance();
    utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
        utils::DiagnosticKind::Error,
        std::string(utils::LEX_ERROR_INCORRECT_CHARACTER_FOUND),
        error_pos
    ));

    return utils::Token(utils::TokenKind::Bad, error_pos);
}

void Lexer::advance() {
    this->ascii_index++;
    this->utf8_awaited_length--;
    if (this->utf8_awaited_length == 0) {
        this->utf8_awaited_length = utils::getUtf8Length(this->source.getCharAt(this->ascii_index));
        this->utf8_index++;
    }
}

utils::Token Lexer::advanceWith(utils::TokenKind kind, uint64_t size) {
    uint64_t old_ascii_pos = this->ascii_index;
    uint64_t old_utf8_pos = this->utf8_index;
    for (uint64_t i = 0; i < size; i++)
        this->advance();
    
    return utils::Token(kind, utils::Position(old_ascii_pos, this->ascii_index - old_ascii_pos, old_utf8_pos, this->utf8_index - old_utf8_pos));
}

void Lexer::skipSpaces() {
    char current = this->get();
    while (isspace(current) && current != '\n') {
        this->advance();
        current = this->get();
    }
}

void Lexer::skipComments(bool is_multiline) {
    this->advance();
    this->advance();

    if (is_multiline) {
        uint64_t old_ascii_pos = this->ascii_index;
        uint64_t old_utf8_pos = this->utf8_index;

        char current = this->get();
        while (current != '\0') {
            if (current == '*') {
                char next = this->getAt(1);
                if (next == '/') {
                    this->advance();
                    this->advance();
                    return;
                }
            }

            this->advance();
            current = this->get();
        }

        if (current == '\0') {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Error,
                std::string(utils::LEX_ERROR_UNTERMINATED_MULTILINE_COMMENT),
                utils::Position(old_ascii_pos, this->ascii_index - old_ascii_pos, old_utf8_pos, this->utf8_index - old_utf8_pos)
            ));
        }
    } else {
        char current = this->get();
        while (current != '\n' && current != '\0') {
            this->advance();
            current = this->get();
        }
    }
}

void Lexer::skipSpecialCharacter() {
    this->advance();
    char old_char = this->get();
    this->advance();

    switch (old_char) {
        case 'B': {
            char current = this->get();
            if (current < '0' || current > '1') {
                utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                    utils::DiagnosticKind::Error,
                    std::string(utils::LEX_ERROR_INCORRECT_BINARY_REPR),
                    utils::Position(this->ascii_index - 2, 2, this->utf8_index - 2, 2)
                ));
                return;
            }

            this->advance();
            current = this->get();
            for (uint8_t i = 0; i < 31; i++) {
                if (current < '0' || current > '1') break;
                this->advance();
                current = this->get();
            }
        } break;

        case 'o': {
            char current = this->get();
            if (current < '0' || current > '7') {
                utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                    utils::DiagnosticKind::Error,
                    std::string(utils::LEX_ERROR_INCORRECT_OCTAL_REPR),
                    utils::Position(this->ascii_index - 2, 2, this->utf8_index - 2, 2)
                ));
                return;
            }

            uint8_t limit = current <= '3' ? 10 : 9;
            this->advance();
            current = this->get();
            for (uint8_t i = 0; i < limit; i++) {
                if (current < '0' || current > '7') break;
                this->advance();
                current = this->get();
            }            
        } break;

        case 'u': case 'x': {
            char current = this->get();
            if (!isxdigit(current)) {
                utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                    utils::DiagnosticKind::Error,
                    std::string(utils::LEX_ERROR_INCORRECT_HEXA_REPR),
                    utils::Position(this->ascii_index - 2, 2, this->utf8_index - 2, 2)
                ));
                return;
            }

            this->advance();
            current = this->get();
            for (uint8_t i = 0; i < 7; i++) {
                if (!isxdigit(current)) break;
                this->advance();
                current = this->get();
            }
        } break;

        default: break;;
    }
}

char Lexer::get() const {
    return this->source.getCharAt(this->ascii_index);
}

char Lexer::getAt(uint64_t offset) const {
    return this->source.getCharAt(this->ascii_index + offset);
}

utils::Token Lexer::createIncorrectToken(uint64_t ascii_start, uint64_t utf8_start, const std::string &message) {
    utils::Position error_pos(ascii_start, this->ascii_index - ascii_start, utf8_start, this->utf8_index - utf8_start);
    utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
        utils::DiagnosticKind::Error,
        message,
        error_pos
    ));

    return utils::Token(utils::TokenKind::Bad, error_pos);
}

utils::Token Lexer::createIncorrectNumberToken(uint64_t ascii_start, uint64_t utf8_start, const std::string &message) {
    char current = this->get();
    while (isxdigit(current) || current == '_' || current == '.') {
        if (current == 'e' || current == 'E') {
            char next = this->get();
            if (next == '+' || next == '-') this->advance();
        }

        this->advance();
        current = this->get();
    }

    if (current == 'i' || current == 'l')
        this->advance();
    
    utils::Position error_pos(ascii_start, this->ascii_index - ascii_start, utf8_start, this->utf8_index - utf8_start);
    utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
        utils::DiagnosticKind::Error,
        message,
        error_pos
    ));

    return utils::Token(utils::TokenKind::Bad, error_pos);
}

std::optional<utils::Token> Lexer::lexBasicToken(char current, char next) {
    switch (current) {
        case '\n': return this->advanceWith(utils::TokenKind::NewLine, 1);
        case '#': return this->lexMacro();
        case '\'': return this->lexCharacter();
        case '"': return this->lexString();
        case '`': return this->lexRawString();
        case '$': return this->lexFormattedString();

        case '+': {
            if (next == '+') return this->advanceWith(utils::TokenKind::PlusPlus, 2);
            else if (next == '=') return this->advanceWith(utils::TokenKind::PlusEquals, 2);
            return this->advanceWith(utils::TokenKind::Plus, 1);
        }
        
        case '-': {
            if (next == '-') return this->advanceWith(utils::TokenKind::MinusMinus, 2);
            else if (next == '=') return this->advanceWith(utils::TokenKind::MinusEquals, 2);
            return this->advanceWith(utils::TokenKind::Minus, 1);
        }

        case '*': {
            if (next == '=') return this->advanceWith(utils::TokenKind::StarEquals, 2);
            return this->advanceWith(utils::TokenKind::Star, 1);
        }

        case '%': {
            if (next == '=') return this->advanceWith(utils::TokenKind::PercentEquals, 2);
            return this->advanceWith(utils::TokenKind::Percent, 1);
        }

        case '=': {
            if (next == '=') return this->advanceWith(utils::TokenKind::EqualsEquals, 2);
            return this->advanceWith(utils::TokenKind::Equals, 1);
        }

        case '!': {
            if (next == '=') return this->advanceWith(utils::TokenKind::BangEquals, 2);
            return this->advanceWith(utils::TokenKind::Bang, 1);
        }
        
        case '^': {
            if (next == '=') return this->advanceWith(utils::TokenKind::HatEquals, 2);
            return this->advanceWith(utils::TokenKind::Hat, 1);
        }

        case '@': {
            if (next == '=') return this->advanceWith(utils::TokenKind::AtEquals, 2);
            else if (next == '<') return this->advanceWith(utils::TokenKind::TypeParameter, 2);
            return this->advanceWith(utils::TokenKind::At, 1);
        }

        case '&': {
            if (next == '&') return this->advanceWith(utils::TokenKind::AmpersandAmpersand, 2);
            else if (next == '=') return this->advanceWith(utils::TokenKind::AmpersandEquals, 2);
            return this->advanceWith(utils::TokenKind::Ampersand, 1);
        }
        
        case '|': {
            if (next == '|') return this->advanceWith(utils::TokenKind::PipePipe, 2);
            else if (next == '=') return this->advanceWith(utils::TokenKind::PipeEquals, 2);
            return this->advanceWith(utils::TokenKind::Pipe, 1);
        }

        case '<': {
            if (next == '<') {
                if (this->source.getCharAt(this->ascii_index + 2) == '=')
                    return this->advanceWith(utils::TokenKind::LeftShiftEquals, 3);
                return this->advanceWith(utils::TokenKind::LeftShift, 2);
            } else if (next == '=')
                return this->advanceWith(utils::TokenKind::LessEquals, 2);
            return this->advanceWith(utils::TokenKind::Less, 1);
        }
        
        case '>': {
            if (next == '>') {
                if (this->source.getCharAt(this->ascii_index + 2) == '=')
                    return this->advanceWith(utils::TokenKind::RightShiftEquals, 3);
                return this->advanceWith(utils::TokenKind::RightShift, 2);
            } else if (next == '=')
                return this->advanceWith(utils::TokenKind::GreaterEquals, 2);
            return this->advanceWith(utils::TokenKind::Greater, 1);
        }

        case '?': {
            if (next == '.') return this->advanceWith(utils::TokenKind::QuestionDot, 2);
            return this->advanceWith(utils::TokenKind::Question, 1);
        }

        case ':': {
            if (next == ':') return this->advanceWith(utils::TokenKind::DoubleColon, 2);
            return this->advanceWith(utils::TokenKind::Colon, 1);
        }

        case '(': return this->advanceWith(utils::TokenKind::LeftParenthesis, 1);
        case ')': return this->advanceWith(utils::TokenKind::RightParenthesis, 1);
        case '[': return this->advanceWith(utils::TokenKind::LeftBracket, 1);
        case ']': return this->advanceWith(utils::TokenKind::RightBracket, 1);
        case '{': return this->advanceWith(utils::TokenKind::LeftBrace, 1);
        case '}': return this->advanceWith(utils::TokenKind::RightBrace, 1);
        case ';': return this->advanceWith(utils::TokenKind::SemiColon, 1);
        case ',': return this->advanceWith(utils::TokenKind::Comma, 1);
        case '.': return this->advanceWith(utils::TokenKind::Dot, 1);
        case '~': return this->advanceWith(utils::TokenKind::Tilde, 1);

        default: return std::nullopt;
    }
}

utils::Token Lexer::lexWord() {
    uint64_t old_ascii_pos = this->ascii_index;
    uint64_t old_utf8_pos = this->utf8_index;

    std::string word;
    char current = this->get();
    while (utils::isIdPart(current)) {
        word += current;
        this->advance();
        current = this->get();
    }

    utils::TokenKind kind = utils::TokenKind::Identifier;
    auto keyword = utils::APICA_KEYWORDS.find(word);
    if (keyword != utils::APICA_KEYWORDS.end())
        kind = keyword->second;

    return utils::Token(kind, utils::Position(old_ascii_pos, this->ascii_index - old_ascii_pos, old_utf8_pos, this->utf8_index - old_utf8_pos));
}

utils::Token Lexer::lexNumber() {
    uint64_t old_ascii_pos = this->ascii_index;
    uint64_t old_utf8_pos = this->utf8_index;
    bool dots = false, exps = false, awaiting = false;
    LexerNumberKind number_kind = LexerNumberKind::Integer;

    char current = this->get();
    this->advance();
    if (current == '0') {
        char next = this->get();
        if (next == 'b') number_kind = LexerNumberKind::Binary;
        else if (next == 'o') number_kind = LexerNumberKind::Octal;
        else if (next == 'x') number_kind = LexerNumberKind::Hexadecimal;

        if (next != LexerNumberKind::Integer)
            this->advance();
    }

    current = this->get();
    while (isxdigit(current) || current == '_' || current == '.') {
        if (current == '_') {
            // Do nothing
        } else if (number_kind != LexerNumberKind::Hexadecimal && (current == 'e' || current == 'E')) {
            if (number_kind != LexerNumberKind::Integer && number_kind != LexerNumberKind::Decimal)
                return this->createIncorrectNumberToken(old_ascii_pos, old_utf8_pos, std::string(utils::LEX_ERROR_HEXA_OCTAL_BINARY_CANT_BE_EXP));
            if (exps)
                return this->createIncorrectNumberToken(old_ascii_pos, old_utf8_pos, std::string(utils::LEX_ERROR_EXP_NUMBER_TOO_MANY_EXP));
            if (awaiting)
                return this->createIncorrectNumberToken(old_ascii_pos, old_utf8_pos, std::string(utils::LEX_ERROR_EXP_NUMBER_POINT_RIGHT_BEFORE));
            
            number_kind = LexerNumberKind::Decimal;
            exps = true;
            char sign = this->getAt(1);
            if (sign != '+' && sign != '-')
                return this->createIncorrectNumberToken(old_ascii_pos, old_utf8_pos, std::string(utils::LEX_ERROR_EXP_NUMBER_NEED_PLUS_OR_MINUS));
            
            this->advance();
            this->advance();
            awaiting = true;

            current = this->get();
            continue;
        } else if (current == '.') {
            if (number_kind != LexerNumberKind::Integer) {
                return (number_kind != LexerNumberKind::Decimal)
                    ? this->createIncorrectNumberToken(old_ascii_pos, old_utf8_pos, std::string(utils::LEX_ERROR_HEXA_OCTAL_BINARY_CANT_BE_DECIMAL))
                    : this->createIncorrectNumberToken(old_ascii_pos, old_utf8_pos, std::string(utils::LEX_ERROR_POINT_AFTER_EXP_NUMBER));
            }

            if (dots)
                return this->createIncorrectNumberToken(old_ascii_pos, old_utf8_pos, std::string(utils::LEX_ERROR_DECIMAL_NUMBER_TOO_MANY_POINTS));
            
            number_kind = LexerNumberKind::Decimal;
            dots = true;
            awaiting = true;
        } else if (number_kind == LexerNumberKind::Binary && (current < '0' || current > '1')) {
            return this->createIncorrectNumberToken(old_ascii_pos, old_utf8_pos, std::string(utils::LEX_ERROR_BINARY_NUMBER_INCORRECT));
        } else if (number_kind == LexerNumberKind::Octal && (current < '0' || current > '7')) {
            return this->createIncorrectNumberToken(old_ascii_pos, old_utf8_pos, std::string(utils::LEX_ERROR_OCTAL_NUMBER_INCORRECT));
        } else if (number_kind != LexerNumberKind::Hexadecimal && current > 'A') {
            return this->createIncorrectNumberToken(old_ascii_pos, old_utf8_pos, std::string(utils::LEX_ERROR_ONLY_HEXA_CONTAIN_A_TO_F));
        }

        if (awaiting)
            awaiting = current == '_';
        this->advance();
        current = this->get();
    }

    if (awaiting)
        return this->createIncorrectToken(old_ascii_pos, old_utf8_pos, std::string(utils::LEX_ERROR_POINT_OR_EXP_AWAITING_IN_NUMBER));
    
    if (current == 'i') {
        this->advance();
        if (number_kind == LexerNumberKind::Decimal)
            return this->createIncorrectToken(old_ascii_pos, old_utf8_pos, std::string(utils::LEX_ERROR_DECIMAL_NUMBER_CANT_HAVE_MOD_I));
        
        switch (number_kind) {
            case LexerNumberKind::Binary: number_kind = LexerNumberKind::BigBinary; break;
            case LexerNumberKind::Octal: number_kind = LexerNumberKind::BigOctal; break;
            case LexerNumberKind::Hexadecimal: number_kind = LexerNumberKind::BigHexadecimal; break;
            default: number_kind = LexerNumberKind::BigInteger; break;
        }
    } else if (current == 'l') {
        this->advance();
        if (number_kind != LexerNumberKind::Integer && number_kind != LexerNumberKind::Decimal)
            return this->createIncorrectToken(old_ascii_pos, old_utf8_pos, std::string(utils::LEX_ERROR_HEXA_OCTAL_BINARY_CANT_HAVE_MOD_L));
        
        number_kind = LexerNumberKind::BigDecimal;
    }

    switch (number_kind) {
        case LexerNumberKind::Binary: return utils::Token(utils::TokenKind::Binary, utils::Position(old_ascii_pos + 2, this->ascii_index - (old_ascii_pos + 2), old_utf8_pos + 2, this->utf8_index - (old_utf8_pos + 2)));
        case LexerNumberKind::Octal: return utils::Token(utils::TokenKind::Octal, utils::Position(old_ascii_pos + 2, this->ascii_index - (old_ascii_pos + 2), old_utf8_pos + 2, this->utf8_index - (old_utf8_pos + 2)));
        case LexerNumberKind::Hexadecimal: return utils::Token(utils::TokenKind::Hexadecimal, utils::Position(old_ascii_pos + 2, this->ascii_index - (old_ascii_pos + 2), old_utf8_pos + 2, this->utf8_index - (old_utf8_pos + 2)));
        case LexerNumberKind::Decimal: return utils::Token(utils::TokenKind::Decimal, utils::Position(old_ascii_pos, this->ascii_index - old_ascii_pos, old_utf8_pos, this->utf8_index - old_utf8_pos));

        case LexerNumberKind::BigInteger: return utils::Token(utils::TokenKind::BigInteger, utils::Position(old_ascii_pos, this->ascii_index - (old_ascii_pos + 1), old_utf8_pos, this->utf8_index - (old_utf8_pos + 1)));
        case LexerNumberKind::BigBinary: return utils::Token(utils::TokenKind::BigBinary, utils::Position(old_ascii_pos + 2, this->ascii_index - (old_ascii_pos + 3), old_utf8_pos + 2, this->utf8_index - (old_utf8_pos + 3)));
        case LexerNumberKind::BigOctal: return utils::Token(utils::TokenKind::BigOctal, utils::Position(old_ascii_pos + 2, this->ascii_index - (old_ascii_pos + 3), old_utf8_pos + 2, this->utf8_index - (old_utf8_pos + 3)));
        case LexerNumberKind::BigHexadecimal: return utils::Token(utils::TokenKind::BigHexadecimal, utils::Position(old_ascii_pos + 2, this->ascii_index - (old_ascii_pos + 3), old_utf8_pos + 2, this->utf8_index - (old_utf8_pos + 3)));
        case LexerNumberKind::BigDecimal: return utils::Token(utils::TokenKind::BigDecimal, utils::Position(old_ascii_pos, this->ascii_index - (old_ascii_pos + 1), old_utf8_pos, this->utf8_index - (old_utf8_pos + 1)));
    
        case LexerNumberKind::Integer: default: return utils::Token(utils::TokenKind::Integer, utils::Position(old_ascii_pos, this->ascii_index - old_ascii_pos, old_utf8_pos, this->utf8_index - old_utf8_pos));
    }
}

utils::Token Lexer::lexMacro() {
    uint64_t old_ascii_pos = this->ascii_index;
    uint64_t old_utf8_pos = this->utf8_index;
    this->advance();

    char current = this->get();
    if (!utils::isIdPart(current))
        return this->createIncorrectToken(old_ascii_pos, old_utf8_pos, std::string(utils::LEX_ERROR_MACRO_INCORRECT));
    
    while (utils::isIdPart(current)) {
        this->advance();
        current = this->get();
    }

    return utils::Token(utils::TokenKind::Macro, utils::Position(old_ascii_pos + 1, this->ascii_index - (old_ascii_pos + 1), old_utf8_pos + 1, this->utf8_index - (old_utf8_pos + 1)));
}

utils::Token Lexer::lexCharacter() {
    uint64_t old_ascii_pos = this->ascii_index;
    uint64_t old_utf8_pos = this->utf8_index;
    this->advance();

    char current = this->get();
    if (current == '\\')
        this->skipSpecialCharacter();
    else if (current == '\'')
        return this->createIncorrectToken(old_ascii_pos, old_utf8_pos, std::string(utils::LEX_ERROR_EMPTY_CHARACTER));
    else {
        uint8_t utf8_length = this->utf8_awaited_length;
        for (uint8_t i = 0; i < utf8_length; i++)
            this->advance();
    }
    
    current = this->get();
    if (current != '\'') {
        while (current != '\'' && current != '\0') {
            this->advance();
            current = this->get();
        }

        this->advance();
        return this->createIncorrectToken(old_ascii_pos, old_utf8_pos, std::string(utils::LEX_ERROR_UNTERMINATED_CHARACTER));
    }

    this->advance();
    return utils::Token(utils::TokenKind::Character, utils::Position(old_ascii_pos + 1, this->ascii_index - (old_ascii_pos + 2), old_utf8_pos + 1, this->utf8_index - (old_utf8_pos + 2)));
}

utils::Token Lexer::lexString() {
    uint64_t old_ascii_pos = this->ascii_index;
    uint64_t old_utf8_pos = this->utf8_index;
    this->advance();

    char current = this->get();
    while (current != '"' && current != '\0') {
        if (current == '\\')
            this->skipSpecialCharacter();
        else
            this->advance();
        
        current = this->get();
    }

    if (current != '"')
        return this->createIncorrectToken(old_ascii_pos, old_utf8_pos, std::string(utils::LEX_ERROR_UNTERMINATED_STRING));
    
    this->advance();
    return utils::Token(utils::TokenKind::String, utils::Position(old_ascii_pos + 1, this->ascii_index - (old_ascii_pos + 2), old_utf8_pos + 1, this->utf8_index - (old_utf8_pos + 2)));
}

utils::Token Lexer::lexRawString() {
    uint64_t old_ascii_pos = this->ascii_index;
    uint64_t old_utf8_pos = this->utf8_index;
    this->advance();

    char current = this->get();
    while (current != '`' && current != '\0') {
        this->advance();
        current = this->get();
    }

    if (current != '`')
        return this->createIncorrectToken(old_ascii_pos, old_utf8_pos, std::string(utils::LEX_ERROR_UNTERMINATED_RAW_STRING));
    
    this->advance();
    return utils::Token(utils::TokenKind::RawString, utils::Position(old_ascii_pos + 1, this->ascii_index - (old_ascii_pos + 2), old_utf8_pos + 1, this->utf8_index - (old_utf8_pos + 2)));
}

utils::Token Lexer::lexFormattedString() {
    uint64_t old_ascii_pos = this->ascii_index;
    uint64_t old_utf8_pos = this->utf8_index;
    this->advance();

    utils::Token token = this->lex();
    if (token.getKind() == utils::TokenKind::Bad)
        return token;
    
    if (token.getKind() == utils::TokenKind::String)
        return utils::Token(utils::TokenKind::FormattedString, token.getPosition());
    
    return this->createIncorrectToken(old_ascii_pos, old_utf8_pos, std::string(utils::LEX_ERROR_FORMATTED_STRING_INCORRECT));
}
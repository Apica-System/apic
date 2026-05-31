#pragma once

#include "utils/position.hpp"

namespace utils {
    enum TokenKind : uint8_t {
        Bad, EndOfFile, NewLine,

        Plus, PlusPlus, PlusEquals,
        Minus, MinusMinus, MinusEquals,
        Star, StarEquals,
        Slash, SlashEquals,
        Equals, EqualsEquals,
        Bang, BangEquals,
        At, AtEquals,
        Ampersand, AmpersandAmpersand, AmpersandEquals,
        Pipe, PipePipe, PipeEquals,
        Tilde,
        Hat, HatEquals,
        Less, LessEquals,
        LeftShift, LeftShiftEquals,
        Greater, GreaterEquals,
        RightShift, RightShiftEquals,
        Percent, PercentEquals,

        Identifier, Macro, Character, String,
        RawString, FormattedString,
        Integer, BigInteger, Decimal, BigDecimal,
        Binary, Octal, Hexadecimal, BigBinary,
        BigOctal, BigHexadecimal,

        Var, Const, Func, Lambda,
        True, False, Null,
        Entrypoint, Specifications, Global,

        Break, Continue, Return,

        If, Else, Switch, Case, Default,
        For, Foreach, While, Do,
        Try, Catch, Finally, Throw,

        As, Typeof,

        Colon, DoubleColon, SemiColon,
        Comma, Dot,
        Question, QuestionDot,
        LeftParenthesis, RightParenthesis,
        LeftBrace, RightBrace,
        LeftBracket, RightBracket,
        TypeParameter, // @<
    };

    class Token final {
    public:
        Token(TokenKind kind, const Position &position);

        static uint8_t getBinaryPrecedence(TokenKind kind);
        static uint8_t getUnaryPrecedence(TokenKind kind);

        TokenKind getKind() const;
        Position getPosition() const;

        void show(char end) const;
    private:
        TokenKind kind;
        Position position;
    };
}
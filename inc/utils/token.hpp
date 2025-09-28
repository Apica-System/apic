#pragma once

#include "utils/position.hpp"

namespace utils {
    enum TokenKind : apic_u8 {
        TOK_Bad = 0, TOK_EndOfFile, TOK_NewLine,

        TOK_Plus, TOK_PlusPlus, TOK_PlusEquals, TOK_Minus, TOK_MinusMinus, TOK_MinusEquals,
        TOK_Star, TOK_StarEquals, TOK_Slash, TOK_SlashEquals, TOK_Equals, TOK_EqualsEquals,
        TOK_Bang, TOK_BangEquals, TOK_At, TOK_AtEquals, TOK_Ampersand, TOK_AmpersandAmpersand,
        TOK_AmpersandEquals, TOK_Pipe, TOK_PipePipe, TOK_PipeEquals, TOK_Tilde, TOK_Hat,
        TOK_HatEquals, TOK_Less, TOK_LessEquals, TOK_LeftShift, TOK_LeftShiftEquals,
        TOK_Greater, TOK_GreaterEquals, TOK_RightShift, TOK_RightShiftEquals, TOK_Percent,
        TOK_PercentEquals,

        TOK_Identifier, TOK_Macro, TOK_Character, TOK_String, TOK_RawString, TOK_FormattedString,
        TOK_FormattedRawString, TOK_Integer, TOK_BigInteger, TOK_Decimal, TOK_BigDecimal,
        TOK_Binary, TOK_Octal, TOK_Hexadecimal, TOK_BigBinary, TOK_BigOctal, TOK_BigHexadecimal,

        TOK_Var, TOK_Const, TOK_Func, TOK_Lambda, TOK_True, TOK_False, TOK_Null, TOK_Entrypoint,
        TOK_Specifications,

        TOK_Break, TOK_Continue, TOK_Return,

        TOK_If, TOK_Else, TOK_Switch, TOK_Case, TOK_Default, TOK_For, TOK_Foreach, TOK_While, TOK_Do,
        TOK_Try, TOK_Catch, TOK_Finally, TOK_Throw,

        TOK_As, TOK_Typeof,

        TOK_Colon, TOK_SemiColon, TOK_Comma, TOK_Dot, TOK_Question, TOK_QuestionDot,
        TOK_LeftParenthesis, TOK_RightParenthesis, TOK_LeftBrace, TOK_RightBrace, 
        TOK_LeftBracket, TOK_RightBracket, TOK_TypeParameter, // @<
    };

    class Token {
    public:
        Token(TokenKind kind, Position *position);
        ~Token();

        const TokenKind GetKind() const;
        Position *GetPosition() const;

        void Show(apic_char end);

        static const apic_u8 GetBinaryPrecedence(TokenKind kind);
        static const apic_u8 GetUnaryPrecedence(TokenKind kind);
    private:
        TokenKind kind;
        Position *position;
    };
}
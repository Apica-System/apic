#include "utils/token.hpp"

namespace utils {
    Token::Token(TokenKind kind, Position *position) {
        this->kind = kind;
        this->position = position;
    }

    Token::~Token() {
        if (this->position) delete this->position;
    }

    const TokenKind Token::GetKind() const {
        return this->kind;
    }

    Position *Token::GetPosition() const {
        return this->position;
    }

    void Token::Show(apic_char end) {
        printf("Token(kind: %u, pos: ", this->kind);
        this->position->Show('\0');
        printf(")%c", end);
    }

    const apic_u8 Token::GetBinaryPrecedence(TokenKind kind) {
        switch (kind) {
            case TokenKind::TOK_At: 
                return 16;
            
            case TokenKind::TOK_Dot: case TokenKind::TOK_QuestionDot:
                return 15;
            
            case TokenKind::TOK_PlusPlus: case TokenKind::TOK_MinusMinus:
                return 14;
            
            case TokenKind::TOK_Star: case TokenKind::TOK_Slash: case TokenKind::TOK_Percent:
                return 12;
            
            case TokenKind::TOK_Plus: case TokenKind::TOK_Minus:
                return 11;

            case TokenKind::TOK_LeftShift: case TokenKind::TOK_RightShift:
                return 10;
            
            case TokenKind::TOK_As:
                return 9;
            
            case TokenKind::TOK_Less: case TokenKind::TOK_LessEquals: case TokenKind::TOK_Greater: case TokenKind::TOK_GreaterEquals:
                return 8;
            
            case TokenKind::TOK_EqualsEquals: case TokenKind::TOK_BangEquals:
                return 7;
            
            case TokenKind::TOK_Ampersand:
                return 6;
            
            case TokenKind::TOK_Hat:
                return 5;
            
            case TokenKind::TOK_Pipe:
                return 4;
            
            case TokenKind::TOK_AmpersandAmpersand:
                return 3;
            
            case TokenKind::TOK_PipePipe:
                return 2;
            
            case TokenKind::TOK_Equals: case TokenKind::TOK_PlusEquals: case TokenKind::TOK_MinusEquals: case TokenKind::TOK_AtEquals: case TokenKind::TOK_AmpersandEquals:
            case TokenKind::TOK_PipeEquals: case TokenKind::TOK_HatEquals: case TokenKind::TOK_LeftShiftEquals: case TokenKind::TOK_RightShiftEquals:
            case TokenKind::TOK_StarEquals: case TokenKind::TOK_SlashEquals: case TokenKind::TOK_PercentEquals:
                return 1;

            default: return 0;
        }
    }

    const apic_u8 Token::GetUnaryPrecedence(TokenKind kind) {
        switch (kind) {
            case TokenKind::TOK_At:
                return 17;
            
            case TokenKind::TOK_Ampersand: case TokenKind::TOK_Plus: case TokenKind::TOK_Minus: case TokenKind::TOK_Tilde: case TokenKind::TOK_Bang:
            case TokenKind::TOK_PlusPlus: case TokenKind::TOK_MinusMinus:
                return 13;

            default: return 0;
        }
    }
}
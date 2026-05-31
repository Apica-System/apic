#include "utils/token.hpp"
#include <iostream>

using namespace utils;

Token::Token(TokenKind kind, const Position &position)
    : position(position) {
    this->kind = kind;
}

uint8_t Token::getBinaryPrecedence(TokenKind kind) {
    switch (kind) {
        case TokenKind::At: return 16;
        case TokenKind::Dot: case TokenKind::QuestionDot: return 15;
        case TokenKind::PlusPlus: case TokenKind::MinusMinus: return 14;
        case TokenKind::Star: case TokenKind::Slash: case TokenKind::Percent: return 12;
        case TokenKind::Plus: case TokenKind::Minus: return 11;
        case TokenKind::LeftShift: case TokenKind::RightShift: return 10;
        case TokenKind::As: return 9;

        case TokenKind::Less: case TokenKind::LessEquals:
        case TokenKind::Greater: case TokenKind::GreaterEquals:
            return 8;
        
        case TokenKind::EqualsEquals: case TokenKind::BangEquals: return 7;
        case TokenKind::Ampersand: return 6;
        case TokenKind::Hat: return 5;
        case TokenKind::Pipe: return 4;
        case TokenKind::AmpersandAmpersand: return 3;
        case TokenKind::PipePipe: return 2;

        case TokenKind::Equals: case TokenKind::PlusEquals: case TokenKind::MinusEquals:
        case TokenKind::StarEquals: case TokenKind::SlashEquals: case TokenKind::PercentEquals:
        case TokenKind::AtEquals: case TokenKind::AmpersandEquals: case TokenKind::PipeEquals:
        case TokenKind::HatEquals: case TokenKind::LeftShiftEquals: case TokenKind::RightShiftEquals:
            return 1;

        default: return 0;
    }
}

uint8_t Token::getUnaryPrecedence(TokenKind kind) {
    switch (kind) {
        case TokenKind::At: return 17;

        case TokenKind::Ampersand: case TokenKind::Plus: 
        case TokenKind::Minus: case TokenKind::Tilde: 
        case TokenKind::Bang: case TokenKind::PlusPlus: 
        case TokenKind::MinusMinus:
            return 13;

        default: return 0;
    }
}

TokenKind Token::getKind() const {
    return this->kind;
}

Position Token::getPosition() const {
    return this->position;
}

void Token::show(char end) const {
    std::cout << "Token<kind: " << static_cast<uint16_t>(this->kind) << ", pos: ";
    this->position.show('\0');
    std::cout << '>' << end;
}
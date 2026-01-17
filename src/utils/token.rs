use crate::utils::position::Position;

#[repr(u8)]
#[derive(PartialEq, Debug, Clone, Copy)]
pub enum TokenKind {
    Bad, EndOfFile, NewLine,

    Plus, PlusPlus, PlusEquals, Minus, MinusMinus,
    MinusEquals, Star, StarEquals, Slash, SlashEquals,
    Equals, EqualsEquals, Bang, BangEquals, At,
    AtEquals, Ampersand, AmpersandAmpersand,
    AmpersandEquals, Pipe, PipePipe, PipeEquals,
    Tilde, Hat, HatEquals, Less, LessEquals,
    LeftShift, LeftShiftEquals, Greater, GreaterEquals,
    RightShift, RightShiftEquals, Percent, PercentEquals,

    Identifier, Macro, Character, String,
    RawString, FormattedString, FormattedRawString,
    Integer, BigInteger, Decimal, BigDecimal,
    Binary, Octal, Hexadecimal, BigBinary,
    BigOctal, BigHexadecimal,

    Var, Const, Func, Lambda, True, False,
    Null, Entrypoint, Specifications, Global,

    Break, Continue, Return,

    If, Else, Switch, Case, Default, For, Foreach,
    While, Do, Try, Catch, Finally, Throw,

    As, Typeof,

    Colon, DoubleColon, SemiColon, Comma, Dot, Question, QuestionDot,
    LeftParenthesis, RightParenthesis, LeftBrace,
    RightBrace, LeftBracket, RightBracket,
    TypeParameter, // @<
}

pub struct Token {
    kind: TokenKind,
    position: Position,
}

impl Token {
    pub fn init(kind: TokenKind, position: Position) -> Token {
        Token { kind, position }
    }
    
    pub fn init_from(token: &Token) -> Token {
        Token { kind: token.kind, position: token.position.clone() }
    }

    pub fn get_kind(&self) -> &TokenKind {
        &self.kind
    }

    pub fn get_position(&self) -> &Position {
        &self.position
    }

    pub fn show(&self, end: char) {
        print!("Token(kind: {:?}, pos: ", self.kind);
        self.position.show('\0');
        print!("){end}");
    }
}

pub fn get_binary_precedence(kind: &TokenKind) -> u8 {
    match kind {
        TokenKind::At => 16,
        TokenKind::Dot | TokenKind::QuestionDot => 15,
        TokenKind::PlusPlus | TokenKind::MinusMinus => 14,
        TokenKind::Star | TokenKind::Slash | TokenKind::Percent => 12,
        TokenKind::Plus | TokenKind::Minus => 11,
        TokenKind::LeftShift | TokenKind::RightShift => 10,
        TokenKind::As => 9,

        TokenKind::Less | TokenKind::LessEquals | TokenKind::Greater | TokenKind::GreaterEquals
            => 8,

        TokenKind::EqualsEquals | TokenKind::BangEquals => 7,
        TokenKind::Ampersand => 6,
        TokenKind::Hat => 5,
        TokenKind::Pipe => 4,
        TokenKind::AmpersandAmpersand => 3,
        TokenKind::PipePipe => 2,

        TokenKind::Equals | TokenKind::PlusEquals | TokenKind::MinusEquals | TokenKind::StarEquals
            | TokenKind::SlashEquals | TokenKind::PercentEquals | TokenKind::AtEquals
            | TokenKind::AmpersandEquals | TokenKind::PipeEquals | TokenKind::HatEquals
            | TokenKind::LeftShiftEquals | TokenKind::RightShiftEquals
            => 1,

        _ => 0,
    }
}

pub fn get_unary_precedence(kind: &TokenKind) -> u8 {
    match kind {
        TokenKind::At => 17,

        TokenKind::Ampersand | TokenKind::Plus | TokenKind::Minus | TokenKind::Tilde
            | TokenKind::Bang | TokenKind::PlusPlus | TokenKind::MinusMinus
            => 13,

        _ => 0,
    }
}
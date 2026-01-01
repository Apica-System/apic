use crate::utils::builtins::APICA_KEYWORDS;
use crate::utils::diagnostic::{Diagnostic, DiagnosticKind};
use crate::utils::diagnostic_bag::DiagnosticBag;
use crate::utils::position::Position;
use crate::utils::source::SourceText;
use crate::utils::token::{Token, TokenKind};

#[derive(PartialEq)]
enum LexerNumberKind {
    Integer,
    Binary,
    Octal,
    Hexadecimal,
    Decimal,

    BigInteger,
    BigBinary,
    BigOctal,
    BigHexadecimal,
    BigDecimal,
}

pub struct Lexer<'a> {
    source: &'a SourceText,
    diag_bag: &'a mut DiagnosticBag,
    current_index: usize,
    current_char: char,
}

impl<'a> Lexer<'a> {
    pub fn init(source: &'a SourceText, diag_bag: &'a mut DiagnosticBag) -> Lexer<'a> {
        Lexer { source, diag_bag, current_index: 0, current_char: source.get_char_at(0) }
    }

    pub fn lex(&mut self) -> Token {
        self.skip_spaces();
        if self.current_char == '\0' {
            return Token::init(TokenKind::EndOfFile, Position::init(self.current_index, 1));
        }

        let next = self.source.get_char_at(self.current_index + 1);
        if self.current_char == '/' {
            if next == '/' || next == '*' {
                self.skip_comments(next == '*');
                return self.lex();
            }

            if next == '=' {
                return self.advance_with(TokenKind::SlashEquals, 2);
            }
            return self.advance_with(TokenKind::Slash, 1);
        }

        if let Some(token) = self.lex_basic_token(next) {
            return token;
        }

        if self.current_char.is_ascii_digit() {
            return self.lex_number();
        } else if self.current_char.is_alphanumeric() {
            return self.lex_word();
        }

        let err_position = Position::init(self.current_index, 1);
        self.advance();
        self.diag_bag.add(Diagnostic::init_complete(
            DiagnosticKind::Error,
            String::from("LexerError: Incorrect character found"),
            Position::init_from(&err_position),
        ));

        Token::init(TokenKind::EndOfFile, err_position)
    }

    fn lex_basic_token(&mut self, next: char) -> Option<Token> {
        match self.current_char {
            '\n' => Some(self.advance_with(TokenKind::NewLine, 1)),
            '\'' => Some(self.lex_character()),
            '"' => Some(self.lex_string()),
            '`' => Some(self.lex_raw_string()),
            '$' => Some(self.lex_formatted_string()),
            '#' => Some(self.lex_macro()),

            '+' => if next == '+' {
                Some(self.advance_with(TokenKind::PlusPlus, 2))
            } else if next == '=' {
                Some(self.advance_with(TokenKind::PlusEquals, 2))
            } else {
                Some(self.advance_with(TokenKind::Plus, 1))
            },

            '-' => if next == '-' {
                Some(self.advance_with(TokenKind::MinusMinus, 2))
            } else if next == '=' {
                Some(self.advance_with(TokenKind::MinusEquals, 2))
            } else {
                Some(self.advance_with(TokenKind::Minus, 1))
            },

            '*' => if next == '=' {
                Some(self.advance_with(TokenKind::StarEquals, 2))
            } else {
                Some(self.advance_with(TokenKind::Star, 1))
            },

            '%' => if next == '=' {
                Some(self.advance_with(TokenKind::PercentEquals, 2))
            } else {
                Some(self.advance_with(TokenKind::Percent, 1))
            },

            '=' => if next == '=' {
                Some(self.advance_with(TokenKind::EqualsEquals, 2))
            } else {
                Some(self.advance_with(TokenKind::Equals, 1))
            },

            '!' => if next == '=' {
                Some(self.advance_with(TokenKind::BangEquals, 2))
            } else {
                Some(self.advance_with(TokenKind::Bang, 1))
            },

            '^' => if next == '=' {
                Some(self.advance_with(TokenKind::HatEquals, 2))
            } else {
                Some(self.advance_with(TokenKind::Hat, 1))
            },

            '@' => if next == '=' {
                Some(self.advance_with(TokenKind::AtEquals, 2))
            } else if next == '<' {
                Some(self.advance_with(TokenKind::TypeParameter, 2))
            } else {
                Some(self.advance_with(TokenKind::At, 1))
            },

            '&' => if next == '&' {
                Some(self.advance_with(TokenKind::AmpersandAmpersand, 2))
            } else if next == '=' {
                Some(self.advance_with(TokenKind::AmpersandEquals, 2))
            } else {
                Some(self.advance_with(TokenKind::Ampersand, 1))
            },

            '|' => if next == '|' {
                Some(self.advance_with(TokenKind::PipePipe, 2))
            } else if next == '=' {
                Some(self.advance_with(TokenKind::PipeEquals, 2))
            } else {
                Some(self.advance_with(TokenKind::Pipe, 1))
            },

            '<' => if next == '<' {
                let third_char = self.source.get_char_at(self.current_index + 2);
                if third_char == '=' {
                    Some(self.advance_with(TokenKind::LeftShiftEquals, 3))
                } else { 
                    Some(self.advance_with(TokenKind::LeftShift, 2))
                }
            } else if next == '=' {
                Some(self.advance_with(TokenKind::LessEquals, 2))
            } else {
                Some(self.advance_with(TokenKind::Less, 1))
            },

            '>' => if next == '>' {
                let third_char = self.source.get_char_at(self.current_index + 2);
                if third_char == '=' {
                    Some(self.advance_with(TokenKind::RightShiftEquals, 3))
                } else {
                    Some(self.advance_with(TokenKind::RightShift, 2))
                }
            } else if next == '=' {
                Some(self.advance_with(TokenKind::GreaterEquals, 2))
            } else {
                Some(self.advance_with(TokenKind::Greater, 1))
            },

            '?' => if next == '.' {
                Some(self.advance_with(TokenKind::QuestionDot, 2))
            } else {
                Some(self.advance_with(TokenKind::Question, 1))
            },

            ':' => if next == ':' {
                Some(self.advance_with(TokenKind::DoubleColon, 2))
            } else {
                Some(self.advance_with(TokenKind::Colon, 1))
            },

            '(' => Some(self.advance_with(TokenKind::LeftParenthesis, 1)),
            ')' => Some(self.advance_with(TokenKind::RightParenthesis, 1)),
            '[' => Some(self.advance_with(TokenKind::LeftBracket, 1)),
            ']' => Some(self.advance_with(TokenKind::RightBracket, 1)),
            '{' => Some(self.advance_with(TokenKind::LeftBrace, 1)),
            '}' => Some(self.advance_with(TokenKind::RightBrace, 1)),
            ';' => Some(self.advance_with(TokenKind::SemiColon, 1)),
            ',' => Some(self.advance_with(TokenKind::Comma, 1)),
            '.' => Some(self.advance_with(TokenKind::Dot, 1)),
            '~' => Some(self.advance_with(TokenKind::Tilde, 1)),

            _ => None,
        }
    }

    fn lex_word(&mut self) -> Token {
        let start = self.current_index;
        let mut word = String::new();

        while self.current_char.is_alphanumeric() || self.current_char == '_' {
            word.push(self.current_char);
            self.advance();
        }

        let mut kind = TokenKind::Identifier;
        if let Some(k) = APICA_KEYWORDS.get(word.as_str()) {
            kind = *k;
        }

        Token::init(kind, Position::init(start, self.current_index - start))
    }

    fn lex_character(&mut self) -> Token {
        let start = self.current_index;
        self.advance();
        self.skip_specials();

        if self.current_char == '\\' {
            self.skip_special_character();
        } else if self.current_char == '\'' {
            self.advance();
            return self.incorrect_token(start, String::from("LexerError: Empty character literal declaration are not allowed"));
        } else {
            self.advance();
        }

        if self.current_char != '\'' {
            while self.current_char != '\'' && self.current_char != '\0' {
                self.advance();
            }

            self.advance();
            return self.incorrect_token(start, String::from("LexerError: Unterminated character literal declaration"));
        }

        self.advance();
        Token::init(
            TokenKind::Character, 
            Position::init(start + 1, self.current_index - (start + 2))
        )
    }

    fn lex_string(&mut self) -> Token {
        let start = self.current_index;
        self.advance();

        while self.current_char != '"' && self.current_char != '\0' {
            if self.current_char == '\\' {
                self.skip_special_character();
            } else {
                self.advance();
            }
        }

        if self.current_char != '"' {
            return self.incorrect_token(start, String::from("LexerError: Unterminated string literal declaration"));
        }

        self.advance();
        Token::init(
            TokenKind::String, 
            Position::init(start + 1, self.current_index - (start + 2))
        )
    }

    fn lex_raw_string(&mut self) -> Token {
        let start = self.current_index;
        self.advance();

        while self.current_char != '`' && self.current_char != '\0' {
            self.advance();
        }

        if self.current_char != '`' {
            return self.incorrect_token(start, String::from("LexerError: Unterminated raw-string literal declaration"));
        }

        self.advance();
        Token::init(
            TokenKind::RawString, 
            Position::init(start + 1, self.current_index - (start + 2))
        )
    }

    fn lex_formatted_string(&mut self) -> Token {
        let start = self.current_index;
        self.advance();

        let token = self.lex();
        if *token.get_kind() == TokenKind::Bad {
            return token;
        }

        if *token.get_kind() == TokenKind::String {
            return Token::init(TokenKind::FormattedString, Position::init_from(token.get_position()));
        } else if *token.get_kind() == TokenKind::RawString {
            return Token::init(TokenKind::FormattedRawString, Position::init_from(token.get_position()));
        }

        self.incorrect_token(start, String::from("LexerError: `$` should be followed by a (raw-)string literal to be formatted"))
    }

    fn lex_macro(&mut self) -> Token {
        let start = self.current_index;
        self.advance();

        if !self.current_char.is_alphanumeric() {
            return self.incorrect_token(start, String::from("LexerError: macro definitions need a valid identifier after `#`"));
        }

        while self.current_char.is_alphanumeric() {
            self.advance();
        }

        Token::init(TokenKind::Macro, Position::init(start + 1, self.current_index - (start + 1)))
    }

    fn lex_number(&mut self) -> Token {
        let start = self.current_index;
        let mut dots = false;
        let mut exps = false;
        let mut awaiting = false;
        let mut number_mode = LexerNumberKind::Integer;

        let first_char = self.current_char;
        self.advance();
        if first_char == '0' {
            if self.current_char == 'b' {
                number_mode = LexerNumberKind::Binary;
            } else if self.current_char == 'o' {
                number_mode = LexerNumberKind::Octal;
            } else if self.current_char == 'x' {
                number_mode = LexerNumberKind::Hexadecimal;
            }

            if number_mode != LexerNumberKind::Integer {
                self.advance();
            }
        }

        while self.current_char.is_digit(16) || self.current_char == '_' || self.current_char == '.' {
            if self.current_char == '_' {
                // Do nothing
            } else if number_mode != LexerNumberKind::Hexadecimal && (self.current_char == 'e' || self.current_char == 'E') {
                if number_mode != LexerNumberKind::Integer && number_mode != LexerNumberKind::Decimal {
                    return self.incorrect_number_token(start, String::from("LexerError: Hexadecimal, octal and binary numbers cannot be declared as exponential numbers"));
                } else if exps {
                    return self.incorrect_number_token(start, String::from("LexerError: Exponential numbers cannot contain more than one `e` (exp)"));
                } else if awaiting {
                    return self.incorrect_number_token(start, String::from("LexerError: Cannot add `e` (exp) right after `.` without any digit in between"));
                }

                number_mode = LexerNumberKind::Decimal;
                exps = true;
                let sign = self.source.get_char_at(self.current_index + 1);
                if sign != '+' && sign != '-' {
                    return self.incorrect_number_token(start, String::from("LexerError: Exponential numbers must have `+` or `-` sign after `e` (exp)"));
                }

                self.advance();
                self.advance();
                awaiting = true;
                continue;
            } else if self.current_char == '.' {
                if number_mode != LexerNumberKind::Integer {
                    return if number_mode != LexerNumberKind::Decimal {
                        self.incorrect_number_token(start, String::from("LexerError: Hexadecimal, octal and binary numbers cannot be declared as decimal numbers"))
                    } else {
                        self.incorrect_number_token(start, String::from("LexerError: Cannot add `.` after using `e` (exp) in a number declaration"))
                    }
                } else if dots {
                    return self.incorrect_number_token(start, String::from("LexerError: Decimal numbers cannot contain more than one `.`"));
                }

                number_mode = LexerNumberKind::Decimal;
                dots = true;
                awaiting = true;
            } else if number_mode == LexerNumberKind::Binary && !self.current_char.is_digit(2) {
                return self.incorrect_number_token(start, String::from("LexerError: Binary numbers can only contain digits from 0 to 1 (0b)"));
            } else if number_mode == LexerNumberKind::Octal && !self.current_char.is_digit(8) {
                return self.incorrect_number_token(start, String::from("LexerError: Octal numbers can only contain digits from 0 to 7 (0o)"));
            } else if number_mode != LexerNumberKind::Hexadecimal && self.current_char >= 'A' {
                return self.incorrect_number_token(start, String::from("LexerError: Letters A to F can only be used in hexadecimal numbers (0x)"));
            }

            if awaiting {
                awaiting = self.current_char == '_';
            }
            self.advance();
        }

        if awaiting {
            return self.incorrect_token(start, String::from("LexerError: A `.` or a `e` (exp) needs at least one digit after it in a number declaration"));
        }

        if self.current_char == 'i' {
            self.advance();
            if number_mode == LexerNumberKind::Decimal {
                return self.incorrect_token(start, String::from("LexerError: Decimal numbers cannot have modifier `i` (u64)"));
            }

            match number_mode {
                LexerNumberKind::Binary => number_mode = LexerNumberKind::BigBinary,
                LexerNumberKind::Octal => number_mode = LexerNumberKind::BigOctal,
                LexerNumberKind::Hexadecimal => number_mode = LexerNumberKind::BigHexadecimal,
                _ => number_mode = LexerNumberKind::BigInteger,
            }
        } else if self.current_char == 'l' {
            self.advance();
            if number_mode != LexerNumberKind::Integer && number_mode != LexerNumberKind::Decimal {
                return self.incorrect_token(start, String::from("LexerError: Hexadecimal, octal and binary numbers cannot have modifier `l` (f64)"));
            }

            number_mode = LexerNumberKind::BigDecimal;
        }

        match number_mode {
            LexerNumberKind::Integer => Token::init(TokenKind::Integer, Position::init(start, self.current_index - start)),
            LexerNumberKind::Binary => Token::init(TokenKind::Binary, Position::init(start + 2, self.current_index - (start + 2))),
            LexerNumberKind::Octal => Token::init(TokenKind::Octal, Position::init(start + 2, self.current_index - (start + 2))),
            LexerNumberKind::Hexadecimal => Token::init(TokenKind::Hexadecimal, Position::init(start + 2, self.current_index - (start + 2))),
            LexerNumberKind::Decimal => Token::init(TokenKind::Decimal, Position::init(start, self.current_index - start)),

            LexerNumberKind::BigInteger => Token::init(TokenKind::BigInteger, Position::init(start, self.current_index - (start + 1))),
            LexerNumberKind::BigBinary => Token::init(TokenKind::BigBinary, Position::init(start + 2, self.current_index - (start + 3))),
            LexerNumberKind::BigOctal => Token::init(TokenKind::BigOctal, Position::init(start + 2, self.current_index - (start + 3))),
            LexerNumberKind::BigHexadecimal => Token::init(TokenKind::BigHexadecimal, Position::init(start + 2, self.current_index - (start + 3))),
            LexerNumberKind::BigDecimal => Token::init(TokenKind::BigDecimal, Position::init(start, self.current_index - (start + 1))),
        }
    }

    fn skip_spaces(&mut self) {
        while self.current_char.is_whitespace() && self.current_char != '\n' {
            self.advance();
        }
    }

    fn skip_specials(&mut self) {
        while self.current_char.is_whitespace() && self.current_char != ' ' {
            self.advance();
        }
    }

    fn skip_comments(&mut self, is_multiline: bool) {
        self.advance();
        self.advance();
        if is_multiline {
            while self.current_char != '\0' {
                if self.current_char == '*' {
                    let next = self.source.get_char_at(self.current_index + 1);
                    if next == '/' {
                        self.advance();
                        self.advance();
                        return;
                    }
                }

                self.advance();
            }
        } else {
            while self.current_char != '\n' && self.current_char != '\0' {
                self.advance();
            }
        }
    }

    fn skip_special_character(&mut self) {
        self.advance();
        let old_char = self.current_char;
        self.advance();

        match old_char {
            'B' => {
                if !self.current_char.is_digit(2) {
                    self.diag_bag.add(Diagnostic::init_complete(
                        DiagnosticKind::Error,
                        String::from("LexerError: `\\B` needs a binary representation of a character after it"),
                        Position::init(self.current_index - 2, 2)
                    ));
                    return;
                }

                self.advance();
                for _ in 0..31 {
                    if !self.current_char.is_digit(2) { break; }
                    self.advance();
                }
            },

            'o' => {
                if !self.current_char.is_digit(8) {
                    self.diag_bag.add(Diagnostic::init_complete(
                        DiagnosticKind::Error,
                        String::from("LexerError: `\\o` needs an octal representation of a character after it"),
                        Position::init(self.current_index - 2, 2)
                    ));
                    return;
                }

                let limit: u8 = if self.current_char <= '3' { 10 } else { 9 };
                self.advance();
                for _ in 0..limit {
                    if !self.current_char.is_digit(8) { break; }
                    self.advance();
                }
            },

            'u' | 'x' => {
                if !self.current_char.is_digit(16) {
                    self.diag_bag.add(Diagnostic::init_complete(
                        DiagnosticKind::Error,
                        String::from("LexerError: `\\u` and `\\x` need a hexadecimal representation of a character after it"),
                        Position::init(self.current_index - 2, 2)
                    ));
                    return;
                }

                self.advance();
                for _ in 0..7 {
                    if !self.current_char.is_digit(16) { break; }
                    self.advance();
                }
            },

            _ => {},
        }
    }

    fn advance(&mut self) {
        self.current_index += 1;
        self.current_char = self.source.get_char_at(self.current_index);
    }

    fn advance_with(&mut self, kind: TokenKind, size: usize) -> Token {
        let start = self.current_index;
        for _ in 0..size {
            self.advance();
        }

        Token::init(kind, Position::init(start, size))
    }

    fn incorrect_token(&mut self, start: usize, message: String) -> Token {
        let err_position = Position::init(start, self.current_index - start);
        self.diag_bag.add(Diagnostic::init_complete(
            DiagnosticKind::Error,
            message,
            Position::init_from(&err_position),
        ));

        Token::init(TokenKind::Bad, err_position)
    }

    fn incorrect_number_token(&mut self, start: usize, message: String) -> Token {
        while self.current_char.is_digit(16) || self.current_char == '_' || self.current_char == '.' {
            if self.current_char == 'e' || self.current_char == 'E' {
                let next = self.source.get_char_at(self.current_index + 1);
                if next == '+' || next == '-' {
                    self.advance();
                }
            }

            self.advance();
        }

        if self.current_char == 'i' || self.current_char == 'l' {
            self.advance();
        }

        let err_position = Position::init(start, self.current_index - start);
        self.diag_bag.add(Diagnostic::init_complete(
            DiagnosticKind::Error,
            message,
            Position::init_from(&err_position),
        ));

        Token::init(TokenKind::Bad, err_position)
    }
}


#[cfg(test)]
mod tests {
    use crate::core::lexer::Lexer;
    use crate::utils::diagnostic_bag::DiagnosticBag;
    use crate::utils::source::SourceText;
    use crate::utils::token::TokenKind;

    #[test]
    fn empty_lexer() {
        let source = SourceText::init(String::new());
        let mut diag_bag = DiagnosticBag::init();

        let mut lexer = Lexer::init(&source, &mut diag_bag);
        let token = lexer.lex();

        assert_eq!(TokenKind::EndOfFile, *token.get_kind());
        assert_eq!(false, diag_bag.has_any());
    }
}
#include "core/lexer.hpp"

namespace core {
    std::unordered_map<std::string, utils::TokenKind> _keywords;
    void init_keywords() {
        if (!_keywords.empty())
            return;
        
        _keywords["var"] = utils::TokenKind::TOK_Var;
        _keywords["const"] = utils::TokenKind::TOK_Const;
        _keywords["func"] = utils::TokenKind::TOK_Func;
        _keywords["lambda"] = utils::TokenKind::TOK_Lambda;

        _keywords["true"] = utils::TokenKind::TOK_True;
        _keywords["false"] = utils::TokenKind::TOK_False;
        _keywords["null"] = utils::TokenKind::TOK_Null;

        _keywords["entry"] = utils::TokenKind::TOK_Entrypoint;
        _keywords["specs"] = utils::TokenKind::TOK_Specifications;
    }

    Lexer::Lexer(utils::SourceText *source, utils::DiagnosticBag *diag_bag) {
        init_keywords();
        
        this->source = source;
        this->diag_bag = diag_bag;
        this->ascii_index = 0;
        this->utf8_index = 0;
        this->current_char = this->source->GetCharAt(0);
        this->utf8_awaited = chars_length_utf8(this->current_char);
    }

    Lexer::~Lexer() {

    }

    utils::Token *Lexer::Lex() {
        this->SkipSpaces();
        if (this->current_char == '\0') {
            return new utils::Token(
                utils::TokenKind::TOK_EndOfFile,
                new utils::Position(this->ascii_index, 1, this->utf8_index, 1)
            );
        }

        apic_char next = this->source->GetCharAt(this->ascii_index + 1);
        if (next == '/') {
            if (next == '*' || next == '/') {
                this->SkipComments(next == '*');
                return this->Lex();
            }

            if (next == '=')
                return this->AdvanceWith(utils::TokenKind::TOK_SlashEquals, 2);
            return this->AdvanceWith(utils::TokenKind::TOK_Slash, 1);
        }

        utils::Token *basic_token = this->LexBasicToken(next);
        if (basic_token)
            return basic_token;
        
        if (this->current_char >= '0' && this->current_char <= '9')
            return this->LexNumber();
        else if (char_is_id_part(this->current_char))
            return this->LexWord();
        
        utils::Position *error_pos = new utils::Position(this->ascii_index, 1, this->utf8_index, 1);
        this->Advance();

        this->diag_bag->Add(new utils::Diagnostic(
            utils::DiagnosticKind::DIAG_Error,
            "LexerError: Incorrect character found",
            new utils::Position(error_pos)
        ));
        return new utils::Token(utils::TokenKind::TOK_Bad, error_pos);
    }

    void Lexer::SkipSpaces() {
        while (this->current_char != '\n' && ((this->current_char >= '\t' && this->current_char <= '\r') || this->current_char == ' '))
            this->Advance();
    }

    void Lexer::SkipSpecials() {
        while (this->current_char != ' ' && (this->current_char >= '\t' && this->current_char <= '\r'))
            this->Advance();
    }
    
    void Lexer::SkipComments(apic_bool is_multi) {
        this->Advance();
        this->Advance();
        if (is_multi) {
            while (this->current_char != '\0') {
                if (this->current_char == '*') {
                    apic_char next = this->source->GetCharAt(this->ascii_index + 1);
                    if (next == '/') {
                        this->Advance();
                        this->Advance();
                        return;
                    }
                }

                this->Advance();
            }
        } else {
            while (this->current_char != '\n' && this->current_char != '\0')
                this->Advance();
            
            this->Advance();
        }
    }

    void Lexer::SkipSpecialCharacter() {
        this->Advance();
        apic_char old_char = this->current_char;
        this->Advance();

        switch (old_char) {
            case 'B': {
                if (!char_is_binary(this->current_char)) {
                    this->diag_bag->Add(new utils::Diagnostic(
                        utils::DiagnosticKind::DIAG_Error,
                        "LexerError: `\\B` needs a binary representation of a character after it",
                        new utils::Position(this->ascii_index - 2, 2, this->utf8_index - 2, 2)
                    ));
                    return;
                }

                this->Advance();
                for (apic_u8 i = 0; i < 31; i++) {
                    if (!char_is_binary(this->current_char)) 
                        break;
                    
                    this->Advance();
                }
            }

            case 'o': {
                if (!char_is_octal(this->current_char)) {
                    this->diag_bag->Add(new utils::Diagnostic(
                        utils::DiagnosticKind::DIAG_Error,
                        "LexerError: `\\o` needs an octal representation of a character after it",
                        new utils::Position(this->ascii_index - 2, 2, this->utf8_index - 2, 2)
                    ));
                    return;
                }

                apic_u8 limit = this->current_char <= '3' ? 10 : 9;
                this->Advance();
                for (apic_u8 i = 0; i < limit; i++) {
                    if (!char_is_octal(this->current_char))
                        break;
                    
                    this->Advance();
                }
            }

            case 'u': case 'x': {
                if (!char_is_hexa(this->current_char)) {
                    this->diag_bag->Add(new utils::Diagnostic(
                        utils::DiagnosticKind::DIAG_Error,
                        "LexerError: `\\u` and `\\x` need a hexadecimal representation of a character after it",
                        new utils::Position(this->ascii_index - 2, 2, this->utf8_index - 2, 2)
                    ));
                    return;
                }

                this->Advance();
                for (apic_u8 i = 0; i < 7; i++) {
                    if (!char_is_hexa(this->current_char))
                        break;
                    
                    this->Advance();
                }
            }

            default: break;
        }
    }

    void Lexer::Advance() {
        if (this->current_char != '\0') {
            this->ascii_index++;
            this->current_char = this->source->GetCharAt(this->ascii_index);
            this->utf8_awaited--;
            if (!this->utf8_awaited) {
                this->utf8_index++;
                this->utf8_awaited = chars_length_utf8(this->current_char);
            }
        }
    }

    utils::Token *Lexer::AdvanceWith(utils::TokenKind kind, apic_u64 size) {
        apic_u64 ascii_start = this->ascii_index;
        apic_u64 utf8_start = this->utf8_index;
        for (apic_u64 i = 0; i < size; i++)
            this->Advance();
        
        return new utils::Token(
            kind,
            new utils::Position(ascii_start, this->ascii_index - ascii_start, utf8_start, this->utf8_index - utf8_start)
        );
    }

    utils::Token *Lexer::LexBasicToken(apic_char next) {
        switch (this->current_char) {
            case '\n': return this->AdvanceWith(utils::TokenKind::TOK_NewLine, 1);
            case '#': return this->LexMacro();
            case '\'': return this->LexCharacter();
            case '"': return this->LexString();
            case '`': return this->LexRawString();
            case '$': return this->LexFormattedString();

            case '+': {
                if (next == '+')
                    return this->AdvanceWith(utils::TokenKind::TOK_PlusPlus, 2);
                else if (next == '=')
                    return this->AdvanceWith(utils::TokenKind::TOK_PlusEquals, 2);
                    
                return this->AdvanceWith(utils::TokenKind::TOK_Plus, 1);
            }

            case '-': {
                if (next == '-')
                    return this->AdvanceWith(utils::TokenKind::TOK_MinusMinus, 2);
                else if (next == '=')
                    return this->AdvanceWith(utils::TokenKind::TOK_MinusEquals, 2);
                    
                return this->AdvanceWith(utils::TokenKind::TOK_Minus, 1);
            }

            case '*': {
                if (next == '=')
                    return this->AdvanceWith(utils::TokenKind::TOK_StarEquals, 2);
                    
                return this->AdvanceWith(utils::TokenKind::TOK_Star, 1);
            }

            case '%': {
                if (next == '=')
                    return this->AdvanceWith(utils::TokenKind::TOK_PercentEquals, 2);
                
                return this->AdvanceWith(utils::TokenKind::TOK_Percent, 1);
            }

            case '=': {
                if (next == '=')
                    return this->AdvanceWith(utils::TokenKind::TOK_EqualsEquals, 2);
                
                return this->AdvanceWith(utils::TokenKind::TOK_Equals, 1);
            }

            case '!': {
                if (next == '=')
                    return this->AdvanceWith(utils::TokenKind::TOK_BangEquals, 2);
                
                return this->AdvanceWith(utils::TokenKind::TOK_Bang, 1);
            }

            case '^': {
                if (next == '=')
                    return this->AdvanceWith(utils::TokenKind::TOK_HatEquals, 2);

                return this->AdvanceWith(utils::TokenKind::TOK_Hat, 1);
            }

            case '@': {
                if (next == '=')
                    return this->AdvanceWith(utils::TokenKind::TOK_AtEquals, 2);
                else if (next == '<')
                    return this->AdvanceWith(utils::TokenKind::TOK_TypeParameter, 2);
                
                return this->AdvanceWith(utils::TokenKind::TOK_At, 1);
            }

            case '&': {
                if (next == '&')
                    return this->AdvanceWith(utils::TokenKind::TOK_AmpersandAmpersand, 2);
                else if (next == '=')
                    return this->AdvanceWith(utils::TokenKind::TOK_AmpersandEquals, 2);
                    
                return this->AdvanceWith(utils::TokenKind::TOK_Ampersand, 1);
            }

            case '|': {
                if (next == '|')
                    return this->AdvanceWith(utils::TokenKind::TOK_PipePipe, 2);
                else if (next == '=')
                    return this->AdvanceWith(utils::TokenKind::TOK_PipeEquals, 2);
                    
                return this->AdvanceWith(utils::TokenKind::TOK_Pipe, 1);
            }

            case '<': {
                if (next == '<') {
                    apic_char third = this->source->GetCharAt(this->ascii_index + 2);
                    if (third == '=')
                        return this->AdvanceWith(utils::TokenKind::TOK_LeftShiftEquals, 3);
                    
                    return this->AdvanceWith(utils::TokenKind::TOK_LeftShift, 2);
                } else if (next == '=')
                    return this->AdvanceWith(utils::TokenKind::TOK_LessEquals, 2);
                    
                return this->AdvanceWith(utils::TokenKind::TOK_Less, 1);
            }

            case '>': {
                if (next == '>') {
                    apic_char third = this->source->GetCharAt(this->ascii_index + 2);
                    if (third == '=')
                        return this->AdvanceWith(utils::TokenKind::TOK_RightShiftEquals, 3);
                    
                    return this->AdvanceWith(utils::TokenKind::TOK_RightShift, 2);
                } else if (next == '=')
                    return this->AdvanceWith(utils::TokenKind::TOK_GreaterEquals, 2);
                    
                return this->AdvanceWith(utils::TokenKind::TOK_Greater, 1);
            }

            case '?': {
                if (next == '.')
                    return this->AdvanceWith(utils::TokenKind::TOK_QuestionDot, 2);
                
                return this->AdvanceWith(utils::TokenKind::TOK_Question, 1);
            }

            case '(': return this->AdvanceWith(utils::TokenKind::TOK_LeftParenthesis, 1);
            case ')': return this->AdvanceWith(utils::TokenKind::TOK_RightParenthesis, 1);
            case '[': return this->AdvanceWith(utils::TokenKind::TOK_LeftBracket, 1);
            case ']': return this->AdvanceWith(utils::TokenKind::TOK_RightBracket, 1);
            case '{': return this->AdvanceWith(utils::TokenKind::TOK_LeftBrace, 1);
            case '}': return this->AdvanceWith(utils::TokenKind::TOK_RightBrace, 1);
            case ';': return this->AdvanceWith(utils::TokenKind::TOK_SemiColon, 1);
            case ':': return this->AdvanceWith(utils::TokenKind::TOK_Colon, 1);
            case ',': return this->AdvanceWith(utils::TokenKind::TOK_Comma, 1);
            case '.': return this->AdvanceWith(utils::TokenKind::TOK_Dot, 1);
            case '~': return this->AdvanceWith(utils::TokenKind::TOK_Tilde, 1);

            default: return nullptr;
        }
    }

    utils::Token *Lexer::LexWord() {
        apic_u64 old_ascii = this->ascii_index, old_utf8 = this->utf8_index;
        std::string word;
        apic_u8 id_count = char_is_id_part(this->current_char);
        while (id_count) {
            for (apic_u8 i = 0; i < id_count; i++) {
                word.push_back(this->current_char);
                this->Advance();
            }

            id_count = char_is_id_part(this->current_char);
        }

        utils::TokenKind kind = utils::TokenKind::TOK_Identifier;
        if (_keywords.count(word))
            kind = _keywords[word];
        
        return new utils::Token(
            kind,
            new utils::Position(old_ascii, this->ascii_index - old_ascii, old_utf8, this->utf8_index - old_utf8)
        );
    }

    utils::Token *Lexer::LexCharacter() {
        apic_u64 old_ascii = this->ascii_index, old_utf8 = this->utf8_index;
        this->Advance();
        this->SkipSpecials();

        if (this->current_char == '\\') {
            this->SkipSpecialCharacter();
        } else if (this->current_char == '\'') {
            this->Advance();
            utils::Position *error_pos = new utils::Position(old_ascii, this->ascii_index - old_ascii, old_utf8, this->utf8_index - old_utf8);
            this->diag_bag->Add(new utils::Diagnostic(
                utils::DiagnosticKind::DIAG_Error,
                "LexerError: Empty character literal declarations are not allowed",
                new utils::Position(error_pos)
            ));

            return new utils::Token(utils::TokenKind::TOK_Bad, error_pos);
        } else {
            apic_u8 utf8_length = chars_length_utf8(this->current_char);
            for (apic_u8 i = 0; i < utf8_length; i++)
                this->Advance();
        }

        if (this->current_char != '\'') {
            while (this->current_char != '\'' && this->current_char != '\0')
                this->Advance();
            this->Advance();

            utils::Position *error_pos = new utils::Position(old_ascii, this->ascii_index - old_ascii, old_utf8, this->utf8_index - old_utf8);
            this->diag_bag->Add(new utils::Diagnostic(
                utils::DiagnosticKind::DIAG_Error,
                "LexerError: Unterminated character literal declaration",
                new utils::Position(error_pos)
            ));

            return new utils::Token(utils::TokenKind::TOK_Bad, error_pos);
        }

        this->Advance();
        return new utils::Token(
            utils::TokenKind::TOK_Character,
            new utils::Position(old_ascii + 1, this->ascii_index - (old_ascii + 2), old_utf8 + 1, this->utf8_index - (old_utf8 + 2))
        );
    }

    utils::Token *Lexer::LexString() {
        apic_u64 old_ascii = this->ascii_index, old_utf8 = this->utf8_index;
        this->Advance();

        while (this->current_char != '"' && this->current_char != '\0') {
            if (this->current_char == '\\')
                this->SkipSpecialCharacter();
            else
                this->Advance();
        }

        if (this->current_char != '"') {
            utils::Position *error_pos = new utils::Position(old_ascii, this->ascii_index - old_ascii, old_utf8, this->utf8_index - old_utf8);
            this->diag_bag->Add(new utils::Diagnostic(
                utils::DiagnosticKind::DIAG_Error,
                "LexerError: Unterminated string literal declaration",
                new utils::Position(error_pos)
            ));

            return new utils::Token(utils::TokenKind::TOK_Bad, error_pos);
        }

        this->Advance();
        return new utils::Token(
            utils::TokenKind::TOK_String,
            new utils::Position(old_ascii + 1, this->ascii_index - (old_ascii + 2), old_utf8 + 1, this->utf8_index - (old_utf8 + 2))
        );
    }

    utils::Token *Lexer::LexRawString() {
        apic_u64 old_ascii = this->ascii_index, old_utf8 = this->utf8_index;
        this->Advance();

        while (this->current_char != '`' && this->current_char != '\0') {
            if (this->current_char == '\\')
                this->SkipSpecialCharacter();
            else
                this->Advance();
        }

        if (this->current_char != '`') {
            utils::Position *error_pos = new utils::Position(old_ascii, this->ascii_index - old_ascii, old_utf8, this->utf8_index - old_utf8);
            this->diag_bag->Add(new utils::Diagnostic(
                utils::DiagnosticKind::DIAG_Error,
                "LexerError: Unterminated raw-string literal declaration",
                new utils::Position(error_pos)
            ));

            return new utils::Token(utils::TokenKind::TOK_RawString, error_pos);
        }

        this->Advance();
        return new utils::Token(
            utils::TokenKind::TOK_RawString,
            new utils::Position(old_ascii + 1, this->ascii_index - (old_ascii + 2), old_utf8 + 1, this->utf8_index - (old_utf8 + 2))
        );
    }

    utils::Token *Lexer::LexFormattedString() {
        apic_u64 old_ascii = this->ascii_index, old_utf8 = this->utf8_index;
        this->Advance();

        utils::Token *string_token = this->Lex();
        if (string_token->GetKind() == utils::TokenKind::TOK_Bad)
            return string_token;
        
        if (string_token->GetKind() == utils::TokenKind::TOK_String) {
            utils::Position *tok_pos = new utils::Position(string_token->GetPosition());
            delete string_token;
            return new utils::Token(utils::TokenKind::TOK_FormattedString, tok_pos);
        } else if (string_token->GetKind() == utils::TokenKind::TOK_RawString) {
            utils::Position *tok_pos = new utils::Position(string_token->GetPosition());
            delete string_token;
            return new utils::Token(utils::TokenKind::TOK_FormattedRawString, tok_pos);
        }

        delete string_token;
        utils::Position *error_pos = new utils::Position(old_ascii, this->ascii_index - old_ascii, old_utf8, this->utf8_index - old_utf8);
        this->diag_bag->Add(new utils::Diagnostic(
            utils::DiagnosticKind::DIAG_Error,
            "LexerError: `$` should be followed by a (raw-)string literal to be formatted",
            new utils::Position(error_pos)
        ));

        return new utils::Token(utils::TokenKind::TOK_Bad, error_pos);
    }

    utils::Token *Lexer::LexMacro() {
        apic_u64 old_ascii = this->ascii_index, old_utf8 = this->utf8_index;
        this->Advance();

        apic_u8 id_count = char_is_id_part(this->current_char);
        if (!id_count) {
            utils::Position *error_pos = new utils::Position(old_ascii, this->ascii_index - old_ascii, old_utf8, this->utf8_index - old_utf8);
            this->diag_bag->Add(new utils::Diagnostic(
                utils::DiagnosticKind::DIAG_Error,
                "LexerError: macro definition need a valid identifier after `#`",
                new utils::Position(error_pos)
            ));

            return new utils::Token(utils::TokenKind::TOK_Bad, error_pos);
        }

        while (id_count) {
            for (apic_u8 i = 0; i < id_count; i++)
                this->Advance();
            id_count = char_is_id_part(this->current_char);
        }

        return new utils::Token(
            utils::TokenKind::TOK_Macro,
            new utils::Position(old_ascii + 1, this->ascii_index - (old_ascii + 1), old_utf8 + 1, this->utf8_index - (old_utf8 + 1))
        );
    }

    utils::Token *Lexer::LexNumber() {
        apic_u64 old_ascii = this->ascii_index, old_utf8 = this->utf8_index;
        apic_bool dots = false, exps = false, awaiting = false;
        apic_char mode = 'i';

        apic_char first_char = this->current_char;
        this->Advance();
        if (first_char == '0') {
            if (this->current_char == 'o')
                mode = 'o';
            else if (this->current_char == 'x')
                mode = 'x';
            else if (this->current_char == 'b')
                mode = 'b';
            
            if (mode != 'i')
                this->Advance();
        }

        while (char_is_hexa(this->current_char) || this->current_char == '_' || this->current_char == '.') {
            if (this->current_char == '_') {
                // DO nothing
            } else if (mode != 'x' && (this->current_char == 'e' || this->current_char == 'E')) {
                apic_u8 error_index = 0;
                if (mode != 'i' && mode != 'f')
                    error_index = 1;
                else if (exps)
                    error_index = 2;
                else if (awaiting)
                    error_index = 3;
                
                if (!error_index) {
                    mode = 'f';
                    exps = true;
                    apic_char sign = this->source->GetCharAt(this->ascii_index + 1);
                    if (sign != '-' && sign != '+')
                        error_index = 4;
                }

                switch (error_index) {
                    case 1: return this->LexIncorrectNumber(old_ascii, old_utf8, "LexerError: Hexadecimal, octal and binary numbers cannot be declared as exponential numbers");
                    case 2: return this->LexIncorrectNumber(old_ascii, old_utf8, "LexerError: Exponential numbers cannot have more than one `e` (exp)");
                    case 3: return this->LexIncorrectNumber(old_ascii, old_utf8, "LexerError: Cannot add `e` (exp) right after `.` without any digit in between");
                    case 4: return this->LexIncorrectNumber(old_ascii, old_utf8, "LexerError: Exponential numbers must have `+` or `-` sign after `e` (exp)");

                    default: break;
                }

                this->Advance();
                this->Advance();
                awaiting = true;
                continue;
            } else if (this->current_char == '.') {
                if (mode != 'i') {
                    if (mode != 'f')
                        return this->LexIncorrectNumber(old_ascii, old_utf8, "LexerError: Hexadecimal, octal and binary numbers cannot be declared as decimal numbers");
                    return this->LexIncorrectNumber(old_ascii, old_utf8, "LexerError: Cannot add `.` after using `e` (exp) in a number declaration");
                } else if (dots)
                    return this->LexIncorrectNumber(old_ascii, old_utf8, "LexerError: Decimal numbers cannot have more than one `.`");
                else if (awaiting)
                    return this->LexIncorrectNumber(old_ascii, old_utf8, "LexerError: Cannot add `.` right after `e` (exp) without any digit in between");
                
                mode = 'f';
                dots = true;
                awaiting = true;
            } else if (mode == 'b' && !char_is_binary(this->current_char))
                return this->LexIncorrectNumber(old_ascii, old_utf8, "LexerError: Binary numbers can only contain digits from 0 to 1");
            else if (mode == 'o' && !char_is_octal(this->current_char))
                return this->LexIncorrectNumber(old_ascii, old_utf8, "LexerError: Octal numbers can only contain digits from 0 to 7");
            else if (mode != 'x' && this->current_char >= 'A')
                return this->LexIncorrectNumber(old_ascii, old_utf8, "LexerError: Letters A to F can only be used in hexadecimal numbers");
        
            if (awaiting)
                awaiting = this->current_char == '_';
            
            this->Advance();
        }

        if (awaiting)
            return this->LexIncorrectNumberEnding(old_ascii, old_utf8, "LexerError: A `.` or a `e` (exp) needs at least one digit after it in a number declaration");
        
        if (this->current_char == 'i') {
            this->Advance();
            if (mode == 'f')
                return this->LexIncorrectNumberEnding(old_ascii, old_utf8, "LexerError: Decimal numbers cannot have modifier `i` (u64)");
            
            switch (mode) {
                case 'b': mode = 'B'; break;
                case 'o': mode = 'O'; break;
                case 'x': mode = 'X'; break;
                default: mode = 'I'; break;
            }
        } else if (this->current_char == 'l') {
            this->Advance();
            if (mode != 'f' && mode != 'i')
                return this->LexIncorrectNumberEnding(old_ascii, old_utf8, "LexerError: Hexadecimal, octal and binary numbers cannot have modifier `l` (f64)");
        
            mode = 'F';
        }

        switch (mode) {
            case 'b': return new utils::Token(utils::TokenKind::TOK_Binary, new utils::Position(old_ascii + 2, this->ascii_index - (old_ascii + 2), old_utf8 + 2, this->utf8_index - (old_utf8 + 2)));
            case 'o': return new utils::Token(utils::TokenKind::TOK_Octal, new utils::Position(old_ascii + 2, this->ascii_index - (old_ascii + 2), old_utf8, this->utf8_index - (old_utf8 + 2)));
            case 'x': return new utils::Token(utils::TokenKind::TOK_Hexadecimal, new utils::Position(old_ascii + 2, this->ascii_index - (old_ascii + 2), old_utf8, this->utf8_index - (old_utf8 + 2)));
            case 'f': return new utils::Token(utils::TokenKind::TOK_Decimal, new utils::Position(old_ascii, this->ascii_index - old_ascii, old_utf8, this->utf8_index - old_utf8));
            case 'I': return new utils::Token(utils::TokenKind::TOK_BigInteger, new utils::Position(old_ascii, this->ascii_index - (old_ascii + 1), old_utf8, this->utf8_index - (old_utf8 + 1)));
            case 'B': return new utils::Token(utils::TokenKind::TOK_BigBinary, new utils::Position(old_ascii + 2, this->ascii_index - (old_ascii + 3), old_utf8 + 2, this->utf8_index - (old_utf8 + 3)));
            case 'O': return new utils::Token(utils::TokenKind::TOK_BigOctal, new utils::Position(old_ascii + 2, this->ascii_index - (old_ascii + 3), old_utf8 + 2, this->utf8_index - (old_utf8 + 3)));
            case 'X': return new utils::Token(utils::TokenKind::TOK_BigHexadecimal, new utils::Position(old_ascii + 2, this->ascii_index - (old_ascii + 3), old_utf8 + 2, this->utf8_index - (old_utf8 + 3)));
            case 'F': return new utils::Token(utils::TokenKind::TOK_BigDecimal, new utils::Position(old_ascii, this->ascii_index - (old_ascii + 1), old_utf8, this->utf8_index - (old_utf8 + 1)));
            case 'i': default: return new utils::Token(utils::TokenKind::TOK_Integer, new utils::Position(old_ascii, this->ascii_index - old_ascii, old_utf8, this->utf8_index - old_utf8));
        }
    }

    utils::Token *Lexer::LexIncorrectNumber(apic_u64 ascii_start, apic_u64 utf8_start, const apic_string &message) {
        while (char_is_hexa(this->current_char) || this->current_char == '_' || this->current_char == '.') {
            if (this->current_char == 'e' || this->current_char == 'E') {
                apic_char next = this->source->GetCharAt(this->ascii_index + 1);
                if (next == '+' || next == '-')
                    this->Advance();
            }

            this->Advance();
        }

        if (this->current_char == 'i' || this->current_char == 'l')
            this->Advance();
        
        utils::Position *error_pos = new utils::Position(ascii_start, this->ascii_index - ascii_start, utf8_start, this->utf8_index - utf8_start);
        this->diag_bag->Add(new utils::Diagnostic(
            utils::DiagnosticKind::DIAG_Error,
            message,
            new utils::Position(error_pos)
        ));

        return new utils::Token(utils::TokenKind::TOK_Bad, error_pos);
    }

    utils::Token *Lexer::LexIncorrectNumberEnding(apic_u64 ascii_start, apic_u64 utf8_start, const apic_string &message) {
        utils::Position *error_pos = new utils::Position(ascii_start, this->ascii_index - ascii_start, utf8_start, this->utf8_index - utf8_start);
        this->diag_bag->Add(new utils::Diagnostic(
            utils::DiagnosticKind::DIAG_Error,
            message,
            new utils::Position(error_pos)
        ));

        return new utils::Token(utils::TokenKind::TOK_Bad, error_pos);
    }
}
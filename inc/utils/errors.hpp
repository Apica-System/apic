#pragma once

#include <string_view>

namespace utils {
    // Apic options' errors
    inline constexpr std::string_view OPT_ERROR_NO_ARGUMENT = "apic error : use `apic help` to see how to use the apica compiler";
    inline constexpr std::string_view OPT_ERROR_UNKNOWN_COMMAND = "apic error : incorrect command found. use `apic help` to see available commands";
    inline constexpr std::string_view OPT_ERROR_ALREADY_DEFINED = "apic error : this option was already defined `";
    inline constexpr std::string_view OPT_ERROR_VALUE_UNEXPECTED = "apic error : didn't expect a value without option before it. `";
    inline constexpr std::string_view OPT_ERROR_OPTION_EXPECTED_VALUE = "apic error : cannot use an option right after an option that awaited a value. `";
    inline constexpr std::string_view OPT_ERROR_INVALID_OPTION = "apic error : invalid option `";
    inline constexpr std::string_view OPT_ERROR_AWAITED_VALUE = "apic error: expected a value for previous option `";
    

    // Apic init errors
    inline constexpr std::string_view INT_ERROR_ASSETS_DIR = "apic error : failed to create assets folder";
    inline constexpr std::string_view INT_ERROR_SRC_DIR = "apic error : failed to create source code folder";
    inline constexpr std::string_view INT_ERROR_MAIN_FILE = "apic error : failed to create main apica source file";
    inline constexpr std::string_view INT_ERROR_WRITE_MAIN_FILE = "apic error : failed to write main apica source file content";


    // Apic build errors
    inline constexpr std::string_view BLD_ERROR_NO_SOURCE = "apic error : `build` command needs a specified main apica source file (i.e. -i)";
    inline constexpr std::string_view BLD_ERROR_CANT_READ_SOURCE = "apic error : failed to read or locate main apica source file `";


    // Lexer errors
    inline constexpr std::string_view LEX_ERROR_INCORRECT_CHARACTER_FOUND = "LexerError: Incorrect character found";
    inline constexpr std::string_view LEX_ERROR_UNTERMINATED_MULTILINE_COMMENT = "LexerError: Unterminated multiline comment";
    inline constexpr std::string_view LEX_ERROR_POINT_OR_EXP_AWAITING_IN_NUMBER = "LexerError: A `.` or a `e` (exp) needs at least one digit after it in a number declaration";
    inline constexpr std::string_view LEX_ERROR_HEXA_OCTAL_BINARY_CANT_BE_EXP = "LexerError: Hexadecimal, octal and binary numbers cannot be declared as exponential numbers";
    inline constexpr std::string_view LEX_ERROR_EXP_NUMBER_TOO_MANY_EXP = "LexerError: Exponential numbers cannot contain more than one `e` (exp)";
    inline constexpr std::string_view LEX_ERROR_EXP_NUMBER_POINT_RIGHT_BEFORE = "LexerError: Cannot add `e` (exp) right after `.` without any digit in between";
    inline constexpr std::string_view LEX_ERROR_EXP_NUMBER_NEED_PLUS_OR_MINUS = "LexerError: Exponential numbers must have `+` or `-` sign after `e` (exp)";
    inline constexpr std::string_view LEX_ERROR_HEXA_OCTAL_BINARY_CANT_BE_DECIMAL = "LexerError: Hexadecimal, octal and binary numbers cannot be declared as decimal numbers";
    inline constexpr std::string_view LEX_ERROR_POINT_AFTER_EXP_NUMBER = "LexerError: Cannot add `.` after using `e` (exp) to declare an exponential number";
    inline constexpr std::string_view LEX_ERROR_DECIMAL_NUMBER_TOO_MANY_POINTS = "LexerError: Decimal numbers cannot contain more than one `.`";
    inline constexpr std::string_view LEX_ERROR_BINARY_NUMBER_INCORRECT = "LexerError: Binary numbers can only contain digits 0 and 1 (0b)";
    inline constexpr std::string_view LEX_ERROR_OCTAL_NUMBER_INCORRECT = "LexerError: Octal numbers can only contains digits from 0 to 7 (0o)";
    inline constexpr std::string_view LEX_ERROR_ONLY_HEXA_CONTAIN_A_TO_F = "LexerError: Letters A to F can only be used in hexadecimal numbers (0x)";
    inline constexpr std::string_view LEX_ERROR_DECIMAL_NUMBER_CANT_HAVE_MOD_I = "LexerError: Decimal numbers cannot have modifier `i` (u64)";
    inline constexpr std::string_view LEX_ERROR_HEXA_OCTAL_BINARY_CANT_HAVE_MOD_L = "LexerError: Hexadecimal, octal and binary numbers cannot have modifier `l` (f64)";
    inline constexpr std::string_view LEX_ERROR_MACRO_INCORRECT = "LexerError: Macro definitions need a valid identifier after `#`";
    inline constexpr std::string_view LEX_ERROR_FORMATTED_STRING_INCORRECT = "LexerError: `$` must be followed by a string literal to be formatted";
    inline constexpr std::string_view LEX_ERROR_UNTERMINATED_RAW_STRING = "LexerError: Unterminated raw-string literal declaration";
    inline constexpr std::string_view LEX_ERROR_INCORRECT_BINARY_REPR = "LexerError: `\\B` needs a binary representation of a character after it";
    inline constexpr std::string_view LEX_ERROR_INCORRECT_OCTAL_REPR = "LexerError: `\\o` needs an octal representation of a character after it";
    inline constexpr std::string_view LEX_ERROR_INCORRECT_HEXA_REPR = "LexerError: `\\u` and `\\u` need a hexadecimal representation of a character after it";
    inline constexpr std::string_view LEX_ERROR_EMPTY_CHARACTER = "LexerError: Empty character literal declaration";
    inline constexpr std::string_view LEX_ERROR_UNTERMINATED_CHARACTER = "LexerError: Unterminated character literal declaration";
    inline constexpr std::string_view LEX_ERROR_UNTERMINATED_STRING = "LexerError: Unterminated string literal declaration";


    // Parser errors
    inline constexpr std::string_view PAR_ERROR_PARENTHESIS_AWAITED = "ParserError: Expected a `)` to end a parenthesized expression";
    inline constexpr std::string_view PAR_ERROR_BRACE_AWAITED = "ParserError: Expected a `}` to end a block of statements";
    inline constexpr std::string_view PAR_ERROR_INCORRECT_TOKEN = "ParserError: Incorrect token found `";
    inline constexpr std::string_view PAR_ERROR_PACKAGE_CALL = "ParserError: Package call must be followed by a var/const call or a function call";
    inline constexpr std::string_view PAR_ERROR_UNKNOWN_ENTRYPOINT = "ParserError: Unknown entrypoint identifier found (accepted are `init`, `update` and `quit`)";
    inline constexpr std::string_view PAR_ERROR_ENTRYPOINT_WITHOUT_COMPOUND = "ParserError: All entrypoint must be followed by a block of statements";
    inline constexpr std::string_view PAR_ERROR_ENTRYPOINT_OUT_OF_MAIN_SCOPE = "ParserError: Cannot declare an entrypoint out of the main global scope";
    inline constexpr std::string_view PAR_ERROR_COMMA_BETWEEN_PARAMS = "ParserError: Expected a `,` between parameters in a function call";
    inline constexpr std::string_view PAR_ERROR_UNTERMINATED_FUNC_CALL = "ParserError: Expected a `)` to end a function call";
    inline constexpr std::string_view PAR_ERROR_TERNARY_SEP_CND_TRUE_EXPR = "ParserError: Expected a `:` to separate true-result from the condition of a ternary expression";
    inline constexpr std::string_view PAR_ERROR_TERNARY_SEP_TRUE_FALSE_EXPR = "ParserError: Expected a `:` to separate false-result from true-result of a ternary expression";
    inline constexpr std::string_view PAR_ERROR_VAR_CONST_INCORRECT_TYPE = "ParserError: `const` and `var` need a valid type after `:`";
    inline constexpr std::string_view PAR_ERROR_TYPEOF_WITHOUT_LESS_CHAR = "ParserError: Expected a `<` to begin a typeof expression";
    inline constexpr std::string_view PAR_ERROR_TYPEOF_INCORRECT_TYPE = "ParserError: `typeof` need a valid type after `<`";
    inline constexpr std::string_view PAR_ERROR_UNTERMINATED_TYPEOF = "ParserError: Expected a `>` to end a typeof expression";
    inline constexpr std::string_view PAR_ERROR_VAR_CONST_WITHOUT_ID = "ParserError: `const` and `var` must be followed by an identifier to initialize a constant or a variable.";
    inline constexpr std::string_view PAR_ERROR_SPECS_WITHOUT_COMPOUND = "ParserError: `specs` must be followed by a block of specifications";
    inline constexpr std::string_view PAR_ERROR_UNTERMINATED_SPECS = "ParserError: `specs` block of specifications must be closed by a `}`";
    inline constexpr std::string_view PAR_ERROR_SPECS_OUT_OF_MAIN_SCOPE = "ParserError: Cannot declare specifications out of the main global scope";
    inline constexpr std::string_view PAR_ERROR_SPEC_WITHOUT_ID = "ParserError: A specification attribute must begin with an identifier";
    inline constexpr std::string_view PAR_ERROR_SPEC_WITHOUT_COLON = "ParserError: Expected a `:` after a specification attribute";
    inline constexpr std::string_view PAR_ERROR_SPEC_UNKNOWN_ID = "ParserError: An incorrect specification attribute id was found";
    inline constexpr std::string_view PAR_ERROR_SPEC_STRING_INCORRECT = "ParserError: Specification attribute must be a string literal for `";
    inline constexpr std::string_view PAR_ERROR_SPEC_U32_INCORRECT = "ParserError: Specification attribute must be a u32 literal for `";
    inline constexpr std::string_view PAR_ERROR_SPEC_BOOL_INCORRECT = "ParserError: Specification attribute must be a bool literal for `";
    inline constexpr std::string_view PAR_ERROR_IF_WITHOUT_LPARENTH = "ParserError: Expected a `(` to begin a if condition";
    inline constexpr std::string_view PAR_ERROR_UNTERMINATED_IF_CND = "ParserError: Expected a `)` to end a if condition";
    inline constexpr std::string_view PAR_ERROR_WHILE_WITHOUT_LPARENTH = "ParserError: Expected a `(` to begin a while condition";
    inline constexpr std::string_view PAR_ERROR_UNTERMINATED_WHILE_CND = "ParserError: Expected a `)` to end a while condition";
    inline constexpr std::string_view PAR_ERROR_NOT_SEPARATED_STATS = "ParserError: Statements must be separated by a new-line or a `;`";
    inline constexpr std::string_view PAR_ERROR_VAR_CONST_WITHOUT_ANYTHING = "ParserError: Cannot declare a variable or a constant without type and expression";
    inline constexpr std::string_view PAR_ERROR_FOR_WITHOUT_LPARENTH = "ParserError: Expected a `(` to begin a for declaration";
    inline constexpr std::string_view PAR_ERROR_UNTERMINATED_FOR = "ParserError: Expected a `)` to end a for declaration";
    inline constexpr std::string_view PAR_ERROR_FOR_WITHOUT_SEMICOLON = "ParserError: Expected a `;` between sections of a for declaration";


    // Analyzer errors
    inline constexpr std::string_view ANL_ERROR_NO_UNIQUE_INIT = "AnalyzerError: A correct main apica source file must contain a unique `init` entrypoint on the main global scope";
    inline constexpr std::string_view ANL_ERROR_NO_UNIQUE_UPDATE = "AnalyzerError: A correct main apica source file must contain a unique `update` entrypoint on the main global scope";
    inline constexpr std::string_view ANL_ERROR_NO_UNIQUE_QUIT = "AnalyzerError: A correct main apica source file must contain a unique `quit` entrypoint on the main global scope";
    inline constexpr std::string_view ANL_ERROR_NO_SPECS = "AnalyzerError: A correct main apica source file must contain a unique block of specifications on the main global scope";
    inline constexpr std::string_view ANL_ERROR_NO_TITLE_SPEC = "AnalyzerError: A correct main apica source file must contain a `title` specification attribute on the block of specifications";
    inline constexpr std::string_view ANL_ERROR_NO_ID_SPEC = "AnalyzerError: A correct main apica source file must contain a `id` specification attribute on the block of specifications";
    inline constexpr std::string_view ANL_ERROR_NO_VERSION_SPEC = "AnalyzerError: A correct main apica source file must contain a `version` specification attribute on the block of specifications";
    inline constexpr std::string_view ANL_ERROR_NOT_UNIQUE_VC = "AnalyzerError: Cannot declare twice a variable or constant with the same name in the same scope";
    inline constexpr std::string_view ANL_ERROR_UNDEFINED_VC = "AnalyzerError: Cannot call an undefined variable or constant";


    // Emitter errors
    inline constexpr std::string_view EMT_ERROR_OPEN_OUTPUT_FILE = "EmitterError: Failed to open the output file at `";
    inline constexpr std::string_view EMT_ERROR_INCORRECT_NODE = "EmitterError: Incorrect node found `";
    inline constexpr std::string_view EMT_ERROR_WRITING_U8 = "EmitterError: Failed to emit a 8-bit word";
    inline constexpr std::string_view EMT_ERROR_WRITING_U16 = "EmitterError: Failed to emit a 16-bit word";
    inline constexpr std::string_view EMT_ERROR_WRITING_U32 = "EmitterError: Failed to emit a 32-bit word";
    inline constexpr std::string_view EMT_ERROR_WRITING_U64 = "EmitterError: Failed to emit a 64-bit word";
    inline constexpr std::string_view EMT_ERROR_WRITING_STR = "EmitterError: Failed to emit a string";
    inline constexpr std::string_view EMT_ERROR_USER_DEFINED_PACKAGE = "EmitterError: User-defined packages are not supported yet, only supported is `APICA`";
    inline constexpr std::string_view EMT_ERROR_INCORRECT_UNARY_OPERATOR = "EmitterError: An unsupported unary operator was found";
    inline constexpr std::string_view EMT_ERROR_INCORRECT_BINARY_OPERATOR = "EmitterError: An unsupported binary operator was found";
    inline constexpr std::string_view EMT_ERROR_INCORRECT_LITERAL = "EmitterError: An unsupported literal type was found `";
    inline constexpr std::string_view EMT_ERROR_VAR_CONST_INCORRECT_TYPE = "EmitterError: An unknown type was found to declare a variable or a constant `";
    inline constexpr std::string_view EMT_ERROR_INCORRECT_VAR_CONST_BUILTIN = "EmitterError: An unknown builtin variable or constant was found";
    inline constexpr std::string_view EMT_ERROR_INCORRECT_TYPE = "EmitterError: An unknown type declaration was found in a typeof";
    inline constexpr std::string_view EMT_ERROR_UNKNOWN_FUNC_CALL = "EmitterError: Cannot call an undefined function";


    // Optimizer errors
    inline constexpr std::string_view OPM_ERROR_NULL_OPERAND_FOUND = "OptimizerError: The operand is not a valid expression";
    inline constexpr std::string_view OPM_ERROR_WHILE_UNEXPECTED_CONDITION = "OptimizerError: The while condition is not a valid expression";
    inline constexpr std::string_view OPM_ERROR_FOR_UNEXPECTED_INIT = "OptimizerError: The for initialize-part is not a valid expression";
    inline constexpr std::string_view OPM_ERROR_FOR_UNEXPECTED_END = "OptimizerError: The for end-part is not a valid expression";
    inline constexpr std::string_view OPM_ERROR_FOR_UNEXPECTED_CONDITION = "OptimizerError: The for condition is not a valid expression";
    inline constexpr std::string_view OPM_ERROR_IF_UNEXPECTED_CONDITION = "OptimizerError: The if condition is not a valid expression";
    inline constexpr std::string_view OPM_ERROR_UNEXPECTED_PARAMETER = "OptimizerError: The parameter is not a valid expression";
    inline constexpr std::string_view OPM_ERROR_RETURN_UNEXPECTED = "OptimizerError: The return expression is not a valid expression";
    inline constexpr std::string_view OPM_ERROR_TERNARY_CONDITION_UNEXPECTED = "OptimizerError: The ternary-operation condition is not a valid expression";
    inline constexpr std::string_view OPM_ERROR_TERNARY_TRUE_UNEXPECTED = "OptimizerError: The ternary-operation true expression is not a valid expression";
    inline constexpr std::string_view OPM_ERROR_TERNARY_FALSE_UNEXPECTED = "OptimizerError: The ternary-operation false expression is not a valid expression";
    inline constexpr std::string_view OPM_ERROR_BINARY_OPERATOR_EXPECTED_VAR_CONST = "OptimizerError: The binary operator cannot be applied to a non var-const-call left operand";
    inline constexpr std::string_view OPM_ERROR_UNARY_OPERATOR_EXPECTED_VAR_CONST = "OptimizerError: The unary operator cannot be applied to a non var-const-call operand";
}
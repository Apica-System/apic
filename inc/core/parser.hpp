#pragma once

#include "nodes/compound.hpp"
#include "nodes/parameter.hpp"
#include "utils/source.hpp"
#include "utils/token.hpp"
#include "bytecodes.hpp"

namespace core {
    class Parser final {
    public:
        Parser(const utils::SourceText &source);

        nodes::NodeCompound *parse();
    private:
        const utils::SourceText &source;
        std::vector<utils::Token> tokens;
        uint64_t index;

        void skipNewLines();
        utils::Token get() const;
        utils::Token getAt(uint64_t offset) const;
        utils::Token getAndAdvance();
        void advance();
        void matchToken(utils::TokenKind expected, const std::string &error_text);

        nodes::Node *parseStatement(uint8_t modifier);
        std::optional<nodes::Node*> parseController(uint8_t modifier);
        nodes::Node *parseBinaryUnaryExpression(uint8_t precedence);
        nodes::Node *parsePrimaryExpression();
        nodes::Node *parseCompound(uint8_t modifier);
        nodes::Node *parseEntrypoint(uint8_t modifier);
        nodes::Node *parseSpecifications(uint8_t modifier);
        nodes::Node *parseVarConstDecl(bool is_const);
        nodes::Node *parseGlobalScope(uint8_t modifier);
        nodes::Node *parseIfStatement(uint8_t modifier);
        nodes::Node *parseWhileStatement(uint8_t modifier);
        nodes::Node *parseTypeof(const utils::Position &typeof_position);

        nodes::Node *parseTernaryExpression(const utils::Position &question_pos);
        nodes::Node *parsePackageCall(const utils::Token &token);
        nodes::Node *parseVarConstCall(const utils::Token &token) const;
        nodes::Node *parseFunctionCall(const utils::Token &token);
        nodes::NodeParameter *parseParameter();

        std::optional<nodes::Node*> parseSpecification();
        nodes::Node *parseSpecificationDataString(common::bytecodes::ApicaSpecificationBytecode bytecode, const std::string &name);
        nodes::Node *parseSpecificationDataU32(common::bytecodes::ApicaSpecificationBytecode bytecode, const std::string &name);
        nodes::Node *parseSpecificationDataBool(common::bytecodes::ApicaSpecificationBytecode bytecode, const std::string &name);

        nodes::Node *createInteger(const utils::Token &token) const;
        nodes::Node *createBinaryInteger(const utils::Token &token) const;
        nodes::Node *createOctalInteger(const utils::Token &token) const;
        nodes::Node *createHexadecimalInteger(const utils::Token &token) const;
        nodes::Node *createBigInteger(const utils::Token &token) const;
        nodes::Node *createBigBinaryInteger(const utils::Token &token) const;
        nodes::Node *createBigOctalInteger(const utils::Token &token) const;
        nodes::Node *createBigHexadecimalInteger(const utils::Token &token) const;
        nodes::Node *createDecimal(const utils::Token &token) const;
        nodes::Node *createBigDecimal(const utils::Token &token) const;

        nodes::Node *createCharacter(const utils::Token &token) const;
        nodes::Node *createString(const utils::Token &token) const;
        nodes::Node *createRawString(const utils::Token &token) const;
    };
}
#include "utils/decoder.hpp"
#include "utils/chars.hpp"

namespace utils {
    uint32_t hexadecimalToValue(char c) {
        if (c < 'A')
            return c - '0';
        else if (c < 'a')
            return 10 + c - 'A';
        else
            return 10 + c - 'a';
    }

    void decodeSpecialCharacter(const std::string &text, uint64_t *index, char special[4]) {
        (*index)++;
        char first = text[(*index)++];

        switch (first) {
            case '0':
                special[0] = '\0'; special[1] = '\0';
                break;
            
            case 'n':
                special[0] = '\n'; special[1] = '\0';
                break;
            
            case 't':
                special[0] = '\t'; special[1] = '\0';
                break;
            
            case 'r':
                special[0] = '\r'; special[1] = '\0';
                break;
            
            case 'f':
                special[0] = '\f'; special[1] = '\0';
                break;
            
            case 'v':
                special[0] = '\v'; special[1] = '\0';
                break;
            
            case 'b':
                special[0] = '\b'; special[1] = '\0';
                break;
            
            case 'a':
                special[0] = '\a'; special[1] = '\0';
                break;
            
            case 'B': {
                uint32_t character = 0;
                for (uint8_t i = 0; i < 32; i++) {
                    if (text[*index] < '0' || text[*index] > '1') break;
                    character = (character << 1) + (text[(*index)++] - '0');
                }

                uint8_t length = 0;
                if (character & 0xFF000000) special[length++] = (character >> 24) & 0xFF;
                if (character & 0x00FF0000) special[length++] = (character >> 16) & 0xFF;
                if (character & 0x0000FF00) special[length++] = (character >> 8) & 0xFF;
                if (character & 0x000000FF) special[length++] = character & 0xFF;
            } break;

            case 'o': {
                uint32_t character = 0;
                for (uint8_t i = 0; i < 10; i++) {
                    if (text[*index] < '0' || text[*index] > '7') break;
                    character = (character << 3) + (text[(*index)++] - '0');
                }

                uint8_t length = 0;
                if (character & 0xFF000000) special[length++] = (character >> 24) & 0xFF;
                if (character & 0x00FF0000) special[length++] = (character >> 16) & 0xFF;
                if (character & 0x0000FF00) special[length++] = (character >> 8) & 0xFF;
                if (character & 0x000000FF) special[length++] = character & 0xFF;
            } break;

            case 'u': case 'x': {
                uint32_t character = 0;
                for (uint8_t i = 0; i < 8; i++) {
                    if (!isxdigit(text[*index])) break;
                    character = (character << 4) + hexadecimalToValue(text[(*index)++]);
                }

                uint8_t length = 0;
                if (character & 0xFF000000) special[length++] = (character >> 24) & 0xFF;
                if (character & 0x00FF0000) special[length++] = (character >> 16) & 0xFF;
                if (character & 0x0000FF00) special[length++] = (character >> 8) & 0xFF;
                if (character & 0x000000FF) special[length++] = character & 0xFF;
            } break;
            
            default:
                special[0] = first; special[1] = '\0';
                break;
        }
    }

    uint32_t decodeInteger(const std::string &text) {
        uint32_t result = 0;
        for (char c : text) {
            if (c == '_') continue;
            result = result * 10 + (c - '0');
        }

        return result;
    }

    uint32_t decodeBinary(const std::string &text) {
        uint32_t result = 0;
        for (char c : text) {
            if (c == '_') continue;
            result = (result << 1) + (c - '0');
        }

        return result;
    }

    uint32_t decodeOctal(const std::string &text) {
        uint32_t result = 0;
        for (char c : text) {
            if (c == '_') continue;
            result = (result << 3) + (c - '0');
        }

        return result;
    }

    uint32_t decodeHexadecimal(const std::string &text) {
        uint32_t result = 0;
        for (char c : text) {
            if (c == '_') continue;
            result = (result << 4) + hexadecimalToValue(c);
        }

        return result;
    }

    uint64_t decodeBigInteger(const std::string &text) {
        uint64_t result = 0;
        for (char c : text) {
            if (c == '_') continue;
            result = (result * 10) + (c - '0');
        }

        return result;
    }

    uint64_t decodeBigBinary(const std::string &text) {
        uint64_t result = 0;
        for (char c : text) {
            if (c == '_') continue;
            result = (result << 1) + (c - '0');
        }

        return result;
    }
    
    uint64_t decodeBigOctal(const std::string &text) {
        uint64_t result = 0;
        for (char c : text) {
            if (c == '_') continue;
            result = (result << 3) + (c - '0');
        }

        return result;
    }
    
    uint64_t decodeBigHexadecimal(const std::string &text) {
        uint64_t result = 0;
        for (char c : text) {
            if (c == '_') continue;
            result = (result << 4) + hexadecimalToValue(c);
        }

        return result;
    }

    float decodeF32(const std::string &text) {
        float result = 0.f;
        uint64_t i = 0;
        while (i < text.length()) {
            char c = text[i];
            if (c == '_') {
                i++;
                continue;
            }

            if (c < '0' || c > '9') break;

            result = result * 10.f + (c - '0');
            i++;
        }

        if (i < text.length() && text[i] == '.') {
            i++;
            float factor = 0.1f;
            while (i < text.length()) {
                char c = text[i];
                if (c == '_') {
                    i++;
                    continue;
                }

                if (c < '0' || c > '9') break;

                result += (c - '0') * factor;
                factor *= 0.1f;
                i++;
            }
        }

        if (i < text.length() && (text[i] == 'e' || text[i] == 'E')) {
            i++;
            bool expNegative = false;
            if (text[i++] == '-')
                expNegative = true;

            int exponent = 0;
            while (i < text.length()) {
                char c = text[i];
                if (c == '_') {
                    i++;
                    continue;
                }

                exponent = exponent * 10 + (c - '0');
                i++;
            }

            float power = 1.f;
            for (int j = 0; j < exponent; j++)
                power *= 10.f;

            if (expNegative) result /= power;
            else result *= power;
        }

        return result;
    }
    
    double decodeF64(const std::string &text) {
        double result = 0.0;
        uint64_t i = 0;
        while (i < text.length()) {
            char c = text[i];
            if (c == '_') {
                i++;
                continue;
            }

            if (c < '0' || c > '9') break;

            result = result * 10.0 + (c - '0');
            i++;
        }

        if (i < text.length() && text[i] == '.') {
            i++;
            double factor = 0.1;
            while (i < text.length()) {
                char c = text[i];
                if (c == '_') {
                    i++;
                    continue;
                }

                if (c < '0' || c > '9') break;

                result += (c - '0') * factor;
                factor *= 0.1;
                i++;
            }
        }

        if (i < text.length() && (text[i] == 'e' || text[i] == 'E')) {
            i++;
            bool expNegative = false;
            if (text[i++] == '-')
                expNegative = true;

            int exponent = 0;
            while (i < text.length()) {
                char c = text[i];
                if (c == '_') {
                    i++;
                    continue;
                }

                exponent = exponent * 10 + (c - '0');
                i++;
            }

            double power = 1.0;
            for (int j = 0; j < exponent; j++)
                power *= 10.0;

            if (expNegative) result /= power;
            else result *= power;
        }

        return result;
    }

    uint32_t decodeCharacter(const std::string &text) {
        uint32_t result = 0;
        if (text[0] == '\\') {
            uint64_t index = 0;
            char special[4] = {0};
            decodeSpecialCharacter(text, &index, special);
            result = static_cast<uint8_t>(special[0]);
            for (uint8_t i = 1; i < 4; i++) {
                if (special[i] == '\0') break;
                result = (result << 8) | static_cast<uint8_t>(special[i]);
            }
        } else {
            uint8_t length = getUtf8Length(text[0]);
            for (uint8_t i = 0; i < length; i++)
                result = (result << 8) | static_cast<uint8_t>(text[i]);
        }

        return result;
    }

    std::string decodeString(const std::string &text) {
        std::string str;
        uint64_t index = 0;

        while (index < text.size()) {
            if (text[index] == '\\') {
                char special[4] = {0};
                decodeSpecialCharacter(text, &index, special);
                for (char c : special) {
                    if (c == '\0') break;
                    str += c;
                }
            } else {
                uint8_t length = getUtf8Length(text[index]);
                for (uint8_t i = 0; i < length; i++)
                    str += text[index++];
            }
        }

        return str;
    }
}
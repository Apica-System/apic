#pragma once

#include <cstdint>
#include <string>

namespace utils {
    uint32_t decodeInteger(const std::string &text);
    uint32_t decodeBinary(const std::string &text);
    uint32_t decodeOctal(const std::string &text);
    uint32_t decodeHexadecimal(const std::string &text);

    uint64_t decodeBigInteger(const std::string &text);
    uint64_t decodeBigBinary(const std::string &text);
    uint64_t decodeBigOctal(const std::string &text);
    uint64_t decodeBigHexadecimal(const std::string &text);

    float decodeF32(const std::string &text);
    double decodeF64(const std::string &text);

    uint32_t decodeCharacter(const std::string &text);
    std::string decodeString(const std::string &text);
}
#pragma once

#include <cstdint>

namespace utils {
    constexpr uint8_t getUtf8Length(char c) {
        if ((c & 0b11100000) == 0b11000000) {
            return 2;
        } else if ((c & 0b11110000) == 0b11100000) {
            return 3;
        } else if ((c & 0b11111000) == 0b11110000) {
            return 4;
        } else {
            return 1;
        }
    }

    constexpr bool isIdPart(uint8_t c) {
        return (c < 0b01111111)
            ? (isalnum(c) || c == '_')
            : true;
    }
}
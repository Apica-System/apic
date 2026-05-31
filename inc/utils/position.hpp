#pragma once

#include <cstdint>

namespace utils {
    class Position final {
    public:
        Position();
        Position(uint64_t ascii_start, uint64_t ascii_length, uint64_t utf8_start, uint64_t utf8_length);
        Position(const Position &from, const Position &to);

        uint64_t getAsciiStart() const;
        uint64_t getAsciiLength() const;
        uint64_t getAsciiEnd() const;

        uint64_t getUtf8Start() const;
        uint64_t getUtf8Length() const;
        uint64_t getUtf8End() const;

        void show(char end) const;
    private:
        uint64_t ascii_start, ascii_length;
        uint64_t utf8_start, utf8_length;
    };
}
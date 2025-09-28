#pragma once

#include "base.hpp"

namespace utils {
    class Position {
    public:
        Position(apic_u64 ascii_start, apic_u64 ascii_length, apic_u64 utf8_start, apic_u64 utf8_length);
        Position(const Position *copy);
        ~Position();

        const apic_u64 GetAsciiStart() const;
        const apic_u64 GetAsciiLength() const;
        const apic_u64 GetAsciiEnd() const;
        const apic_u64 GetUtf8Start() const;
        const apic_u64 GetUtf8Length() const;
        const apic_u64 GetUtf8End() const;

        void Show(char end);
    private:
        apic_u64 ascii_start, ascii_length;
        apic_u64 utf8_start, utf8_length;
    };
}
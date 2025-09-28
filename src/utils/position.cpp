#include "utils/position.hpp"

namespace utils {
    Position::Position(apic_u64 ascii_start, apic_u64 ascii_length, apic_u64 utf8_start, apic_u64 utf8_length) {
        this->ascii_start = ascii_start;
        this->ascii_length = ascii_length;
        this->utf8_start = utf8_start;
        this->utf8_length = utf8_length;
    }

    Position::Position(const Position *copy) {
        this->ascii_start = copy->ascii_start;
        this->ascii_length = copy->ascii_length;
        this->utf8_start = copy->utf8_start;
        this->utf8_length = copy->utf8_length;
    }

    Position::~Position() {
        // Nothing to do
    }

    const apic_u64 Position::GetAsciiStart() const {
        return this->ascii_start;
    }

    const apic_u64 Position::GetAsciiLength() const {
        return this->ascii_length;
    }

    const apic_u64 Position::GetAsciiEnd() const {
        return this->ascii_start + this->ascii_length;
    }
    
    const apic_u64 Position::GetUtf8Start() const {
        return this->utf8_start;
    }

    const apic_u64 Position::GetUtf8Length() const {
        return this->utf8_length;
    }
    
    const apic_u64 Position::GetUtf8End() const {
        return this->utf8_start + this->utf8_length;
    }

    void Position::Show(char end) {
        printf("Position(ascii: %llu.%llu, utf8: %llu.%llu)%c", this->ascii_start, this->ascii_length, this->utf8_start, this->utf8_length, end);
    }
}
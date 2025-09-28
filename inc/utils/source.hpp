#pragma once

#include "utils/token.hpp"

namespace utils {
    class SourceText {
    public:
        SourceText(const apic_string &source);
        ~SourceText();

        const apic_char GetCharAt(apic_u64 index);
        const apic_u64 GetLineLength(apic_u64 index);
        apic_string GetLineText(apic_u64 index);
        void GetInfosForPosition(Position *position, apic_u64 *line_start, apic_u64 *line_end, apic_u64 *left_offset, apic_u64 *right_offset);
        apic_string GetTextFromPosition(Position *position);
    private:
        apic_string source;
        std::vector<Position*> lines;

        void CalculateLines();
    };
}
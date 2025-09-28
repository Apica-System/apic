#include "utils/source.hpp"

namespace utils {
    SourceText::SourceText(const apic_string &source)
        : source(source) {
        this->CalculateLines();
    }

    SourceText::~SourceText() {
        for (Position *line : this->lines)
            delete line;
    }

    const apic_char SourceText::GetCharAt(apic_u64 index) {
        if (index >= this->source.length())
            return '\0';
        
        return this->source[index];
    }

    const apic_u64 SourceText::GetLineLength(apic_u64 index) {
        if (index >= this->lines.size())
            return 0;
        
        return this->lines[index]->GetUtf8Length();
    }

    apic_string SourceText::GetLineText(apic_u64 index) {
        if (index >= this->lines.size())
            return "";
        
        Position *line = this->lines[index];
        apic_string text = this->source.substr(line->GetAsciiStart(), line->GetAsciiLength());
        if (text[text.length() - 1] != '\n')
            text.push_back('\n');
        
        return text;
    }

    void SourceText::GetInfosForPosition(Position *position, apic_u64 *line_start, apic_u64 *line_end, apic_u64 *left_offset, apic_u64 *right_offset) {
        apic_u64 i = 0;
        for (Position *line : this->lines) {
            if (position->GetUtf8Start() >= line->GetUtf8Start() && position->GetUtf8Start() <= line->GetUtf8End()) {
                *line_start = i;
                *left_offset = position->GetUtf8Start() - line->GetUtf8Start();
            }

            if (position->GetUtf8End() >= line->GetUtf8Start() && position->GetUtf8End() <= line->GetUtf8End()) {
                *line_end = i;
                *right_offset = line->GetUtf8End() - position->GetUtf8End();
            }

            i++;
        }
    }

    apic_string SourceText::GetTextFromPosition(Position *position) {
        if (position->GetAsciiEnd() > this->source.length())
            return "";
        
        return this->source.substr(position->GetAsciiStart(), position->GetAsciiLength());
    }

    void SourceText::CalculateLines() {
        if (!this->source.length())
            return;
        
        apic_u64 ascii_pos = 0, old_ascii_pos = 0;
        apic_u64 utf8_pos = 0, old_utf8_pos = 0;
        apic_u8 utf8_awaited = chars_length_utf8(this->source[0]);
        for (char _ : this->source) {
            utf8_awaited--;
            if (!utf8_awaited) {
                utf8_pos++;
                utf8_awaited = chars_length_utf8(this->source[ascii_pos+1]);
                if (this->source[ascii_pos] == '\n') {
                    this->lines.push_back(new Position(old_ascii_pos, ascii_pos + 1 - old_ascii_pos, old_utf8_pos, utf8_pos - old_utf8_pos));
                    old_ascii_pos = ascii_pos + 1;
                    old_utf8_pos = utf8_pos;
                }
            }

            ascii_pos++;
        }

        this->lines.push_back(new Position(old_ascii_pos, ascii_pos + 1 - old_ascii_pos, old_utf8_pos, utf8_pos + 1 - old_utf8_pos));
    }
}
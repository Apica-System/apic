#include "utils/source.hpp"
#include "utils/chars.hpp"

using namespace utils;

SourceText::SourceText(const std::string &source)
    : source(source) {
    this->calculateLines();
}

char SourceText::getCharAt(uint64_t index) const {
    if (index >= this->source.length())
        return '\0';
    
    return this->source[index];
}

std::optional<uint64_t> SourceText::getLineLength(uint64_t line_index) const {
    if (line_index >= this->lines.size())
        return std::nullopt;
    
    return this->lines[line_index].getUtf8Length();
}

std::optional<std::string> SourceText::getLineText(uint64_t line_index) const {
    if (line_index >= this->lines.size())
        return std::nullopt;
    
    Position line = this->lines[line_index];
    return this->source.substr(line.getAsciiStart(), line.getAsciiLength());
}

std::optional<std::string> SourceText::getTextFromPosition(const Position &position) const {
    if (position.getAsciiEnd() > this->source.length())
        return std::nullopt;
    
    return this->source.substr(position.getAsciiStart(), position.getAsciiLength());
}

void SourceText::getInfosForPosition(const Position &position, SourceInformations &infos) const {
    uint64_t index = 0;
    infos.success = false;

    for (const Position &line : this->lines) {
        if (position.getUtf8Start() >= line.getUtf8Start() && position.getUtf8Start() <= line.getUtf8End()) {
            infos.line_start = index;
            infos.left_offset = position.getUtf8Start() - line.getUtf8Start();
        }

        if (position.getUtf8End() >= line.getUtf8Start() && position.getUtf8End() <= line.getUtf8End()) {
            infos.line_end = index;
            infos.right_offset = line.getUtf8End() - position.getUtf8End();
            infos.success = true;
            return;
        }

        index++;
    }
}

void SourceText::calculateLines() {
    if (this->source.length() == 0)
        return;
    
    uint64_t old_ascii_pos = 0, old_utf8_pos = 0;
    uint64_t actual_ascii_pos = 0, actual_utf8_pos = 0;
    uint8_t utf8_length = getUtf8Length(this->source[0]);

    for (char c : this->source) {
        if (c == '\n') {
            lines.push_back(Position(old_ascii_pos, actual_ascii_pos - old_ascii_pos, old_utf8_pos, actual_utf8_pos - old_utf8_pos));
            old_ascii_pos = actual_ascii_pos + 1;
            old_utf8_pos = actual_utf8_pos + 1;
        }

        actual_ascii_pos++;
        utf8_length--;
        if (utf8_length == 0) {
            utf8_length = getUtf8Length(c);
            actual_utf8_pos++;
        }
    }

    lines.push_back(Position(old_ascii_pos, actual_ascii_pos - old_ascii_pos, old_utf8_pos, actual_utf8_pos - old_utf8_pos));
}
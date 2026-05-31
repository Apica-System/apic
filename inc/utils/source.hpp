#pragma once

#include "utils/position.hpp"
#include <string>
#include <vector>
#include <optional>

namespace utils {
    typedef struct {
        bool success;
        uint64_t line_start, line_end;
        uint64_t left_offset, right_offset;
    } SourceInformations;

    class SourceText final {
    public:
        SourceText(const std::string &source);

        char getCharAt(uint64_t index) const;
        std::optional<uint64_t> getLineLength(uint64_t line_index) const;
        std::optional<std::string> getLineText(uint64_t line_index) const;
        std::optional<std::string> getTextFromPosition(const Position &position) const;
        void getInfosForPosition(const Position &position, SourceInformations &infos) const;
    private:
        std::string source;
        std::vector<Position> lines;

        void calculateLines();
    };
}
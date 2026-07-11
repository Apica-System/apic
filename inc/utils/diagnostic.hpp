#pragma once

#include "utils/position.hpp"
#include <string>
#include <optional>
#include "utils/source.hpp"
#include "values/error.hpp"

#include "nlohmann/json.hpp"

namespace utils {
    enum DiagnosticKind : uint8_t {
        Success,
        Information,
        Warning,
        Error
    };

    class Diagnostic final {
    public:
        Diagnostic(DiagnosticKind kind, const std::string &message);
        Diagnostic(DiagnosticKind kind, const std::string &message, const Position &position);
        Diagnostic(common::values::ValueError *error, const Position &position);

        void show(const SourceText &source) const;
        void lspFlush(const SourceText &source, nlohmann::json &diagnostic_list);

        DiagnosticKind getKind() const;
    private:
        DiagnosticKind kind;
        std::string message;
        std::optional<Position> position;

        void showWithoutPosition(const std::string &color) const;
        void showWithPosition(const std::string &color, const SourceText &source) const;
        static std::string getColorByKind(DiagnosticKind kind);
    };
}
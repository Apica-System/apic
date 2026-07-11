#pragma once

#include "utils/diagnostic.hpp"

namespace utils {
    class DiagnosticBag final {
    public:
        static DiagnosticBag &getInstance();

        void addDiagnostic(const Diagnostic &diagnostic);
        void showAll(const SourceText &source) const;
        void clear();
        void lspFlush(const SourceText &source, const std::string &uri);

        bool hasAny() const;
        bool hasAnyError() const;
        bool hasAnyWarning() const;
    private:
        std::vector<Diagnostic> diagnostics;
        uint64_t errors, warnings;

        DiagnosticBag();

        DiagnosticBag(DiagnosticBag &other) = delete;
        void operator=(const DiagnosticBag &) = delete;
    };
}
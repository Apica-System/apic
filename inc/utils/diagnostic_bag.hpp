#pragma once

#include "utils/diagnostic.hpp"

namespace utils {
    class DiagnosticBag {
    public:
        DiagnosticBag();
        ~DiagnosticBag();

        void Add(Diagnostic *diagnostic);
        void ShowAll(SourceText *source);
        const apic_bool HasAny() const;
        const apic_bool HasAnyError() const;
        const apic_bool HasAnyWarning() const;
        void Clear();
    private:
        std::vector<Diagnostic*> diagnostics;
        apic_u64 errors, warnings;
    };
}
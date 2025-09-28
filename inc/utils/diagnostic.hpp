#pragma once

#include "utils/writer.hpp"

namespace utils {
    enum DiagnosticKind : apic_u8 {
        DIAG_Success, DIAG_Info, DIAG_Warning, DIAG_Error
    };

    class Diagnostic {
    public:
        Diagnostic(DiagnosticKind kind, const apic_string &message, Position *position);
        Diagnostic(DiagnosticKind kind, const apic_string &message);
        ~Diagnostic();

        const DiagnosticKind GetKind() const;
        void Show(SourceText *source);
    private:
        DiagnosticKind kind;
        apic_string message;
        Position *position;

        void ShowWithoutPosition(const apic_char *color);
        void ShowWithPosition(const apic_char *color, SourceText *source);
        static const apic_char *GetColorByKind(DiagnosticKind kind);
    };
}
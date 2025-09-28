#include "utils/diagnostic.hpp"

namespace utils {
    Diagnostic::Diagnostic(DiagnosticKind kind, const apic_string &message, Position *position) 
        : message(message) {
        this->kind = kind;
        this->position = position;
    }

    Diagnostic::Diagnostic(DiagnosticKind kind, const apic_string &message) 
        : message(message) {
        this->kind = kind;
        this->position = nullptr;
    }

    Diagnostic::~Diagnostic() {
        if (this->position) delete this->position;
    }

    const DiagnosticKind Diagnostic::GetKind() const {
        return this->kind;
    }

    void Diagnostic::Show(SourceText *source) {
        const apic_char *color = Diagnostic::GetColorByKind(this->kind);
        if (this->position)
            this->ShowWithPosition(color, source);
        else
            this->ShowWithoutPosition(color);
    }

    void Diagnostic::ShowWithoutPosition(const apic_char *color) {
        printf("%s%s.\033[0m", color, this->message.c_str());
    }

    void Diagnostic::ShowWithPosition(const apic_char *color, SourceText *source) {
        apic_u64 line_start = 0, line_end = 0, left_offset = 0, right_offset = 0;
        source->GetInfosForPosition(this->position, &line_start, &line_end, &left_offset, &right_offset);
        if (line_start == line_end) {
            apic_string line_text = source->GetLineText(line_start);
            apic_string offset_text = apic_string(left_offset, ' ');
            apic_string arrow_text = apic_string(this->position->GetUtf8Length(), '^');

            printf("%s%s%s%s\n[At line %llu, column %llu-%llu]: %s.\033[0m\n", line_text.c_str(), color, offset_text.c_str(), arrow_text.c_str(), line_start + 1, left_offset + 1, left_offset + this->position->GetUtf8Length(), this->message.c_str());
        } else {
            for (apic_u64 i = line_start; i <= line_end; i++) {
                apic_string line_text = source->GetLineText(i);
                apic_u64 line_length = source->GetLineLength(i);
                printf("%s", line_text.c_str());

                if (i == line_start) {
                    apic_string offset_text = apic_string(left_offset, ' ');
                    apic_string arrow_text = apic_string(line_length - left_offset, '^');

                    printf("%s%s%s\033[0m\n", color, offset_text.c_str(), arrow_text.c_str());
                } else if (i == line_end) {
                    apic_string arrow_text = apic_string(line_length - right_offset, '^');

                    printf("%s%s\033[0m\n", color, arrow_text.c_str());
                } else {
                    apic_string arrow_text = apic_string(line_length, '^');

                    printf("%s%s\033[0m\n", color, arrow_text.c_str());
                }
            }

            printf("%s[From line %llu to line %llu]: %s.\033[0m\n", color, line_start + 1, line_end + 1, this->message.c_str());
        }
    }
    
    const apic_char *Diagnostic::GetColorByKind(DiagnosticKind kind) {
        switch (kind) {
            case DiagnosticKind::DIAG_Success: return "\033[32m";
            case DiagnosticKind::DIAG_Info: return "\033[37m";
            case DiagnosticKind::DIAG_Error: return "\033[31m";
            case DiagnosticKind::DIAG_Warning: return "\033[33m";
            default: return "\033[37m";
        }
    }
}
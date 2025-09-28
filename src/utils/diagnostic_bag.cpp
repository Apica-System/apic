#include "utils/diagnostic_bag.hpp"

namespace utils {
    DiagnosticBag::DiagnosticBag() {
        this->errors = 0;
        this->warnings = 0;
    }

    DiagnosticBag::~DiagnosticBag() {
        for (Diagnostic *diag : this->diagnostics)
            delete diag;
    }

    void DiagnosticBag::Add(Diagnostic *diagnostic) {
        if (diagnostic->GetKind() == DiagnosticKind::DIAG_Error)
            this->errors++;
        else if (diagnostic->GetKind() == DiagnosticKind::DIAG_Warning)
            this->warnings++;

        this->diagnostics.push_back(diagnostic);
    }

    void DiagnosticBag::ShowAll(SourceText *source) {
        for (Diagnostic *diag : this->diagnostics)
            diag->Show(source);
    }

    const apic_bool DiagnosticBag::HasAny() const {
        return this->diagnostics.size() != 0;
    }

    const apic_bool DiagnosticBag::HasAnyError() const {
        return this->errors != 0;
    }

    const apic_bool DiagnosticBag::HasAnyWarning() const {
        return this->warnings != 0;
    }

    void DiagnosticBag::Clear() {
        this->diagnostics.clear();
    }
}
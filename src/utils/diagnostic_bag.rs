use crate::utils::diagnostic::{Diagnostic, DiagnosticKind};
use crate::utils::source::SourceText;

pub struct DiagnosticBag {
    diagnostics: Vec<Diagnostic>,
    errors: u64,
    warnings: u64,
}

impl DiagnosticBag {
    pub fn init() -> DiagnosticBag {
        DiagnosticBag{diagnostics: vec![], errors: 0, warnings: 0}
    }

    pub fn add(&mut self, diagnostic: Diagnostic) {
        if *diagnostic.get_kind() == DiagnosticKind::Error {
            self.errors += 1;
        } else if *diagnostic.get_kind() == DiagnosticKind::Warning {
            self.warnings += 1;
        }

        self.diagnostics.push(diagnostic);
    }

    pub fn show_all(&self, source: &SourceText) {
        let mut i: usize = 0;
        for diagnostic in &self.diagnostics {
            diagnostic.show(source);
            if i != self.diagnostics.len() - 1 {
                print!("\n\n");
            }

            i += 1;
        }

        println!("\x1b[4m{} errors, {} warnings\x1b[0m", self.errors, self.warnings);
    }

    pub fn clear(&mut self) {
        self.diagnostics.clear();
    }

    pub fn has_any(&self) -> bool {
        !self.diagnostics.is_empty()
    }

    pub fn has_any_error(&self) -> bool {
        self.errors > 0
    }

    pub fn has_any_warning(&self) -> bool {
        self.warnings > 0
    }
}
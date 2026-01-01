use crate::utils::position::Position;
use crate::utils::source::SourceText;

#[derive(PartialEq)]
pub enum DiagnosticKind {
    Success,
    Info,
    Warning,
    Error,
}

pub struct Diagnostic {
    kind: DiagnosticKind,
    message: String,
    position: Option<Position>,
}

impl Diagnostic {
    pub fn init_message(kind: DiagnosticKind, message: String) -> Diagnostic {
        Diagnostic { kind, message, position: None }
    }

    pub fn init_complete(kind: DiagnosticKind, message: String, position: Position) -> Diagnostic {
        Diagnostic { kind, message, position: Some(position) }
    }

    pub fn get_kind(&self) -> &DiagnosticKind {
        &self.kind
    }

    pub fn show(&self, source: &SourceText) {
        let color = Self::get_color_by_kind(&self.kind);
        if let Some(position) = &self.position {
            self.show_with_position(color, source, position);
        } else {
            self.show_without_position(color);
        }
    }

    fn show_without_position(&self, color: &str) {
        println!("{color}{}.\x1b[0m", self.message);
    }

    fn show_with_position(&self, color: &str, source: &SourceText, position: &Position) {
        let mut line_start: usize = 0;
        let mut line_end: usize = 0;
        let mut left_offset: usize = 0;
        let mut right_offset: usize = 0;
        source.get_infos_for_position(position, &mut line_start, &mut line_end, &mut left_offset, &mut right_offset);

        if line_start == line_end {
            let line_text = source.get_line_text(line_start).unwrap();
            let offset_text = " ".repeat(left_offset);
            let arrow_text = "^".repeat(position.get_length());

            println!("{line_text}\n{color}{offset_text}{arrow_text}\n[At line {}, column {}-{}]: {}.\x1b[0m", line_start + 1, left_offset + 1, left_offset + position.get_length(), self.message);
        } else {
            for i in line_start..line_end+1 {
                let line_text = source.get_line_text(i).unwrap();
                let line_length = source.get_line_length(i).unwrap();
                println!("{line_text}");

                if i == line_start {
                    let offset_text = " ".repeat(left_offset);
                    let arrow_text = "^".repeat(line_length - left_offset);
                    println!("{color}{offset_text}{arrow_text}\x1b[0m");
                } else if i == line_end {
                    let arrow_text = "^".repeat(line_length - right_offset);
                    println!("{color}{arrow_text}\x1b[0m");
                } else {
                    let arrow_text = "^".repeat(line_length);
                    println!("{color}{arrow_text}\x1b[0m");
                }
            }

            println!("{color}[From line {} to line {}]: {}.\x1b[0m", line_start + 1, line_end + 1, self.message);
        }
    }

    fn get_color_by_kind(kind: &DiagnosticKind) -> &str {
        match kind {
            DiagnosticKind::Success => "\x1b[32m",
            DiagnosticKind::Info => "\x1b[37m",
            DiagnosticKind::Warning => "\x1b[33m",
            DiagnosticKind::Error => "\x1b[31m",
        }
    }
}
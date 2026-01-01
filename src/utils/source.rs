use crate::utils::position::Position;

pub struct SourceText {
    source: String,
    lines: Vec<Position>,
}

impl SourceText {
    pub fn init(source: String) -> SourceText {
        let lines = Self::calculate_lines(&source);
        SourceText { source, lines }
    }

    pub fn get_char_at(&self, index: usize) -> char {
        self.source.chars().nth(index).unwrap_or('\0')
    }

    pub fn get_line_length(&self, index: usize) -> Option<usize> {
        self.lines.get(index).map(|line| line.get_length())
    }

    pub fn get_line_text(&self, index: usize) -> Option<String> {
        self.lines.get(index).map(|line| {
            self.source.chars()
                .skip(line.get_start())
                .take(line.get_length())
                .collect()
        })
    }

    pub fn get_text_from_position(&self, position: &Position) -> String {
        self.source.chars()
            .skip(position.get_start())
            .take(position.get_length())
            .collect()
    }

    pub fn get_infos_for_position(&self, pos: &Position, line_start: &mut usize, line_end: &mut usize, left_offset: &mut usize, right_offset: &mut usize) {
        let mut i: usize = 0;
        for line in &self.lines {
            if pos.get_start() >= line.get_start() && pos.get_start() <= line.get_end() {
                *line_start = i;
                *left_offset = pos.get_start() - line.get_start();
            }

            if pos.get_end() >= line.get_start() && pos.get_end() <= line.get_end() {
                *line_end = i;
                *right_offset = line.get_end() - pos.get_end();
            }

            i += 1;
        }
    }

    fn calculate_lines(source: &String) -> Vec<Position> {
        let mut lines = vec![];
        if source.is_empty() {
            return lines;
        }

        let mut old_pos: usize = 0;
        let mut actual_pos: usize = 0;
        for c in source.chars() {
            if c == '\n' {
                lines.push(Position::init(old_pos, actual_pos - old_pos));
                old_pos = actual_pos + 1;
            }

            actual_pos += 1;
        }

        lines.push(Position::init(old_pos, actual_pos - old_pos));
        lines
    }
}


#[cfg(test)]
mod tests {
    use crate::utils::source::SourceText;

    #[test]
    fn empty_source() {
        let source = SourceText::init(String::new());

        assert_eq!(source.get_line_length(0), None);
        assert_eq!(source.get_char_at(0), '\0');
        assert_eq!(source.get_line_text(0), None);
    }
}
pub struct Position {
    start: usize,
    length: usize,
}

impl Position {
    pub fn init(start: usize, length: usize) -> Position {
        Position { start, length }
    }

    pub fn init_from(pos: &Position) -> Position {
        Position { start: pos.start, length: pos.length }
    }

    pub fn get_start(&self) -> usize {
        self.start
    }

    pub fn get_length(&self) -> usize {
        self.length
    }

    pub fn get_end(&self) -> usize {
        self.start + self.length
    }

    pub fn show(&self, end: char) {
        print!("Position({}..{}){end}", self.start, self.length);
    }
}
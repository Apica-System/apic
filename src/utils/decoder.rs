fn hexadecimal_to_value(c: char) -> u32 {
    if c < 'A' {
        c as u32 - '0' as u32
    } else if c < 'a' {
        10 + c as u32 - 'A' as u32
    } else {
        10 + c as u32 - 'a' as u32
    }
}

fn decode_special_character(text: &str, index: &mut usize) -> char {
    *index += 1;
    let start = text.chars().nth(*index).unwrap();

    match start {
        '0' => '\0',
        'n' => '\n',
        't' => '\t',
        'r' => '\r',
        'f' => '\x0c',
        'b' => '\x08',
        'v' => '\x0b',

        _ => start,
    }
}

pub fn decode_integer(text: &String) -> u32 {
    let mut result: u32 = 0;
    for c in text.chars() {
        if c == '_' { continue; }
        result = result * 10 + (c as u32 - '0' as u32);
    }

    result
}

pub fn decode_binary_integer(text: &String) -> u32 {
    let mut result: u32 = 0;
    for c in text.chars() {
        if c == '_' { continue; }
        result = (result << 1) + (c as u32 - '0' as u32);
    }

    result
}

pub fn decode_octal_integer(text: &String) -> u32 {
    let mut result: u32 = 0;
    for c in text.chars() {
        if c == '_' { continue; }
        result = (result << 3) + (c as u32 - '0' as u32);
    }

    result
}

pub fn decode_hexadecimal_integer(text: &String) -> u32 {
    let mut result: u32 = 0;
    for c in text.chars() {
        if c == '_' { continue; }
        result = (result << 4) + hexadecimal_to_value(c);
    }

    result
}

pub fn decode_string(text: &String) -> String {
    let mut result: String = String::new();
    let mut index: usize = 0;
    while index < text.chars().count() {
        let current = text.chars().nth(index).unwrap();
        if current == '\\' {
            result.push(decode_special_character(&text, &mut index));
        } else {
            result.push(current);
        }

        index += 1;
    }

    result
}
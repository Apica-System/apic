use std::fs::File;
use std::io::Write;
use crate::utils::diagnostic::{Diagnostic, DiagnosticKind};
use crate::utils::diagnostic_bag::DiagnosticBag;

pub fn write_u8(file: &mut File, word: u8, diag_bag: &mut DiagnosticBag) {
    if file.write_all(&[word]).is_err() {
        diag_bag.add(Diagnostic::init_message(
            DiagnosticKind::Error,
            String::from("EmitterError: Failed to emit a 8-bit word to the output file"),
        ));
    }
}

pub fn write_u16(file: &mut File, word: u16, diag_bag: &mut DiagnosticBag) {
    if file.write_all(&word.to_le_bytes()).is_err() {
       diag_bag.add(Diagnostic::init_message(
           DiagnosticKind::Error,
           String::from("EmitterError: Failed to emit a 16-bit word to the output file"),
       )) ;
    }
}

pub fn write_u32(file: &mut File, word: u32, diag_bag: &mut DiagnosticBag) {
    if file.write_all(&word.to_le_bytes()).is_err() {
        diag_bag.add(Diagnostic::init_message(
            DiagnosticKind::Error,
            String::from("EmitterError: Failed to emit a 32-bit word to the output file"),
        ));
    }
}

pub fn write_u64(file: &mut File, word: u64, diag_bag: &mut DiagnosticBag) {
    if file.write_all(&word.to_le_bytes()).is_err() {
        diag_bag.add(Diagnostic::init_message(
            DiagnosticKind::Error,
            String::from("EmitterError: Failed to emit a 64-bit word to the output file"),
        ));
    }
}

pub fn write_string(file: &mut File, string: &String, diag_bag: &mut DiagnosticBag) {
    for byte in string.as_bytes() {
        write_u8(file, *byte, diag_bag);
    }

    write_u8(file, 0 , diag_bag);
}
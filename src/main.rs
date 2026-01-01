use std::process::exit;
use crate::core::options::{handle_options, OptionKind};
use crate::utils::apic_cmd::{build_bytecode_file, init_apic_workdir, show_apic_help};

pub mod utils;
pub mod core;
pub mod nodes;

fn main() {
    let options = handle_options();
    match options[0].get_kind() {
        OptionKind::Version => {
            println!("apic {}", env!("CARGO_PKG_VERSION"));
        },

        OptionKind::Help => show_apic_help(),
        OptionKind::Init => init_apic_workdir(),

        OptionKind::Build => if !build_bytecode_file(&options) {
            exit(1);
        },

        OptionKind::Bad => {
            println!("\x1b[31m{}\x1b[0m", options[0].get_value());
            exit(1);
        },

        _ => {
            println!("\x1b[31mapic error: an unknown command was found after handling the command-line arguments. Use `apic help` to see how to use the apica compiler\x1b[0m");
            exit(1);
        },
    }
}

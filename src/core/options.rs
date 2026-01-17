use std::env::args;

#[derive(PartialEq, Copy, Clone)]
pub enum OptionKind {
    Bad,

    Init, Build, Help, Version,

    SourceFile, OutputFile,
    
    ShowNodes, NoOpt,
}

pub struct ApicOption {
    kind: OptionKind,
    value: String,
}

pub fn contain_option_kind(options: &Vec<ApicOption>, option_kind: OptionKind) -> bool {
    for option in options {
        if *option.get_kind() == option_kind {
            return true;
        }
    }
    
    false
}

impl ApicOption {
    pub fn init(kind: OptionKind, value: String) -> ApicOption {
        ApicOption { kind, value }
    }

    pub fn get_kind(&self) -> &OptionKind {
        &self.kind
    }

    pub fn get_value(&self) -> &String {
        &self.value
    }
}

pub fn handle_options() -> Vec<ApicOption> {
    let args: Vec<String> = args().collect();
    let mut options = vec![];

    if args.len() < 2 {
        options.push(ApicOption::init(OptionKind::Bad, String::from("apic error : use `apic help` to see how to use the apica compiler")));
        return options;
    }

    if !handle_command(&mut options, &args) {
        return options;
    }

    if args.len() > 2 {
        let mut awaiting: Option<OptionKind> = None;
        for arg in &args[2..] {
            if arg.chars().nth(0).unwrap() == '-' {
                if awaiting.is_some() {
                    options.insert(0, ApicOption::init(OptionKind::Bad, String::from("apic error: cannot use an option right after an option that awaited a value. Use `apic help` to see how to use the apica compiler")));
                    return options;
                }

                match arg.as_str() {
                    "-i" => awaiting = Some(OptionKind::SourceFile),
                    "-o" => awaiting = Some(OptionKind::OutputFile),
                    "--nodes" => options.push(ApicOption::init(OptionKind::ShowNodes, String::new())),
                    "--no-opt" => options.push(ApicOption::init(OptionKind::NoOpt, String::new())),

                    _ => {
                        options.insert(0, ApicOption::init(OptionKind::Bad, String::from("apic error: invalid option found. Use `apic help` to see how to use the apica compiler")));
                        return options;
                    }
                }
            } else {
                if let Some(kind) = &awaiting {
                    options.push(ApicOption::init(*kind, arg.clone()));
                    awaiting = None;
                } else {
                    options.insert(0, ApicOption::init(OptionKind::Bad, String::from("apic error: a value can only be placed after an option that requires one. Use `apic help` to see how to use the apica compiler`")));
                    return options;
                }
            }
        }
    }

    options
}

fn handle_command(options: &mut Vec<ApicOption>, args: &Vec<String>) -> bool {
    match args[1].as_str() {
        "help" => options.push(ApicOption::init(OptionKind::Help, String::new())),
        "init" => options.push(ApicOption::init(OptionKind::Init, String::new())),
        "build" => options.push(ApicOption::init(OptionKind::Build, String::new())),
        "version" => options.push(ApicOption::init(OptionKind::Version, String::new())),

        _ => {
            options.push(ApicOption::init(OptionKind::Bad, String::from("apic error : incorrect command found. Use `apic help` to see the availabled commands")));
            return false;
        }
    }

    true
}
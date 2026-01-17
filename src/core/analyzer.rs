use apica_common::bytecodes::ApicaEntrypointBytecode;
use crate::nodes::compound::NodeCompound;
use crate::nodes::global_scope::NodeGlobalScope;
use crate::nodes::node::Node;
use crate::utils::diagnostic::{Diagnostic, DiagnosticKind};
use crate::utils::diagnostic_bag::DiagnosticBag;

pub struct Analyzer<'a> {
    diag_bag: &'a mut DiagnosticBag,
}

impl<'a> Analyzer<'a> {
    pub fn init(diag_bag: &'a mut DiagnosticBag) -> Analyzer<'a> {
        Analyzer { diag_bag }
    }

    pub fn analyze(&mut self, root: &mut NodeCompound) {
        if !self.check_entrypoint_specs(root) {
            return;
        }

        self.extract_specs(root);
        self.extract_globals(root);
    }

    fn check_entrypoint_specs(&mut self, root: &mut NodeCompound) -> bool {
        let mut entry_init: u8 = 0;
        let mut entry_update: u8 = 0;
        let mut entry_quit: u8 = 0;
        let mut specs: u8 = 0;
        
        let mut success: bool = true;

        for node in root.get_nodes() {
            if let Node::Entrypoint(entry) = node {
                match entry.get_entry() {
                    ApicaEntrypointBytecode::Init => entry_init = if entry_init == 0 { 1 } else { 2 },
                    ApicaEntrypointBytecode::Update => entry_update = if entry_update == 0 { 1 } else { 2 },
                    ApicaEntrypointBytecode::Quit => entry_quit = if entry_quit == 0 { 1 } else { 2 },
                }
            } else if let Node::DataSpecs(_) = node {
                specs = if specs == 0 { 1 } else { 2 };
            }
        }

        if entry_init != 1 {
            self.diag_bag.add(Diagnostic::init_message(
                DiagnosticKind::Error,
                String::from("AnalyzerError: A correct main apica source file should contain a unique `init` entrypoint"),
            ));
            success = false;
        }

        if entry_update != 1 {
            self.diag_bag.add(Diagnostic::init_message(
                DiagnosticKind::Error,
                String::from("AnalyzerError: A correct main apica source file should contain a unique `update` entrypoint"),
            ));
            success = false;
        }

        if entry_quit != 1 {
            self.diag_bag.add(Diagnostic::init_message(
                DiagnosticKind::Error,
                String::from("AnalyzerError: A correct main apica source file should contain a unique `quit` entrypoint"),
            ));
            success = false;
        }
        
        if specs != 1 {
            self.diag_bag.add(Diagnostic::init_message(
                DiagnosticKind::Error,
                String::from("AnalyzerError: A correct main apica source file should contain a unique `specs` declaration")
            ));
            success = false;
        }

        success
    }

    fn extract_globals(&mut self, root: &mut NodeCompound) {
        let mut extracted = vec![];
        let nodes = root.get_mut_nodes();
        let mut kept = vec![];

        for node in nodes.drain(..) {
            if matches!(node, Node::Entrypoint(_)) || matches!(node, Node::DataSpecs(_)) || matches!(node, Node::EndOfFile(_)) {
                kept.push(node);
            } else {
                extracted.push(node);
            }
        }

        *nodes = kept;
        if !extracted.is_empty() {
            for node in root.get_mut_nodes() {
                if let Node::Entrypoint(entry) = node {
                    if *entry.get_entry() == ApicaEntrypointBytecode::Init {
                        if let Node::Compound(entry_compound) = entry.get_mut_body() {
                            let init_nodes = entry_compound.get_mut_nodes();

                            init_nodes.insert(0, Node::GlobalScope(Box::new(
                                NodeGlobalScope::init(
                                    extracted[0].get_position().clone(),
                                    Node::Compound(NodeCompound::init(
                                        extracted[0].get_position().clone(),
                                        extracted,
                                    )),
                                ),
                            )));

                            break;
                        }
                    }
                }
            }
        }

    }

    fn extract_specs(&mut self, root: &mut NodeCompound) {
        let nodes = root.get_mut_nodes();
        let mut specs = None;

        let mut i = 0;
        while i < nodes.len() {
            if matches!(nodes[i], Node::Entrypoint(_) | Node::EndOfFile(_)) {
                i += 1;
            } else {
                specs = Some(nodes.remove(i));
            }
        }

        if let Some(spec) = specs {
            nodes.insert(0, spec);
        }
    }
}
#include "core/emitter.hpp"

namespace core {
    Emitter::Emitter(utils::DiagnosticBag *diag_bag, const apic_string &outpath)
        : outpath(outpath) {
        this->diag_bag = diag_bag;
        this->output_file = fopen(outpath.c_str(), "wb");

        if (!this->output_file) {
            this->diag_bag->Add(new utils::Diagnostic(
                utils::DiagnosticKind::DIAG_Error,
                "EmitterError: Failed to open or create the output file"
            ));
        }
    }

    Emitter::~Emitter() {
        if (this->output_file)
            fclose(this->output_file);
    }

    void Emitter::Emit(nodes::NodeCompound *root) {
        if (!this->output_file)
            return;
        
        for (nodes::Node *node : root->GetNodes())
            this->EmitNode(node, EmitterModifier::EM_None);

        utils::write_u64(this->output_file, ApicaBytecode::AB_EndOfFile);
        if (!this->diag_bag->HasAnyError()) {
            this->diag_bag->Add(new utils::Diagnostic(
                utils::DiagnosticKind::DIAG_Success,
                "ApicaCompiler: The source code has been emitted successfully"
            ));
        } else
            remove(this->outpath.c_str());
    }

    void Emitter::EmitNode(nodes::Node *node, EmitterModifier mode) {
        switch (node->GetKind()) {
            case nodes::NodeKind::ND_Compound:
                return this->EmitCompound((nodes::NodeCompound*)node);
            
            case nodes::NodeKind::ND_Parameter:
                return this->EmitParameter((nodes::NodeParameter*)node);
            
            case nodes::NodeKind::ND_FuncCall:
                return this->EmitFuncCall((nodes::NodeFuncCall*)node, mode);
            
            case nodes::NodeKind::ND_Literal:
                return this->EmitLiteral((nodes::NodeLiteral*)node);
            
            case nodes::NodeKind::ND_Entrypoint:
                return this->EmitEntrypoint((nodes::NodeEntrypoint*)node);
            
            case nodes::NodeKind::ND_PackageCall:
                return this->EmitPackageCall((nodes::NodePackageCall*)node);

            case nodes::NodeKind::ND_Bad: break;
            default: {
                this->diag_bag->Add(new utils::Diagnostic(
                    utils::DiagnosticKind::DIAG_Error,
                    "EmitterError: Found an unsupported node type (future feature?)"
                ));
            } break;
        }
    }

    void Emitter::EmitCompound(nodes::NodeCompound *compound) {
        for (nodes::Node *node : compound->GetNodes())
            this->EmitNode(node, EmitterModifier::EM_None);
        
        utils::write_u64(this->output_file, ApicaBytecode::AB_EndOfBlock);
    }

    void Emitter::EmitEntrypoint(nodes::NodeEntrypoint *entrypoint) {
        utils::write_u64(this->output_file, ApicaBytecode::AB_EntryPoint);
        utils::write_u64(this->output_file, entrypoint->GetEntrypoint());
        this->EmitNode(entrypoint->GetBody(), EmitterModifier::EM_None);
    }

    void Emitter::EmitPackageCall(nodes::NodePackageCall *package) {
        if (package->GetName() == "APICA")
            this->EmitNode(package->GetNext(), EmitterModifier::EM_Builtin);
        else {
            this->diag_bag->Add(new utils::Diagnostic(
                utils::DiagnosticKind::DIAG_Error,
                "EmitterModifier: Other packages than `APICA` (builtin) are not yet supported"
            ));
        }
    }

    void Emitter::EmitFuncCall(nodes::NodeFuncCall *func_call, EmitterModifier mode) {
        if (mode | EmitterModifier::EM_Builtin) {
            utils::write_u64(this->output_file, ApicaBytecode::AB_BuiltinFuncCall);
            ApicaBuiltinFuncCallBytecode bytecode = utils::get_builtin_func_bytecode(func_call->GetName(), this->diag_bag, func_call->GetPosition());

            utils::write_u64(this->output_file, bytecode);
            for (nodes::NodeParameter *param : func_call->GetParameters())
                this->EmitParameter(param);
            
            utils::write_u64(this->output_file, ApicaBytecode::AB_EndOfBlock);
        } else {
            this->diag_bag->Add(new utils::Diagnostic(
                utils::DiagnosticKind::DIAG_Error,
                "EmitterError: User-defined function calls are not yet supported",
                new utils::Position(func_call->GetPosition())
            ));
        }
    }

    void Emitter::EmitParameter(nodes::NodeParameter *param) {
        // TODO : use explcite name
        this->EmitNode(param->GetExpression(), EmitterModifier::EM_None);
    }

    void Emitter::EmitLiteral(nodes::NodeLiteral *literal) {
        utils::write_u64(this->output_file, ApicaBytecode::AB_Literal);
        switch (literal->GetValue()->GetKind()) {
            case values::ValueKind::VK_Null:
                utils::write_u64(this->output_file, ApicaTypeBytecode::ATB_Null);
                break;
            
            case values::ValueKind::VK_String: {
                utils::write_u64(this->output_file, ApicaTypeBytecode::ATB_String);
                apic_string *string = ((values::ValueString*)literal->GetValue())->GetValue();
                if (!string) {
                    this->diag_bag->Add(new utils::Diagnostic(
                        utils::DiagnosticKind::DIAG_Error,
                        "EmitterError: Strings with null value are not yet supported"
                    ));
                    return;
                }

                for (apic_char c : *string)
                    utils::write_i8(this->output_file, c);
                utils::write_i8(this->output_file, '\0');
            } break;

            default: {
                this->diag_bag->Add(new utils::Diagnostic(
                    utils::DiagnosticKind::DIAG_Error,
                    "EmitterError: Unsupported literal type found (future feature?)"
                ));
            }
        }
    }
}
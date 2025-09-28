#pragma once

#include "core/analyser.hpp"

namespace core {
    enum EmitterModifier : apic_u8 {
        EM_None =       0b00000000,
        EM_Builtin =    0b00000001
    };
    
    class Emitter {
    public:
        Emitter(utils::DiagnosticBag *diag_bag, const apic_string &outpath);
        ~Emitter();

        void Emit(nodes::NodeCompound *root);
    private:
        utils::DiagnosticBag *diag_bag;
        FILE *output_file;
        apic_string outpath;

        void EmitNode(nodes::Node *node, EmitterModifier mode);
        void EmitCompound(nodes::NodeCompound *compound);
        void EmitEntrypoint(nodes::NodeEntrypoint *entrypoint);
        void EmitPackageCall(nodes::NodePackageCall *package);
        void EmitFuncCall(nodes::NodeFuncCall *func_call, EmitterModifier mode);
        void EmitParameter(nodes::NodeParameter *param);
        void EmitLiteral(nodes::NodeLiteral *literal);
    };
}
#pragma once

#include "utils/bytecode.hpp"

namespace nodes {
    enum NodeKind : apic_u8 {
        ND_Bad = 0, ND_Compound, ND_EndOfFile,

        ND_Binary, ND_Unary, ND_Question,

        ND_Literal, ND_Parenthesized, ND_Ternary,

        ND_Entrypoint, ND_PackageCall,

        ND_Parameter, ND_FuncCall,

        ND_Break, ND_Continue, ND_Return,

        ND_VarConstDecl, ND_VarConstCall
    };
    
    class Node {
    public:
        virtual ~Node();
        virtual const NodeKind GetKind() const = 0;
        virtual void Show(apic_string indent) = 0;

    protected:
        Node();
    };
}
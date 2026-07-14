#pragma once

#include <cstdint>

namespace nodes {
    enum NodeKind : uint8_t {
        Bad, EndOfFile, Compound, NoOp,

        Entrypoint, PackageCall, GlobalScope,

        UnaryOperation, BinaryOperation, TernaryOperation,
        Literal,

        Break, Continue, Return,

        VarConstCall, VarConstDeclaration,
        FuncCall, Parameter,

        IfElse, While, For,

        Typeof,

        DataSpecs,
        DataString, DataU32, DataBool
    };
}
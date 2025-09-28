#pragma once

#include "common/types.hpp"

namespace values {
    enum ValueKind : unsigned char {
        VK_Null = 0, VK_Any,

        VK_I8, VK_I16, VK_I32, VK_I64, VK_U8, VK_U16, VK_U32, VK_U64,
        VK_F32, VK_F64,
        VK_Bool, VK_Char, VK_String
    };

    class Value {
    public:
        virtual ~Value();
        virtual const ValueKind GetKind() const = 0;
        virtual void Show(apic_char end) = 0;
        virtual const bool IsNull() const = 0;

    protected:
        Value();
    };
}
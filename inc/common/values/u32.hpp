#pragma once

#include "common/values/null.hpp"

namespace values {
    class ValueU32 : public Value {
    public:
        ValueU32(apic_u32 val);
        ValueU32();
        ~ValueU32();

        const ValueKind GetKind() const override;
        void Show(apic_char end) override;
        const bool IsNull() const override;
        const apic_u32 *GetValue() const;
    private:
        apic_u32 *value;
    };
}
#pragma once

#include "common/values/u32.hpp"

namespace values {
    class ValueString : public Value {
    public:
        ValueString(const apic_string &val);
        ValueString();
        ~ValueString();

        const ValueKind GetKind() const override;
        void Show(apic_char end) override;
        const bool IsNull() const override;
        apic_string *GetValue() const;
    private:
        apic_string *value;
    };
}
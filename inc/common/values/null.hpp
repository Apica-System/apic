#pragma once

#include "common/values/value.hpp"

namespace values {
    class ValueNull : public Value {
    public:
        ValueNull();
        ~ValueNull();

        const ValueKind GetKind() const override;
        void Show(apic_char end) override;
        const bool IsNull() const override;
    };
}
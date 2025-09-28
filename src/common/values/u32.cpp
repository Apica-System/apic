#include "common/values/u32.hpp"

namespace values {
    ValueU32::ValueU32(apic_u32 val) {
        this->value = new apic_u32(val);
    }

    ValueU32::ValueU32() {
        this->value = nullptr;
    }

    ValueU32::~ValueU32() {
        if (this->value) delete this->value;
    }

    const ValueKind ValueU32::GetKind() const {
        return ValueKind::VK_U32;
    }

    void ValueU32::Show(apic_char end) {
        if (this->value)
            printf("u32(%u)%c", *this->value, end);
        else
            printf("u32(null)%c", end);
    }

    const bool ValueU32::IsNull() const {
        return this->value == nullptr;
    }

    const apic_u32 *ValueU32::GetValue() const {
        return this->value;
    }
}
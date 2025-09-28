#include "common/values/string.hpp"

namespace values {
    ValueString::ValueString(const apic_string &val) {
        this->value = new apic_string(val);
    }

    ValueString::ValueString() {
        this->value = nullptr;
    }

    ValueString::~ValueString() {
        if (this->value) delete this->value;
    }

    const ValueKind ValueString::GetKind() const {
        return ValueKind::VK_String;
    }

    void ValueString::Show(apic_char end) {
        if (this->value)
            printf("string(%s)%c", this->value->c_str(), end);
        else
            printf("string(null)%c", end);
    }

    const bool ValueString::IsNull() const {
        return this->value == nullptr;
    }

    apic_string *ValueString::GetValue() const {
        return this->value;
    }
}
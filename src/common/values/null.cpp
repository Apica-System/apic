#include "common/values/null.hpp"

namespace values {
    ValueNull::ValueNull() {

    }

    ValueNull::~ValueNull() {

    }

    const ValueKind ValueNull::GetKind() const {
        return ValueKind::VK_Null;
    }

    void ValueNull::Show(apic_char end) {
        printf("null()%c", end);
    }
    
    const bool ValueNull::IsNull() const {
        return true;
    }
}
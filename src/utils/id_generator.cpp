#include "utils/id_generator.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/errors.hpp"

using namespace utils;

IdGenerator::IdGenerator()
    : max_id(0), is_global(false) {

}

IdGenerator &IdGenerator::getInstance() {
    static IdGenerator instance;
    return instance;
}

uint64_t IdGenerator::getIdCount() const {
    return this->max_id + 1;
}

void IdGenerator::pushContext() {
    if (this->ids.empty()) {
        this->ids.push_back({});
    } else {
        this->ids.push_back({});
    }
}

void IdGenerator::popContext() {
    if (!this->ids.empty())
        this->ids.pop_back();
}

void IdGenerator::setNewId(const std::string &name, const Position &pos) {
    std::unordered_map<std::string, uint64_t> &current_context = is_global ? this->ids.front() : this->ids.back();
    auto already_exists = current_context.find(name);
    if (already_exists != current_context.end()) {
        DiagnosticBag::getInstance().addDiagnostic(Diagnostic(
            DiagnosticKind::Error,
            std::string(ANL_ERROR_NOT_UNIQUE_VC),
            pos
        ));
    }

    uint64_t new_id = this->max_id++;
    current_context[name] = new_id;
}

std::optional<uint64_t> IdGenerator::getId(const std::string &name) const {
    int64_t scope = is_global ? 0 : this->ids.size() - 1;
    while (scope >= 0) {
        auto associated_id = this->ids[scope].find(name);
        if (associated_id != this->ids[scope--].end())
            return associated_id->second;
    }

    return std::nullopt;
}

void IdGenerator::setGlobal(bool indicator) {
    this->is_global = indicator;
}
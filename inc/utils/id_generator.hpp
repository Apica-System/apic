#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <optional>
#include "utils/position.hpp"

namespace utils {
    enum IdGeneratorModifier : uint8_t {
        IGM_None =      0b00000000,
        IGM_Global =    0b00000001,
        IGM_BUILTIN =   0b00000010
    };

    class IdGenerator {
    public:
        static IdGenerator &getInstance();

        uint64_t getIdCount() const;

        void pushContext();
        void popContext();
        uint64_t setNewId(const std::string &name, const Position &pos);
        std::optional<uint64_t> getId(const std::string &name) const;

        uint8_t getModifier() const;
        void addModifier(IdGeneratorModifier modifier);
        void removeModifier(IdGeneratorModifier modifier);
    private:
        std::vector<std::unordered_map<std::string, uint64_t>> ids;
        uint64_t max_id;
        uint8_t modifier;
        bool is_global;

        IdGenerator();

        IdGenerator(IdGenerator &other) = delete;
        void operator=(const IdGenerator &) = delete;
    };
}
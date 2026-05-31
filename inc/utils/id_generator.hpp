#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <optional>
#include "utils/position.hpp"

namespace utils {
    class IdGenerator {
    public:
        static IdGenerator &getInstance();

        uint64_t getIdCount() const;

        void pushContext();
        void popContext();
        void setNewId(const std::string &name, const Position &pos);
        std::optional<uint64_t> getId(const std::string &name) const;

        void setGlobal(bool indicator);
    private:
        std::vector<std::unordered_map<std::string, uint64_t>> ids;
        uint64_t max_id;
        bool is_global;

        IdGenerator();

        IdGenerator(IdGenerator &other) = delete;
        void operator=(const IdGenerator &) = delete;
    };
}
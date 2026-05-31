#pragma once

#include "bytecodes.hpp"
#include <string>
#include <cstdio>

namespace core {
    enum EmitterModifier : uint8_t {
        EM_None =           0b00000000,
        EM_Builtin =        0b00000001,
        EM_UnpackCompound = 0b00000010
    };

    class Emitter final {
    public:
        Emitter(const std::string &output_path);
        ~Emitter();

        void processResult();

        uint8_t getModifier() const;
        void addModifier(EmitterModifier modifier);
        void removeModifier(EmitterModifier modifier);

        void writeU8(uint8_t data);
        void writeU16(uint16_t data);
        void writeU32(uint32_t data);
        void writeU64(uint64_t data);
        void writeString(const std::string &data);
    private:
        std::string output_path;
        FILE *output_file;
        uint8_t modifier;
        bool write_error_happened;
    };
}
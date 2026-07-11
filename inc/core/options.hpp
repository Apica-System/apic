#pragma once

#include <cstdint>
#include <string>
#include <optional>
#include <unordered_map>

namespace core {
    enum ApicOptionKind : uint8_t {
        Bad,

        Init, Build, Help, Version, Lsp,

        SourceFile, OutputFile,

        ShowNodes, 
        
        NoOptimisation
    };

    class ApicOptions final {
    public:
        static ApicOptions getOptions(int argc, char **argv);

        std::optional<std::string> getOption(ApicOptionKind kind) const;
    private:
        ApicOptions();

        std::unordered_map<ApicOptionKind, std::string> options;

        bool handleCommand(char *arg);
        void addCommandOrError(ApicOptionKind kind, const std::string &value);
        bool addOption(ApicOptionKind kind, const std::string &value, const std::string &cmd);
    };
}
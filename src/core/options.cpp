#include "core/options.hpp"
#include "utils/errors.hpp"
#include <cstring>

using namespace core;

struct AwaitedOptionKind {
    std::optional<ApicOptionKind> kind;
    std::string cmd;
};

ApicOptions::ApicOptions() {

}

ApicOptions ApicOptions::getOptions(int argc, char **argv) {
    ApicOptions apic_options;
    if (argc < 2) {
        apic_options.addCommandOrError(ApicOptionKind::Bad, std::string(utils::OPT_ERROR_NO_ARGUMENT));
        return apic_options;
    }

    if (!apic_options.handleCommand(argv[1])) {
        apic_options.addCommandOrError(ApicOptionKind::Bad, std::string(utils::OPT_ERROR_UNKNOWN_COMMAND));
        return apic_options;
    }

    AwaitedOptionKind awaited_kind;
    awaited_kind.cmd = "";
    awaited_kind.kind = std::nullopt;

    for (int i = 2; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (awaited_kind.kind) {
                std::string error_message(utils::OPT_ERROR_OPTION_EXPECTED_VALUE);
                error_message += awaited_kind.cmd;
                error_message += '`';

                apic_options.addCommandOrError(ApicOptionKind::Bad, error_message);
                return apic_options;
            }

            if (!strcmp(argv[i], "-i")) {
                awaited_kind.kind = ApicOptionKind::SourceFile;
                awaited_kind.cmd = "-i";
            } else if (!strcmp(argv[i], "-o")) {
                awaited_kind.kind = ApicOptionKind::OutputFile;
                awaited_kind.cmd = "-o";
            } else if (!strcmp(argv[i], "--nodes")) {
                if (!apic_options.addOption(ApicOptionKind::ShowNodes, "", "--nodes"))
                    return apic_options;
            } else if (!strcmp(argv[i], "--werror")) {
                if (!apic_options.addOption(ApicOptionKind::WarningsAsErrors, "", "--werror"))
                    return apic_options;
            } else {
                std::string error_message(utils::OPT_ERROR_INVALID_OPTION);
                error_message += argv[i];
                error_message += '`';

                apic_options.addCommandOrError(ApicOptionKind::Bad, error_message);
                return apic_options;
            }
        } else {
            if (awaited_kind.kind) {
                if (!apic_options.addOption(awaited_kind.kind.value(), argv[i], awaited_kind.cmd))
                    return apic_options;
                awaited_kind.kind = std::nullopt;
            } else {
                std::string error_message(utils::OPT_ERROR_VALUE_UNEXPECTED);
                error_message += argv[i];
                error_message += '`';

                apic_options.addCommandOrError(ApicOptionKind::Bad, error_message);
                return apic_options;
            }
        }
    }

    if (awaited_kind.kind) {
        std::string error_message(utils::OPT_ERROR_AWAITED_VALUE);
        error_message += awaited_kind.cmd;
        error_message += '`';

        apic_options.addCommandOrError(ApicOptionKind::Bad, error_message);
    }

    return apic_options;
}

std::optional<std::string> ApicOptions::getOption(ApicOptionKind kind) const {
    auto option = this->options.find(kind);
    if (option == this->options.end())
        return std::nullopt;
    
    return option->second;
}

bool ApicOptions::handleCommand(char *arg) {
    if (!strcmp(arg, "help")) this->addCommandOrError(ApicOptionKind::Help, "");
    else if (!strcmp(arg, "init")) this->addCommandOrError(ApicOptionKind::Init, "");
    else if (!strcmp(arg, "build")) this->addCommandOrError(ApicOptionKind::Build, "");
    else if (!strcmp(arg, "version")) this->addCommandOrError(ApicOptionKind::Version, "");
    else if (!strcmp(arg, "lsp")) this->addCommandOrError(ApicOptionKind::Lsp, "");
    else return false;

    return true;
}

void ApicOptions::addCommandOrError(ApicOptionKind kind, const std::string &value) {
    this->options[kind] = value;
}

bool ApicOptions::addOption(ApicOptionKind kind, const std::string &value, const std::string &cmd) {
    auto option = this->options.find(kind);
    if (option != this->options.end()) {
        std::string error_message(utils::OPT_ERROR_ALREADY_DEFINED);
        error_message += cmd;
        error_message += '`';

        this->addCommandOrError(ApicOptionKind::Bad, error_message);
        return false;
    }

    this->options[kind] = value;
    return true;
}
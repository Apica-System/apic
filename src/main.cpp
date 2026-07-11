#include "utils/version.hpp"
#include "utils/builtins.hpp"
#include "core/options.hpp"
#include "utils/apic_cmd.hpp"
#include "lsp/server.hpp"
#include <string>

using namespace utils;
using namespace core;

int main(int argc, char **argv) {
    // Convert console arguments -> apic options
    ApicOptions options = ApicOptions::getOptions(argc, argv);

    // Error
    std::optional<std::string> option = options.getOption(ApicOptionKind::Bad);
    if (option) {
        std::cout << "\x1b[31m" << option.value() << "\x1b[0m\n";
        return EXIT_FAILURE;
    }

    // Help
    option = options.getOption(ApicOptionKind::Help);
    if (option) {
        std::cout << APIC_HELP;
        return EXIT_SUCCESS;
    }

    // Version
    option = options.getOption(ApicOptionKind::Version);
    if (option) {
        std::cout << "apic " << APIC_VERSION << '\n';
        return EXIT_SUCCESS;
    }

    // Init
    option = options.getOption(ApicOptionKind::Init);
    if (option) {
        return init_apic_workdir() ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    // Build
    option = options.getOption(ApicOptionKind::Build);
    if (option) {
        return build_bytecode_file(options) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    // LSP
    option = options.getOption(ApicOptionKind::Lsp);
    if (option) {
        lsp::lsp_entrypoints();
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
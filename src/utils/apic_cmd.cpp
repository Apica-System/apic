#include "utils/apic_cmd.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include "utils/errors.hpp"
#include "core/parser.hpp"
#include "core/analyzer.hpp"
#include "core/optimizer.hpp"
#include "core/emitter.hpp"
#include "utils/diagnostic_bag.hpp"

namespace utils {
    std::string read_file(const std::string& path) {
        std::ifstream file(path, std::ios::binary);

        if (!file) {
            return {};
        }

        file.seekg(0, std::ios::end);
        size_t size = file.tellg();

        std::string buffer(size, '\0');

        file.seekg(0);
        file.read(buffer.data(), size);
        if (buffer.starts_with("\xEF\xBB\xBF"))
            buffer.erase(0, 3);

        return buffer;
    }

    bool init_apic_workdir() {
        if (!std::filesystem::exists("assets") && !std::filesystem::create_directory("assets")) {
            std::cout << "\x1b[31m" << INT_ERROR_ASSETS_DIR << "\x1b[0m\n";
            return false;
        }

        if (!std::filesystem::exists("src") && !std::filesystem::create_directory("src")) {
            std::cout << "\x1b[31m" << INT_ERROR_SRC_DIR << "\x1b[0m\n";
            return false;
        }

        std::filesystem::path main_path = "src/main.apc";
        if (!std::filesystem::exists(main_path)) {
            std::ofstream main_file(main_path);
            if (!main_file) {
                std::cout << "\x1b[31m" << INT_ERROR_MAIN_FILE << "\x1b[0m\n";
                return false;
            }

            main_file << R"(// Basic apica main file - automatically generated

specs {
    id: "Application's id",
    title: "Application's title",
    version: "Application's version"
}

entry init {}

entry update {}

entry quit {}
)";

            if (!main_file) {
                std::cout << "\x1b[31m" << INT_ERROR_WRITE_MAIN_FILE << "\x1b[0m\n";
                return false;
            }
        }

        std::cout << "\x1b[32mapic : workdir initialized successfully!\x1b[0m\n";
        return true;
    }

    bool build_bytecode_file(const core::ApicOptions &apic_options) {
        std::optional<std::string> input_filepath = apic_options.getOption(core::ApicOptionKind::SourceFile);
        if (!input_filepath) {
            std::cout << "\x1b[31m" << BLD_ERROR_NO_SOURCE << "\x1b[0m\n";
            return false;
        }

        std::string source_content = read_file(input_filepath.value());
        if (source_content.empty()) {
            std::cout << "\x1b[31m" << BLD_ERROR_CANT_READ_SOURCE << input_filepath.value() << "`\x1b[0m\n";
            return false;
        }

        utils::SourceText source(source_content);
        nodes::NodeCompound *root;
        {
            core::Parser parser(source);
            root = parser.parse();
        }

        if (!utils::DiagnosticBag::getInstance().hasAnyError()) {
            std::optional<std::string> no_opt = apic_options.getOption(core::ApicOptionKind::NoOptimisation);
            if (!no_opt) {
                core::Optimizer optimizer;
                root->optimize(optimizer);
            }
        }

        if (!utils::DiagnosticBag::getInstance().hasAnyError()) {
            core::Analyzer analyzer;
            analyzer.analyze(root);
        }

        if (!utils::DiagnosticBag::getInstance().hasAnyError()) {
            std::optional<std::string> show_nodes = apic_options.getOption(core::ApicOptionKind::ShowNodes);
            if (show_nodes) {
                std::string indent;
                root->show(indent, '\n');
            }
        }

        {
            std::optional<std::string> output_path = apic_options.getOption(core::ApicOptionKind::OutputFile);
            core::Emitter emitter(output_path.value_or("out.apb"));
            root->emit(emitter);
            emitter.processResult();
        }
        
        delete root;
        utils::DiagnosticBag::getInstance().showAll(source);
        return true;
    }
}
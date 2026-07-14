#include "lsp/server.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/position.hpp"
#include "core/parser.hpp"
#include "core/analyzer.hpp"
#include "core/emitter.hpp"
#include "core/optimizer.hpp"

#include "jsonrpcxx/server.hpp"

#include <iostream>
#include <string>

#ifdef _WIN32
    #include <fcntl.h>
    #include <io.h>
#endif

using namespace lsp;

void lsp_analyze_code(const std::string &text, const std::string &uri) {
    utils::DiagnosticBag::getInstance().clear();
    utils::SourceText source(text);

    core::Parser parser(source);
    nodes::NodeCompound *root = parser.parse();

    core::Optimizer optimizer;
    root->optimize(optimizer);

    core::Analyzer analyzer;
    analyzer.analyze(root, false);

    core::Emitter emitter;
    root->emitRoot(emitter);
    utils::DiagnosticBag::getInstance().lspFlush(source, uri);
    delete root;
}

void lsp_mainloop(jsonrpccxx::Dispatcher &dispatcher, bool &should_exit) {
    while (std::cin.good() && !should_exit) {
        std::string header;
        int content_length = 0;

        while (std::getline(std::cin, header) && header != "\r") {
            if (header.rfind("Content-Length: ", 0) == 0) {
                content_length = std::stoi(header.substr(16));
            }
        }

        if (content_length <= 0)
            continue;

        std::string json_payload(content_length, '\0');
        std::cin.read(&json_payload[0], content_length);
        std::streamsize actually_read = std::cin.gcount();
        if (actually_read != content_length) {
            std::cerr << "[apic-lsp] incomplete reading: expected " << content_length
                       << " received " << actually_read << std::endl;
            continue;
        }

        try {
            auto request_json = nlohmann::json::parse(json_payload);
            std::string method_name = request_json["method"];
            nlohmann::json params = request_json.contains("params") ? request_json["params"] : nlohmann::json::object();
            nlohmann::json wrapped_params = nlohmann::json::array({params});

            if (request_json.contains("id")) {
                nlohmann::json result = dispatcher.InvokeMethod(method_name, wrapped_params);

                nlohmann::json response_json;
                response_json["jsonrpc"] = "2.0";
                response_json["id"] = request_json["id"];
                response_json["result"] = result;

                std::string response_str = response_json.dump();
                std::cout << "Content-Length: " << response_str.size() << "\r\n\r\n" << response_str << std::flush;
            } else {
                dispatcher.InvokeNotification(method_name, wrapped_params);
            }
        } catch (const std::exception& e) {
            std::cerr << "[apic-lsp] exception: " << e.what() << std::endl;
        }
    }
}

void lsp_dispatch(jsonrpccxx::Dispatcher &dispatcher, bool &should_exit) {
    dispatcher.Add("initialize", jsonrpccxx::MethodHandle([](const nlohmann::json&) -> nlohmann::json {
        nlohmann::json response;
        response["capabilities"] = {
            {"textDocumentSync", {
                { "openClose", true },
                { "change", 1 },
                { "save", {{"includeText", true}} }
            }}
        };

        return response;
    }));

    dispatcher.Add("initialized", jsonrpccxx::NotificationHandle([](const nlohmann::json&) {
        // Do nothing
    }));

    dispatcher.Add("textDocument/didOpen", jsonrpccxx::NotificationHandle([](const nlohmann::json& params) {
        const nlohmann::json &p = params[0];
        std::string uri = p["textDocument"]["uri"];
        std::string initial_code = p["textDocument"]["text"];
        lsp_analyze_code(initial_code, uri);
    }));

    dispatcher.Add("textDocument/didChange", jsonrpccxx::NotificationHandle([](const nlohmann::json&) {
        // Do nothing
    }));

    dispatcher.Add("textDocument/didSave", jsonrpccxx::NotificationHandle([](const nlohmann::json &params) {
        const nlohmann::json &p = params[0];
        std::string uri = p["textDocument"]["uri"];
        std::string saved_code = p.value("text", "");
        if (!saved_code.empty())
            lsp_analyze_code(saved_code, uri);
    }));

    dispatcher.Add("exit", jsonrpccxx::NotificationHandle([&should_exit](const nlohmann::json&) {
        should_exit = true;
    }));
}

void lsp::lsp_entrypoints() {
#ifdef _WIN32
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    jsonrpccxx::Dispatcher dispatcher;
    bool should_exit = false;

    lsp_dispatch(dispatcher, should_exit);
    lsp_mainloop(dispatcher, should_exit);
}
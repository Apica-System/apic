#include "utils/diagnostic_bag.hpp"

#include <iostream>

using namespace utils;

DiagnosticBag::DiagnosticBag()
    : errors(0), warnings(0) {

}

DiagnosticBag &DiagnosticBag::getInstance() {
    static DiagnosticBag instance;
    return instance;
}

void DiagnosticBag::addDiagnostic(const Diagnostic &diagnostic) {
    if (diagnostic.getKind() == DiagnosticKind::Error)
        this->errors++;
    else if (diagnostic.getKind() == DiagnosticKind::Warning)
        this->warnings++;
    
    this->diagnostics.push_back(diagnostic);
}

void DiagnosticBag::showAll(const SourceText &source) const {
    uint64_t i = 0;
    size_t length = this->diagnostics.size();

    for (const Diagnostic &diagnostic : this->diagnostics) {
        diagnostic.show(source);
        if (++i != length)
            std::cout << "\n\n";
    }

    if (length)
        std::cout << "\n\x1b[4m" << this->errors << " errors, " << this->warnings << " warnings\x1b[0m\n";
}

void DiagnosticBag::clear() {
    this->diagnostics.clear();
}

void DiagnosticBag::lspFlush(const SourceText &source, const std::string &uri) {
    nlohmann::json diagnostic_list = nlohmann::json::array();
    for (Diagnostic &diag : this->diagnostics) {
        diag.lspFlush(source, diagnostic_list);
    }

    nlohmann::json notification_json;
    notification_json["jsonrpc"] = "2.0";
    notification_json["method"] = "textDocument/publishDiagnostics";
    notification_json["params"] = {
        {"uri", uri},
        {"diagnostics", diagnostic_list}
    };

    std::string notification_str = notification_json.dump();
    std::cout << "Content-Length: " << notification_str.size() << "\r\n\r\n" 
                << notification_str << std::flush;
}

bool DiagnosticBag::hasAny() const {
    return !this->diagnostics.empty();
}

bool DiagnosticBag::hasAnyError() const {
    return this->errors > 0;
}

bool DiagnosticBag::hasAnyWarning() const {
    return this->warnings > 0;
}
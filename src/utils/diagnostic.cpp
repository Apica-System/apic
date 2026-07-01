#include "utils/diagnostic.hpp"
#include <iostream>

using namespace utils;

Diagnostic::Diagnostic(DiagnosticKind kind, const std::string &message)
    : kind(kind), message(message), position(std::nullopt) {

}

Diagnostic::Diagnostic(DiagnosticKind kind, const std::string &message, const Position &position)
    : kind(kind), message(message), position(position) {

}

Diagnostic::Diagnostic(common::values::ValueError *error, const Position &position)
    : kind(DiagnosticKind::Error), position(position) {
    this->message = error->getName().value_or("");
    if (error->getDetails())
        this->message += ": " + error->getDetails().value();
}

void Diagnostic::show(const SourceText &source) const {
    std::string color = Diagnostic::getColorByKind(this->kind);
    if (this->position)
        this->showWithPosition(color, source);
    else
        this->showWithoutPosition(color);
}

DiagnosticKind Diagnostic::getKind() const {
    return this->kind;
}

void Diagnostic::showWithoutPosition(const std::string &color) const {
    std::cout << color << this->message << ".\x1b[0m\n";
}

void Diagnostic::showWithPosition(const std::string &color, const SourceText &source) const {
    SourceInformations infos;
    source.getInfosForPosition(this->position.value(), infos);
    if (infos.line_start == infos.line_end) {
        std::optional<std::string> line_text = source.getLineText(infos.line_start);
        if (!line_text)
            return;
        
        std::cout << line_text.value() << '\n' 
            << color << std::string(infos.left_offset, ' ') << std::string(this->position.value().getUtf8Length(), '^') << '\n'
            << "[At line " << infos.line_start + 1 << ", column " << infos.left_offset + 1 << '-' << infos.left_offset + this->position.value().getUtf8Length() << "]: " << this->message << ".\x1b[0m\n";
    } else {
        for (uint64_t i = infos.line_start; i <= infos.line_end; i++) {
            std::optional<std::string> line_text = source.getLineText(i);
            std::optional<uint64_t> line_length = source.getLineLength(i);
            if (!line_text || !line_length)
                return;
            
            std::cout << line_text.value() << '\n';
            if (i == infos.line_start) {
                std::cout << color << std::string(infos.left_offset, ' ') << std::string(line_length.value() - infos.left_offset, '^') << "\x1b[0m\n";
            } else if (i == infos.line_end) {
                std::cout << color << std::string(line_length.value() - infos.right_offset, '^') << "\x1b[0m\n";
            } else {
                std::cout << color << std::string(line_length.value(), '^') << "\x1b[0m\n";
            }
        }

        std::cout << color << "[From line " << infos.line_start + 1 << " to line " << infos.line_end + 1 << "]: " << this->message << ".\x1b[0m\n";
    }
}

std::string Diagnostic::getColorByKind(DiagnosticKind kind) {
    switch (kind) {
        case DiagnosticKind::Success: return "\x1b[32m";
        case DiagnosticKind::Warning: return "\x1b[33m";
        case DiagnosticKind::Error: return "\x1b[31m";

        case DiagnosticKind::Information: default: return "\x1b[37m";
    }
}
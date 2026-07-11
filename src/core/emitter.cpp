#include "core/emitter.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/errors.hpp"
#include "bytecodes.hpp"

using namespace core;

Emitter::Emitter() 
    : output_file(nullptr), modifier(EmitterModifier::EM_None), write_error_happened(false), is_mock(true) {

}

Emitter::Emitter(const std::string &output_path)
    : output_path(output_path), modifier(EmitterModifier::EM_None), write_error_happened(false), is_mock(false) {
    this->output_file = fopen(output_path.c_str(), "wb");
    if (!this->output_file) {
        std::string error_message(utils::EMT_ERROR_OPEN_OUTPUT_FILE);
        error_message += output_path;
        error_message += '`';
        
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            error_message
        ));
    }
}

Emitter::~Emitter() {
    if (this->output_file) fclose(this->output_file);
}

void Emitter::processResult() {
    if (this->is_mock)
        return;

    if (utils::DiagnosticBag::getInstance().hasAnyError()) {
        fclose(this->output_file);
        this->output_file = nullptr;
        std::remove(this->output_path.c_str());
    } else {
        this->writeU64(common::bytecodes::ApicaBytecode::EndOfFile);
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Success,
            "apic : The source code has been emitted successfully"
        ));
    }
}

uint8_t Emitter::getModifier() const {
    return this->modifier;
}

void Emitter::addModifier(EmitterModifier modifier) {
    this->modifier |= modifier;
}

void Emitter::removeModifier(EmitterModifier modifier) {
    this->modifier &= ~modifier;
}

void Emitter::writeU8(uint8_t data) {
    if (this->is_mock)
        return;

    if (fputc(data, this->output_file) == EOF) {
        if (!this->write_error_happened) {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Error,
                std::string(utils::EMT_ERROR_WRITING_U8)
            ));
            this->write_error_happened = true;
        }
    }
}

void Emitter::writeU16(uint16_t data) {
    if (this->is_mock)
        return;

    if (fwrite(&data, sizeof(uint16_t), 1, this->output_file) != 1) {
        if (!this->write_error_happened) {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Error,
                std::string(utils::EMT_ERROR_WRITING_U16)
            ));
            this->write_error_happened = true;
        }
    }
}

void Emitter::writeU32(uint32_t data) {
    if (this->is_mock)
        return;

    if (fwrite(&data, sizeof(uint32_t), 1, this->output_file) != 1) {
        if (!this->write_error_happened) {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Error,
                std::string(utils::EMT_ERROR_WRITING_U32)
            ));
            this->write_error_happened = true;
        }
    }
}

void Emitter::writeU64(uint64_t data) {
    if (this->is_mock)
        return;

    if (fwrite(&data, sizeof(uint64_t), 1, this->output_file) != 1) {
        if (!this->write_error_happened) {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Error,
                std::string(utils::EMT_ERROR_WRITING_U64)
            ));
            this->write_error_happened = true;
        }
    }
}

void Emitter::writeString(const std::string &data) {
    if (this->is_mock)
        return;

    if (fwrite(data.c_str(), sizeof(char), data.length() + 1, this->output_file) != (data.length() + 1)) {
        if (!this->write_error_happened) {
            utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
                utils::DiagnosticKind::Error,
                std::string(utils::EMT_ERROR_WRITING_STR)
            ));
            this->write_error_happened = true;
        }
    }
}
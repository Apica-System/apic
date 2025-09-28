// VERSION: 0.1
#include "core/emitter.hpp"

int main(int argc, char **argv) {
    utils::DiagnosticBag *diag_bag = new utils::DiagnosticBag();
    
    FILE *input_file = fopen("./tests/apc/main.apc", "rb");
    if (!input_file) {
        printf("ApicaError: Unable to read source file at `./tests/apc/main.apc`\n");
        return 1;
    }

    apic_string raw_source;
    apic_char raw_c = fgetc(input_file);
    while (raw_c != -1) {
        raw_source.push_back(raw_c);
        raw_c = fgetc(input_file);
    }

    fclose(input_file);
    
    utils::SourceText *source = new utils::SourceText(raw_source);
    core::Parser *parser = new core::Parser(source, diag_bag);
    nodes::NodeCompound *root = parser->Parse();
    root->Show("");

    delete parser;
    core::Emitter *emitter = new core::Emitter(diag_bag, "main.apb");
    emitter->Emit(root);

    delete root;
    delete emitter;
    diag_bag->ShowAll(source);
    
    delete source;
    delete diag_bag;
    return 0;
}
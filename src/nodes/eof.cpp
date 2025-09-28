#include "nodes/eof.hpp"

namespace nodes {
    NodeEndOfFile::NodeEndOfFile() {

    }

    NodeEndOfFile::~NodeEndOfFile() {

    }

    const NodeKind NodeEndOfFile::GetKind() const {
        return NodeKind::ND_EndOfFile;
    }
    
    void NodeEndOfFile::Show(apic_string indent) {
        printf("%sNodeEndOfFile\n", indent.c_str());
    }
}
#include "core/analyzer.hpp"
#include "nodes/entrypoint.hpp"
#include "utils/diagnostic_bag.hpp"
#include "utils/errors.hpp"
#include "nodes/data/specs.hpp"
#include "nodes/global_scope.hpp"
#include "utils/id_generator.hpp"

using namespace core;

Analyzer::Analyzer() {

}

void Analyzer::analyze(nodes::NodeCompound *root, bool cmp) {
    if (cmp && !this->checkEntrypoints(root))
        return;
    
    this->setIds(root);
    this->extractSpecifications(root);
    this->extractGlobals(root);
}

bool Analyzer::checkEntrypoints(nodes::NodeCompound *root) {
    uint8_t entry_init = 0, entry_update = 0, entry_quit = 0;
    bool success = true;

    for (nodes::Node *node : root->getNodes()) {
        if (node->getKind() == nodes::NodeKind::Entrypoint) {
            nodes::NodeEntrypoint *entrypoint = static_cast<nodes::NodeEntrypoint*>(node);
            switch (entrypoint->getEntryBytecode()) {
                case common::bytecodes::ApicaEntrypointBytecode::Init:
                    entry_init = entry_init == 0 ? 1 : 2;
                    break;
                
                case common::bytecodes::ApicaEntrypointBytecode::Update:
                    entry_update = entry_update == 0 ? 1 : 2;
                    break;
                
                case common::bytecodes::ApicaEntrypointBytecode::Quit:
                    entry_quit = entry_quit == 0 ? 1 : 2;
                    break;

                default: break;
            }
        }
    }

    if (entry_init != 1) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::ANL_ERROR_NO_UNIQUE_INIT)
        ));
        success = false;
    }

    if (entry_update != 1) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::ANL_ERROR_NO_UNIQUE_UPDATE)
        ));
        success = false;
    }

    if (entry_quit != 1) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::ANL_ERROR_NO_UNIQUE_QUIT)
        ));
        success = false;
    }

    return success;
}

bool Analyzer::checkSpecifications(nodes::NodeCompound *root) {
    uint8_t specs = 0;
    bool spec_title = false, spec_id = false, spec_version = false;

    bool success = true;

    std::optional<nodes::NodeDataSpecifications*> node_specs = std::nullopt;
    for (nodes::Node *node : root->getNodes()) {
        if (node->getKind() == nodes::NodeKind::DataSpecs) {
            specs = specs == 0 ? 1 : 2;
            node_specs = static_cast<nodes::NodeDataSpecifications*>(node);
        }
    }

    if (specs != 1) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::ANL_ERROR_NO_SPECS)
        ));
        return false;
    }

    for (nodes::NodeData *data : node_specs.value()->getDataNodes()) {
        switch (data->getSpecificationBytecode()) {
            case common::bytecodes::ApicaSpecificationBytecode::Title:
                spec_title = true;
                break;
            
            case common::bytecodes::ApicaSpecificationBytecode::Id:
                spec_id = true;
                break;
            
            case common::bytecodes::ApicaSpecificationBytecode::Version:
                spec_version = true;
                break;

            default: break;
        }
    }

    if (!spec_title) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::ANL_ERROR_NO_TITLE_SPEC)
        ));
        success = false;
    }

    if (!spec_id) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::ANL_ERROR_NO_ID_SPEC)
        ));
        success = false;
    }

    if (!spec_version) {
        utils::DiagnosticBag::getInstance().addDiagnostic(utils::Diagnostic(
            utils::DiagnosticKind::Error,
            std::string(utils::ANL_ERROR_NO_VERSION_SPEC)
        ));
        success = false;
    }

    return success;
}

void Analyzer::extractGlobals(nodes::NodeCompound *root) {
    std::vector<nodes::Node*> extracted;
    std::vector<nodes::Node*> &mutable_nodes = root->getMutableNodes();

    for (auto it = mutable_nodes.begin(); it != mutable_nodes.end(); ) {
        nodes::Node* node = *it;

        if (node->getKind() == nodes::NodeKind::Entrypoint ||
            node->getKind() == nodes::NodeKind::DataSpecs ||
            node->getKind() == nodes::NodeKind::EndOfFile) {
            ++it;
        } else {
            extracted.push_back(node);
            it = mutable_nodes.erase(it);
        }
    }

    if (!extracted.empty()) {
        std::optional<nodes::NodeEntrypoint*> entrypoint_init = std::nullopt;
        for (nodes::Node *node : mutable_nodes) {
            if (node->getKind() == nodes::NodeKind::Entrypoint) {
                nodes::NodeEntrypoint *entrypoint = static_cast<nodes::NodeEntrypoint*>(node);
                if (entrypoint->getEntryBytecode() == common::bytecodes::ApicaEntrypointBytecode::Init) {
                    entrypoint_init = entrypoint;
                    break;
                }
            }
        }

        if (entrypoint_init) {
            nodes::NodeCompound *init_compound = static_cast<nodes::NodeCompound*>(entrypoint_init.value()->getBody());
            std::vector<nodes::Node*> &mutable_init_nodes = init_compound->getMutableNodes();
            mutable_init_nodes.insert(mutable_init_nodes.begin(), new nodes::NodeGlobalScope(
                extracted[0]->getPosition(),
                new nodes::NodeCompound(
                    extracted[0]->getPosition(),
                    extracted
                )
            ));
        }
    }
}

void Analyzer::extractSpecifications(nodes::NodeCompound *root) {
    std::vector<nodes::Node*> &mutable_nodes = root->getMutableNodes();
    std::optional<nodes::NodeDataSpecifications*> data_specs = std::nullopt;

    for (auto it = mutable_nodes.begin(); it != mutable_nodes.end(); ++it) {
        nodes::Node* node = *it;

        if (node->getKind() == nodes::NodeKind::DataSpecs) {
            data_specs = static_cast<nodes::NodeDataSpecifications*>(node);
            mutable_nodes.erase(it);
            break;
        }
    }

    if (data_specs) {
        data_specs.value()->addIdCount(utils::IdGenerator::getInstance().getIdCount());
        mutable_nodes.insert(mutable_nodes.begin(), data_specs.value());
    }
}

void Analyzer::setIds(nodes::NodeCompound *root) {
    root->setId();
}
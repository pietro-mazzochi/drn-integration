#include <drn_integration_communication/entities/api/node_types.hpp>

namespace drn_integration_communication
{
    NodeType::NodeType(int id, const std::string &name)
        : id(id), name(name) {}
}
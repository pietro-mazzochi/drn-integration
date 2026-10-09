#include <drn_integration_communication/entities/api/node_connections.hpp>

namespace drn_integration_communication
{
    NodeConnection::NodeConnection(int id, const std::vector<std::vector<double>>& line, int source_node_id, int target_node_id)
        : id(id), line(line), source_node_id(source_node_id), target_node_id(target_node_id) {}
}
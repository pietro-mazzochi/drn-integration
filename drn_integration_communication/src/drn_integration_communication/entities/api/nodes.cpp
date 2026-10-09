#include <drn_integration_communication/entities/api/nodes.hpp>

namespace drn_integration_communication
{
    Node::Node(int id, const std::string& pos_x, const std::string& pos_y, const std::string& angle, const std::string& deleted_supervisory, int map_id, int node_type, const std::string& direction, const std::string& label_id)
        : id(id), pos_x(pos_x), pos_y(pos_y), angle(angle),  deleted_supervisory(deleted_supervisory), map_id(map_id), node_type(node_type), direction(direction), label_id(label_id) {}
}
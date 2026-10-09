#include <drn_integration_communication/entities/api/anchors.hpp>

namespace drn_integration_communication
{
    Anchor::Anchor(int id, const std::string &name, const std::string& pos_x, const std::string& pos_y, const std::string& angle)
        : id(id), name(name), pos_x(pos_x), pos_y(pos_y), angle(angle) {}
}
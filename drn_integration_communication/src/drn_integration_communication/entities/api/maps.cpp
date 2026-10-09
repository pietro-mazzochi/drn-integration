#include <drn_integration_communication/entities/api/maps.hpp>

namespace drn_integration_communication
{
    Map::Map(int id, const std::string& name, const std::string& mode, const std::string& resolution, const std::string& origin_x, const std::string& origin_y, const std::string& origin_z, const std::string& negate, const std::string& occupied_thresh, const std::string& free_thresh)
        : id(id), name(name), mode(mode), resolution(resolution), origin_x(origin_x), origin_y(origin_y), origin_z(origin_z), negate(negate), occupied_thresh(occupied_thresh), free_thresh(free_thresh) {}
}
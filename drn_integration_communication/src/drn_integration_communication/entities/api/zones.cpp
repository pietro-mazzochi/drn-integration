#include <drn_integration_communication/entities/api/zones.hpp>

namespace drn_integration_communication
{
    Zone::Zone(int id, const std::string& max_velocity, const std::vector<std::vector<double>>& area, int map_id, int type_zone_id)
        : id(id), max_velocity(max_velocity), area(area), map_id(map_id), type_zone_id(type_zone_id) {}
}
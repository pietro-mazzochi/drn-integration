#include <drn_integration_communication/entities/api/zone_types.hpp>

namespace drn_integration_communication
{
    ZoneType::ZoneType(int id, const std::string& name)
        : id(id), name(name) {}
}
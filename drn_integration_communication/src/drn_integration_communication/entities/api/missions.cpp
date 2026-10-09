#include <drn_integration_communication/entities/api/missions.hpp>

namespace drn_integration_communication
{
    Mission::Mission(int id, const std::string &title, const std::string &type, int end_id, const std::string &mission_direction)
        : id(id), title(title), type(type), end_id(end_id), mission_direction(mission_direction)  {}
}
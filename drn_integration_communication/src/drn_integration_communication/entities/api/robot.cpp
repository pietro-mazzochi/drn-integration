#include <drn_integration_communication/entities/api/robot.hpp>

namespace drn_integration_communication
{
    Robot::Robot(int id, const std::string& uuid, const std::string& data, const std::string& timestamp)
        : id(id), uuid(uuid) ,data(data), timestamp(timestamp) {}
}

#include <drn_integration_communication/entities/api/logs.hpp>

namespace drn_integration_communication
{
    Log::Log(int id, const std::string& uuid, const std::string& data, const std::string& timestamp, const std::string& supervisory_registration)
        : id(id), uuid(uuid) ,data(data), timestamp(timestamp), supervisory_registration(supervisory_registration) {}
}

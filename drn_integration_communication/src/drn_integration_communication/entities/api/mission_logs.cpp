#include <drn_integration_communication/entities/api/mission_logs.hpp>

namespace drn_integration_communication
{
    MissionLog::MissionLog(int id, const std::string& uuid, const std::string& status, const std::string &data, const std::string& timestamp, const std::string& supervisory_registration, int mission_id)
        : id(id), uuid(uuid), status(status), data(data), timestamp(timestamp), mission_id(mission_id), supervisory_registration(supervisory_registration) {}
}
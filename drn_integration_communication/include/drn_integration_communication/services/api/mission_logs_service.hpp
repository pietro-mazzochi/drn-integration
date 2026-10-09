#ifndef DRN_INTEGRATION_COMUNICATION_MISSIONLOGSERVICE_H
#define DRN_INTEGRATION_COMUNICATION_MISSIONLOGSERVICE_H

#include <drn_integration_communication/entities/api/mission_logs.hpp>
#include <pqxx/pqxx>

namespace drn_integration_communication
{
    class MissionLogService
    {
    public:
        static void create(pqxx::connection &connection_ptr, const MissionLog &missionLog);
        static std::vector<MissionLog> getAll(pqxx::connection &connection_ptr);
        static MissionLog getById(pqxx::connection &connection_ptr, int id);
        static void update(pqxx::connection &connection_ptr, const MissionLog &missionLog);
        static int32_t getLastId(pqxx::connection &connection_ptr);
        static void removeFirstRecord(pqxx::connection &connection_ptr);
        static void removeMissionLogs(pqxx::connection &connection_ptr, const std::string &mission_id);
        static void removeMissionLogs(pqxx::connection &connection_ptr, const int16_t mission_id);
        static void remove(pqxx::connection &connection_ptr, int id);
        static void updateSupervisoryRegistration(pqxx::connection &connection_ptr, const std::string &uuid);
    };
}
#endif
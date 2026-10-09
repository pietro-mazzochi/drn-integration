#ifndef DRN_INTEGRATION_COMUNICATION_MISSIONSERVICE_H
#define DRN_INTEGRATION_COMUNICATION_MISSIONSERVICE_H

#include <drn_integration_communication/entities/api/missions.hpp>
#include <pqxx/pqxx>

namespace drn_integration_communication
{
    class MissionService 
    {
        public:
            static void create(pqxx::connection &connection_ptr, const Mission& mission);
            static std::vector<Mission> getAll(pqxx::connection &connection_ptr);
            static Mission getById(pqxx::connection &connection_ptr, int id);
            static Mission getLastMission(pqxx::connection &connection_ptr);
            static bool existId(pqxx::connection &connection_ptr, int id);
            static void update(pqxx::connection &connection_ptr, const Mission& mission);
            static int32_t getLastId(pqxx::connection &connection_ptr);
            static void removeFirstRecord(pqxx::connection &connection_ptr);
            static int32_t getRecordCount(pqxx::connection &connection_ptr);
            static int32_t getFirstId(pqxx::connection &connection_ptr);            
            static void remove(pqxx::connection &connection_ptr, int id);
    };
}
#endif
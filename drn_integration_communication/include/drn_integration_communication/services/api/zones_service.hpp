#ifndef DRN_INTEGRATION_COMUNICATION_ZONESERVICE_H
#define DRN_INTEGRATION_COMUNICATION_ZONESERVICE_H

#include <drn_integration_communication/entities/api/zones.hpp>
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>

namespace drn_integration_communication
{
    class ZoneService 
    {
        public:
            static void create(pqxx::connection &connection_ptr, const Zone& zone);
            static std::vector<Zone> getAll(pqxx::connection &connection_ptr);
            static Zone getById(pqxx::connection &connection_ptr, int id);
            static void update(pqxx::connection &connection_ptr, const Zone& zone);
            static void remove(pqxx::connection &connection_ptr, int id);
    };
}
#endif
#ifndef DRN_INTEGRATION_COMUNICATION_MAPSERVICE_H
#define DRN_INTEGRATION_COMUNICATION_MAPSERVICE_H

#include <drn_integration_communication/entities/api/maps.hpp>
#include <pqxx/pqxx>

namespace drn_integration_communication
{
    class MapService 
    {
        public:
            static void create(pqxx::connection &connection_ptr, const Map& map);
            static std::vector<Map> getAll(pqxx::connection &connection_ptr);
            static Map getById(pqxx::connection &connection_ptr, int id);
            static int32_t getLastId(pqxx::connection &connection_ptr);
            static void update(pqxx::connection &connection_ptr, const Map& map);
            static void remove(pqxx::connection &connection_ptr, int id);
            static void clearMapTable(pqxx::connection &connection_ptr);
    };
}
#endif
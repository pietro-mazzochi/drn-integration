#ifndef DRN_INTEGRATION_COMUNICATION_ZONETYPESERVICE_H
#define DRN_INTEGRATION_COMUNICATION_ZONETYPESERVICE_H

#include <drn_integration_communication/entities/api/zone_types.hpp>
#include <pqxx/pqxx>

namespace drn_integration_communication
{
    class ZoneTypeService 
    {
        public:
            static void create(pqxx::connection &connection_ptr, const ZoneType& zoneType);
            static std::vector<ZoneType> getAll(pqxx::connection &connection_ptr);
            static ZoneType getById(pqxx::connection &connection_ptr, int id);
            static void update(pqxx::connection &connection_ptr, const ZoneType& zoneType);
            static void remove(pqxx::connection &connection_ptr, int id);
    };
}
#endif
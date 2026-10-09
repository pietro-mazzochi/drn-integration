#ifndef DRN_INTEGRATION_COMUNICATION_TELEMETRIESERVICE_H
#define DRN_INTEGRATION_COMUNICATION_TELEMETRIESERVICE_H

#include <drn_integration_communication/entities/api/telemetries.hpp>
#include <pqxx/pqxx>

namespace drn_integration_communication
{
    class TelemetrieService 
    {
        public:
            static void create(pqxx::connection &connection_ptr, const Telemetrie& telemetrie);
            static std::vector<Telemetrie> getAll(pqxx::connection &connection_ptr);
            static Telemetrie getById(pqxx::connection &connection_ptr, int id);
            static void update(pqxx::connection &connection_ptr, const Telemetrie& telemetrie);
            static void remove(pqxx::connection &connection_ptr, int id);
    };
}
#endif
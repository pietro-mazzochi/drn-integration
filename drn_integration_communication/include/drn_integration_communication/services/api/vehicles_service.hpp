#ifndef DRN_INTEGRATION_COMUNICATION_VEHICLESERVICE_H
#define DRN_INTEGRATION_COMUNICATION_VEHICLESERVICE_H

#include <drn_integration_communication/entities/api/vehicles.hpp>
#include <pqxx/pqxx>

namespace drn_integration_communication
{
    class VehicleService 
    {
        public:
            static void create(pqxx::connection &connection_ptr, const Vehicle& vehicle);
            static std::vector<Vehicle> getAll(pqxx::connection &connection_ptr);
            static Vehicle getById(pqxx::connection &connection_ptr, int id);
            static void update(pqxx::connection &connection_ptr, const Vehicle& vehicle);
            static void remove(pqxx::connection &connection_ptr, int id);
    };
}
#endif
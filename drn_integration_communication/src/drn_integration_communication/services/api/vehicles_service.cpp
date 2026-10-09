#include <drn_integration_communication/services/api/vehicles_service.hpp>
#include <stdexcept>

namespace drn_integration_communication
{
    void VehicleService::create(pqxx::connection &connection_ptr, const Vehicle &Vehicle)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "INSERT INTO vehicles (code, name, model) VALUES ('" + W.esc(Vehicle.code) + "', '" + W.esc(Vehicle.name) + "', '" + W.esc(Vehicle.model) + "');";
        W.exec(query);
        W.commit();
    }

    std::vector<Vehicle> VehicleService::getAll(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, code, name, model FROM vehicles;";
        pqxx::result R(N.exec(query));

        std::vector<Vehicle> vehicles;

        for (const auto &row : R)
        {
            vehicles.emplace_back(
                row[0].as<int>(),        
                row[1].as<std::string>(), 
                row[2].as<std::string>(), 
                row[3].as<std::string>()  
            );
        }

        return vehicles;
    }

    Vehicle VehicleService::getById(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, code, name, model FROM vehicles WHERE id = " + std::to_string(id) + ";";
        pqxx::result R(N.exec(query));

        if (R.size() != 1)
        {
            return Vehicle(0, "0", "0", "0");
        }

        auto row = R[0];
        return Vehicle(row[0].as<int>(), row[1].as<std::string>(), row[2].as<std::string>(), row[3].as<std::string>());
    }

    void VehicleService::update(pqxx::connection &connection_ptr, const Vehicle &Vehicle)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query =
            "INSERT INTO vehicles (id, code, name, model) "
            "VALUES (" +
            std::to_string(Vehicle.id) + ", '" + W.esc(Vehicle.code) + "', '" + W.esc(Vehicle.name) + "', '" + W.esc(Vehicle.model) + "') "
                                                                                                                                      "ON CONFLICT (id) DO UPDATE SET "
                                                                                                                                      "code = EXCLUDED.code, name = EXCLUDED.name, model = EXCLUDED.model;";

        W.exec(query);
        W.commit();
    }

    void VehicleService::remove(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "DELETE FROM vehicles WHERE id = " + std::to_string(id) + ";";
        W.exec(query);
        W.commit();
    }
}
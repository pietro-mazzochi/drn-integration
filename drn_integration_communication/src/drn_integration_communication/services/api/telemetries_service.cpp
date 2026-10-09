#include <drn_integration_communication/services/api/telemetries_service.hpp>
#include <stdexcept>

namespace drn_integration_communication
{
    void TelemetrieService::create(pqxx::connection &connection_ptr, const Telemetrie &Telemetrie)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "INSERT INTO telemetries (data, pos_x, pos_y, angle, battery, tension, velocity) VALUES ('" + W.esc(Telemetrie.data) + "', '" + W.esc(Telemetrie.pos_x) + "', '" + W.esc(Telemetrie.pos_y) + "', '" + W.esc(Telemetrie.angle) + "', '" + W.esc(Telemetrie.battery) + "', '" + W.esc(Telemetrie.tension) + "', '" + W.esc(Telemetrie.velocity) + "');";
        W.exec(query);
        W.commit();
    }

    std::vector<Telemetrie> TelemetrieService::getAll(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, data, pos_x, pos_y, angle, battery, tension, velocity FROM telemetries;";
        pqxx::result R(N.exec(query));

        std::vector<Telemetrie> telemetries;

        for (const auto &row : R)
        {
            telemetries.emplace_back(
                row[0].as<int>(),        
                row[1].as<std::string>(), 
                row[2].as<std::string>(), 
                row[3].as<std::string>(), 
                row[4].as<std::string>(),
                row[5].as<std::string>(), 
                row[6].as<std::string>(), 
                row[7].as<std::string>()  
            );
        }

        return telemetries;
    }

    Telemetrie TelemetrieService::getById(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, data, pos_x, pos_y, angle, battery, tension, velocity FROM telemetries WHERE id = " + std::to_string(id) + ";";
        pqxx::result R(N.exec(query));

        if (R.size() != 1)
        {
            throw std::runtime_error("Telemetrie not found");
        }

        auto row = R[0];
        return Telemetrie(row[0].as<int>(), row[1].as<std::string>(), row[2].as<std::string>(), row[3].as<std::string>(), row[4].as<std::string>(), row[5].as<std::string>(), row[6].as<std::string>(), row[7].as<std::string>());
    }

    void TelemetrieService::update(pqxx::connection &connection_ptr, const Telemetrie &Telemetrie)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "UPDATE telemetries SET data = '" + W.esc(Telemetrie.data) + "', pos_x = '" + W.esc(Telemetrie.pos_x) + "', pos_y = '" + W.esc(Telemetrie.pos_y) + "', angle = '" + W.esc(Telemetrie.angle) + "', battery = '" + W.esc(Telemetrie.battery) + "', tension = '" + W.esc(Telemetrie.tension) + "', velocity = '" + W.esc(Telemetrie.velocity) + "' WHERE id = " + std::to_string(Telemetrie.id) + ";";
        W.exec(query);
        W.commit();
    }

    void TelemetrieService::remove(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "DELETE FROM telemetries WHERE id = " + std::to_string(id) + ";";
        W.exec(query);
        W.commit();
    }
}
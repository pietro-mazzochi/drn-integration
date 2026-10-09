#include <drn_integration_communication/services/api/zones_service.hpp>
#include <stdexcept>

namespace drn_integration_communication
{
    void ZoneService::create(pqxx::connection &connection_ptr, const Zone &Zone)
    {
        pqxx::nontransaction W(connection_ptr);

        nlohmann::json area_json = Zone.area;
        std::string area_str = area_json.dump();

        std::string query = "INSERT INTO zones (max_velocity, area, map_id, type_zone_id) VALUES ('" + W.esc(Zone.max_velocity) + "', '" + W.esc(area_str) + "', '" + W.esc(std::to_string(Zone.map_id)) + "', '" + W.esc(std::to_string(Zone.type_zone_id)) + "');";
        W.exec(query);
        W.commit();
    }

    std::vector<Zone> ZoneService::getAll(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, max_velocity, area, map_id, type_zone_id FROM zones;";
        pqxx::result R(N.exec(query));

        std::vector<Zone> zones;

        for (const auto &row : R)
        {
            std::string area_str = row["area"].as<std::string>();
            nlohmann::json area_json = nlohmann::json::parse(area_str);

            std::vector<std::vector<double>> area;
            for (const auto &coord_pair : area_json)
            {
                std::vector<double> coordinates;
                coordinates.push_back(coord_pair[0].get<double>());
                coordinates.push_back(coord_pair[1].get<double>());
                area.push_back(coordinates);
            }

            zones.emplace_back(
                row["id"].as<int>(),                  
                row["max_velocity"].as<std::string>(),
                area,                                
                row["map_id"].as<int>(),          
                row["type_zone_id"].as<int>()         
            );
        }

        return zones;
    }

    Zone ZoneService::getById(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, max_velocity, area, map_id, type_zone_id FROM zones WHERE id = " + std::to_string(id) + ";";
        pqxx::result R(N.exec(query));

        if (R.size() != 1)
        {
            throw std::runtime_error("zones not found");
        }

        auto row = R[0];

        std::string area_str = row["area"].as<std::string>();
        nlohmann::json area_json = nlohmann::json::parse(area_str);

        std::vector<std::vector<double>> area;
        for (const auto &coord_pair : area_json)
        {
            std::vector<double> coordinates;
            coordinates.push_back(coord_pair[0].get<double>());
            coordinates.push_back(coord_pair[1].get<double>());
            area.push_back(coordinates);
        }

        return Zone(row["id"].as<int>(), row["max_velocity"].as<std::string>(), area, row["map_id"].as<int>(), row["type_zone_id"].as<int>());
    }

    void ZoneService::update(pqxx::connection &connection_ptr, const Zone &Zone)
    {
        pqxx::nontransaction W(connection_ptr);

        nlohmann::json area_json = Zone.area;
        std::string area_str = area_json.dump();

        std::string query =
            "INSERT INTO zones (id, max_velocity, area, map_id, type_zone_id) "
            "VALUES (" +
            std::to_string(Zone.id) + ", '" + W.esc(Zone.max_velocity) + "', '" + W.esc(area_str) + "', '" + W.esc(std::to_string(Zone.map_id)) + "', '" + W.esc(std::to_string(Zone.type_zone_id)) + "') "
                                                                                                                                                                                                     "ON CONFLICT (id) DO UPDATE SET "
                                                                                                                                                                                                     "max_velocity = EXCLUDED.max_velocity, area = EXCLUDED.area, map_id = EXCLUDED.map_id, type_zone_id = EXCLUDED.type_zone_id;";
        W.exec(query);
        W.commit();
    }

    void ZoneService::remove(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "DELETE FROM zones WHERE id = " + std::to_string(id) + ";";
        W.exec(query);
        W.commit();
    }
}
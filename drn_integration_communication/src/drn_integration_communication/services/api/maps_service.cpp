#include <drn_integration_communication/services/api/maps_service.hpp>
#include <stdexcept>

namespace drn_integration_communication
{
    void MapService::create(pqxx::connection &connection_ptr, const Map &Map)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "INSERT INTO maps (name, mode, resolution, origin_x, origin_y, origin_z, negate, occupied_thresh, free_thresh) VALUES ('" + 
                            W.esc(Map.name) + "', '" + W.esc(Map.mode) + "', '" + W.esc(Map.resolution) + "', '" + W.esc(Map.origin_x) + 
                            "', '" + W.esc(Map.origin_y) + "', '" + W.esc(Map.origin_z) + "', '" + 
                            W.esc(Map.negate) + "', '" + W.esc(Map.occupied_thresh) + "', '" + 
                            W.esc(Map.free_thresh) + "');";
        W.exec(query);
        W.commit();
    }

    std::vector<Map> MapService::getAll(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, name, mode, resolution, origin_x, origin_y, origin_z, negate, occupied_thresh, free_thresh FROM maps;";
        pqxx::result R(N.exec(query));

        std::vector<Map> maps;

        for (const auto &row : R)
        {
            maps.emplace_back(
                row[0].as<int>(),               // id
                row[1].as<std::string>(),        // name
                row[2].as<std::string>(),        // mode
                row[3].as<std::string>(),        // resolution
                row[4].as<std::string>(),        // origin_x
                row[5].as<std::string>(),        // origin_y
                row[6].as<std::string>(),        // origin_z
                row[7].as<std::string>(),        // negate
                row[8].as<std::string>(),        // occupied_thresh
                row[9].as<std::string>()         // free_thresh
            );
        }

        return maps;
    }

    Map MapService::getById(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, name, mode, resolution, origin_x, origin_y, origin_z, negate, occupied_thresh, free_thresh FROM maps WHERE id = " + std::to_string(id) + ";";
        pqxx::result R(N.exec(query));

        if (R.size() != 1)
        {
            throw std::runtime_error("Map not found");
        }

        auto row = R[0];
        return Map(row[0].as<int>(), row[1].as<std::string>(), row[2].as<std::string>(),  row[3].as<std::string>(), row[4].as<std::string>(), row[5].as<std::string>(), row[6].as<std::string>(), row[7].as<std::string>(), row[8].as<std::string>(), row[9].as<std::string>());
    }

    int32_t MapService::getLastId(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id FROM maps ORDER BY id DESC LIMIT 1;";
        pqxx::result R(N.exec(query));

        if (R.empty())
        {
            return 0;
        }

        return R[0][0].as<int>();
    } 

    void MapService::update(pqxx::connection &connection_ptr, const Map &Map)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query =
            "INSERT INTO maps (id, name, mode, resolution, origin_x, origin_y, origin_z, negate, occupied_thresh, free_thresh) "
            "VALUES (" +
            std::to_string(Map.id) + ", '" + W.esc(Map.name) + "', '" + W.esc(Map.mode) + "', '" + W.esc(Map.resolution) + "', '" + 
            W.esc(Map.origin_x) + "', '" + W.esc(Map.origin_y) + "', '" + 
            W.esc(Map.origin_z) + "', '" + W.esc(Map.negate) + "', '" + 
            W.esc(Map.occupied_thresh) + "', '" + W.esc(Map.free_thresh) + "') "
            "ON CONFLICT (id) DO UPDATE SET "
            "name = EXCLUDED.name, mode = EXCLUDED.mode, resolution = EXCLUDED.resolution, origin_x = EXCLUDED.origin_x, origin_y = EXCLUDED.origin_y, origin_z = EXCLUDED.origin_z, "
            "negate = EXCLUDED.negate, occupied_thresh = EXCLUDED.occupied_thresh, free_thresh = EXCLUDED.free_thresh;";

        W.exec(query);
        W.commit();
    }

    void MapService::remove(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "DELETE FROM maps WHERE id = " + std::to_string(id) + ";";
        W.exec(query);
        W.commit();
    }

    void MapService::clearMapTable(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction W(connection_ptr);

        std::string delete_query;

        delete_query = "DELETE FROM mission_logs;";
        W.exec(delete_query);

        delete_query = "DELETE FROM missions;";
        W.exec(delete_query);

        delete_query = "DELETE FROM anchors;";
        W.exec(delete_query);

        delete_query = "DELETE FROM node_connections;";
        W.exec(delete_query);

        delete_query = "DELETE FROM nodes;";
        W.exec(delete_query);

        delete_query = "DELETE FROM node_types;";
        W.exec(delete_query);

        delete_query = "DELETE FROM zones;";
        W.exec(delete_query);

        delete_query = "DELETE FROM zone_types;";
        W.exec(delete_query);

        delete_query = "DELETE FROM maps;";
        W.exec(delete_query);

        W.commit();
    }
}

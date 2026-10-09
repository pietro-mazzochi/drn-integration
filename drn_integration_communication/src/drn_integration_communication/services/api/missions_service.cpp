#include <drn_integration_communication/services/api/missions_service.hpp>
#include <stdexcept>

namespace drn_integration_communication
{
    void MissionService::create(pqxx::connection &connection_ptr, const Mission &mission)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "INSERT INTO missions (id, title, type, end_id, mission_direction) VALUES ('" +
                            W.esc(std::to_string(mission.id)) + "', '" +
                            W.esc(mission.title) + "', '" +
                            W.esc(mission.type) + "', '" +
                            W.esc(std::to_string(mission.end_id)) + "', '" +
                            W.esc(mission.mission_direction) + "');";
        W.exec(query);
        W.commit();
    }

    std::vector<Mission> MissionService::getAll(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, title, type, end_id FROM missions;";
        pqxx::result R(N.exec(query));

        std::vector<Mission> missions;

        for (const auto &row : R)
        {
            missions.emplace_back(
                row["id"].as<int>(),
                row["title"].as<std::string>(),
                row["type"].as<std::string>(),
                row["end_id"].as<int>(),
                row["mission_direction"].as<std::string>());
        }

        return missions;
    }

    Mission MissionService::getById(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, title, type, end_id, mission_direction FROM missions WHERE id = " + std::to_string(id) + ";";
        pqxx::result R(N.exec(query));

        if (R.size() != 1)
        {
            throw std::runtime_error("missions not found");
        }

        auto row = R[0];
        return Mission(row["id"].as<int>(), row["title"].as<std::string>(), row["type"].as<std::string>(), row["end_id"].as<int>(), row["mission_direction"].as<std::string>());
    }

    void MissionService::update(pqxx::connection &connection_ptr, const Mission &Mission)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "UPDATE missions SET title = '" + W.esc(Mission.title) + "', type = '" + W.esc(Mission.type) + "', end_id = '" + W.esc(std::to_string(Mission.end_id)) + "' WHERE id = " + std::to_string(Mission.id) + "', mission_direction = '" + W.esc(Mission.mission_direction) + "';";
        W.exec(query);
        W.commit();
    }

    Mission MissionService::getLastMission(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction N(connection_ptr);
        pqxx::result R = N.exec("SELECT * FROM missions ORDER BY id DESC LIMIT 1");

        if (R.empty())
        {
            return Mission(0, "", "", 0, "");
        }

        auto row = R[0];
        return Mission(
            row["id"].as<int>(),
            row["title"].as<std::string>(),
            row["type"].as<std::string>(),
            row["end_id"].as<int>(),
            row["mission_direction"].as<std::string>()
        );
    }

    bool MissionService::existId(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction W(connection_ptr);

        pqxx::result R = W.exec("SELECT id FROM missions WHERE id = " + W.esc(std::to_string(id)) + ";");

        W.commit();

        return !R.empty();
    }

    int32_t MissionService::getLastId(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction W(connection_ptr);
        int32_t last_id = 0;
        std::string query = "SELECT COALESCE(MAX(id), 0) AS last_id FROM missions;";
        pqxx::result R = W.exec(query);

        if (!R.empty())
        {
            last_id = R[0][0].as<int32_t>();
        }

        W.commit();
        return last_id;
    }

    int32_t MissionService::getFirstId(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction W(connection_ptr);
        int32_t first_id = 0;
        std::string query = "SELECT COALESCE(MIN(id), 0) AS first_id FROM missions;";
        pqxx::result R = W.exec(query);

        if (!R.empty())
        {
            first_id = R[0][0].as<int32_t>();
        }

        W.commit();
        return first_id;
    }

    int32_t MissionService::getRecordCount(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction W(connection_ptr);
        int32_t record_count = 0;
        std::string query = "SELECT COUNT(*) AS record_count FROM missions;";
        pqxx::result R = W.exec(query);

        if (!R.empty())
        {
            record_count = R[0][0].as<int32_t>();
        }

        W.commit();
        return record_count;
    }

    void MissionService::removeFirstRecord(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction W(connection_ptr);

        std::string query_select = "SELECT id FROM missions ORDER BY id ASC LIMIT 1;";
        pqxx::result R = W.exec(query_select);

        if (!R.empty())
        {
            int32_t first_id = R[0][0].as<int32_t>();
            std::string query_delete = "DELETE FROM missions WHERE id = " + std::to_string(first_id) + ";";
            W.exec(query_delete);
        }

        W.commit();
    }

    void MissionService::remove(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "DELETE FROM missions WHERE id = " + std::to_string(id) + ";";
        W.exec(query);
        W.commit();
    }
}
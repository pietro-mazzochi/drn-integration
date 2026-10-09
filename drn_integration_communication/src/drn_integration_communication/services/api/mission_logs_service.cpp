#include <drn_integration_communication/services/api/mission_logs_service.hpp>
#include <stdexcept>

namespace drn_integration_communication
{
    void MissionLogService::create(pqxx::connection &connection_ptr, const MissionLog &MissionLog)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "INSERT INTO mission_logs (uuid, status, data, timestamp, supervisory_registration, mission_id) VALUES ('" + W.esc(MissionLog.uuid) + "','" + W.esc(MissionLog.status) + "', '" + W.esc(MissionLog.data) + "', '" + W.esc(MissionLog.timestamp) + "', '" + W.esc(MissionLog.supervisory_registration) + "', '" + W.esc(std::to_string(MissionLog.mission_id)) + "');";
        W.exec(query);
        W.commit();
    }

    void MissionLogService::removeMissionLogs(pqxx::connection &connection_ptr, const int16_t mission_id)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "DELETE FROM mission_logs WHERE mission_id = '" + W.esc(std::to_string(mission_id)) + "';";
        W.exec(query);
        W.commit();
    }

    std::vector<MissionLog> MissionLogService::getAll(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, uuid, status, data, timestamp, supervisory_registration, mission_id FROM mission_logs;";
        pqxx::result R(N.exec(query));

        std::vector<MissionLog> missionLogs;

        for (const auto &row : R)
        {
            missionLogs.emplace_back(
                row["id"].as<int>(),
                row["uuid"].as<std::string>(),
                row["status"].as<std::string>(),
                row["data"].as<std::string>(),
                row["timestamp"].as<std::string>(),
                row["supervisory_registration"].as<std::string>(),
                row["mission_id"].as<int>());
        }

        return missionLogs;
    }

    void MissionLogService::updateSupervisoryRegistration(pqxx::connection &connection_ptr, const std::string &uuid)
    {
        pqxx::nontransaction W(connection_ptr);

        std::string check_query = "SELECT COUNT(*) FROM mission_logs WHERE uuid = '" + W.esc(uuid) + "';";
        pqxx::result R = W.exec(check_query);

        if (!R.empty() && R[0][0].as<int>() > 0)
        {
            std::string update_query = "UPDATE mission_logs SET supervisory_registration = 'true' WHERE uuid = '" + W.esc(uuid) + "';";
            W.exec(update_query);
            W.commit();
        }
        else
        {
            W.abort();
        }
    }

    MissionLog MissionLogService::getById(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, uuid, status, data, timestamp, supervisory_registration, mission_id FROM mission_logs WHERE id = " + std::to_string(id) + ";";
        pqxx::result R(N.exec(query));

        if (R.size() != 1)
        {
            throw std::runtime_error("mission_logs not found");
        }

        auto row = R[0];
        return MissionLog(row["id"].as<int>(), row["uuid"].as<std::string>(), row["status"].as<std::string>(), row["data"].as<std::string>(), row["timestamp"].as<std::string>(), row["supervisory_registration"].as<std::string>(), row["mission_id"].as<int>());
    }

    void MissionLogService::update(pqxx::connection &connection_ptr, const MissionLog &MissionLog)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "UPDATE mission_logs SET uuid = '" + W.esc(MissionLog.uuid) + "', status = '" + W.esc(MissionLog.status) + "', data = '" + W.esc(MissionLog.data) + "', timestamp = '" + W.esc(MissionLog.timestamp) + "', supervisory_registration = '" + W.esc(MissionLog.supervisory_registration) + "', mission_id = '" + W.esc(std::to_string(MissionLog.mission_id)) + "' WHERE id = " + std::to_string(MissionLog.id) + ";";
        W.exec(query);
        W.commit();
    }

    int32_t MissionLogService::getLastId(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction W(connection_ptr);
        int32_t last_id = 0;
        std::string query = "SELECT COALESCE(MAX(id), 0) AS last_id FROM mission_logs;";
        pqxx::result R = W.exec(query);

        if (!R.empty())
        {
            last_id = R[0][0].as<int32_t>();
        }

        W.commit();
        return last_id;
    }

    void MissionLogService::removeFirstRecord(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction W(connection_ptr);

        std::string query_select = "SELECT id FROM mission_logs ORDER BY id ASC LIMIT 1;";
        pqxx::result R = W.exec(query_select);

        if (!R.empty())
        {
            int32_t first_id = R[0][0].as<int32_t>();
            std::string query_delete = "DELETE FROM mission_logs WHERE id = " + std::to_string(first_id) + ";";
            W.exec(query_delete);
        }

        W.commit();
    }

    void MissionLogService::remove(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "DELETE FROM mission_logs WHERE id = " + std::to_string(id) + ";";
        W.exec(query);
        W.commit();
    }
}
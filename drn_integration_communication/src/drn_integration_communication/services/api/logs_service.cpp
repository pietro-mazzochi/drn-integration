#include <drn_integration_communication/services/api/logs_service.hpp>
#include <stdexcept>
#include <optional>

namespace drn_integration_communication
{
    void LogService::create(pqxx::connection &connection_ptr, const Log &Log)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "INSERT INTO logs (uuid, data, timestamp, supervisory_registration) VALUES ('" + W.esc(Log.uuid) + "', '" + W.esc(Log.data) + "', '" + W.esc(Log.timestamp) + "', '" + W.esc(Log.supervisory_registration) + "');";
        W.exec(query);
        W.commit();
    }

    std::vector<Log> LogService::getAll(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, uuid, data, timestamp, supervisory_registration FROM logs;";
        pqxx::result R(N.exec(query));

        std::vector<Log> logs;

        for (const auto &row : R)
        {
            logs.emplace_back(
                row[0].as<int>(),
                row[1].as<std::string>(),
                row[2].as<std::string>(),
                row[3].as<std::string>(),
                row[4].as<std::string>());
        }

        return logs;
    }

    void LogService::updateSupervisoryRegistration(pqxx::connection &connection_ptr, const std::string &uuid)
    {
        pqxx::nontransaction W(connection_ptr);

        std::string check_query = "SELECT COUNT(*) FROM logs WHERE uuid = '" + W.esc(uuid) + "';";
        pqxx::result R = W.exec(check_query);

        if (!R.empty() && R[0][0].as<int>() > 0)
        {
            std::string update_query = "UPDATE logs SET supervisory_registration = 'true' WHERE uuid = '" + W.esc(uuid) + "';";
            W.exec(update_query);
            W.commit();
        }
        else
        {
            W.abort(); 
        }
    }

    Log LogService::getById(pqxx::connection &connection_ptr, int id)
    {
        try
        {
            pqxx::nontransaction N(connection_ptr);
            pqxx::result R = N.exec_params(
                "SELECT id, uuid, data, timestamp, supervisory_registration FROM logs WHERE id = $1", id);

            if (R.empty())
            {
                throw std::runtime_error("Log not found");
            }

            auto row = R[0];
            return Log(row[0].as<int>(), row[1].as<std::string>(), row[2].as<std::string>(), row[3].as<std::string>(), row[4].as<std::string>());
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error(std::string("Database query error: ") + e.what());
        }
    }

    void LogService::update(pqxx::connection &connection_ptr, const Log &Log)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "UPDATE logs SET uuid = '" + W.esc(Log.uuid) + "', data = '" + W.esc(Log.data) + "', timestamp = '" + W.esc(Log.timestamp) + "', supervisory_registration = '" + W.esc(Log.supervisory_registration) + "' WHERE id = " + std::to_string(Log.id) + ";";
        W.exec(query);
        W.commit();
    }

    int32_t LogService::getLastId(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction W(connection_ptr);
        int32_t last_id = 0;
        std::string query = "SELECT COALESCE(MAX(id), 0) AS last_id FROM logs;";
        pqxx::result R = W.exec(query);

        if (!R.empty())
        {
            last_id = R[0][0].as<int32_t>();
        }

        W.commit();
        return last_id;
    }

    int32_t LogService::getRecordCount(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction W(connection_ptr);
        int32_t record_count = 0;
        std::string query = "SELECT COUNT(*) AS record_count FROM logs;";
        pqxx::result R = W.exec(query);

        if (!R.empty())
        {
            record_count = R[0][0].as<int32_t>();
        }

        W.commit();
        return record_count;
    }

    void LogService::removeFirstRecord(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction W(connection_ptr);

        std::string query_select = "SELECT id FROM logs ORDER BY id ASC LIMIT 1;";
        pqxx::result R = W.exec(query_select);

        if (!R.empty())
        {
            int32_t first_id = R[0][0].as<int32_t>();
            std::string query_delete = "DELETE FROM logs WHERE id = " + std::to_string(first_id) + ";";
            W.exec(query_delete);
        }

        W.commit();
    }

    void LogService::remove(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "DELETE FROM logs WHERE id = " + std::to_string(id) + ";";
        W.exec(query);
        W.commit();
    }

    bool LogService::existId(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction W(connection_ptr);

        pqxx::result R = W.exec("SELECT id FROM logs WHERE id = " + W.esc(std::to_string(id)) + ";");

        W.commit();

        return !R.empty();
    }

}
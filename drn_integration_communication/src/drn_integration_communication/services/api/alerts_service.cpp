#include <drn_integration_communication/services/api/alerts_service.hpp>
#include <stdexcept>

namespace drn_integration_communication
{
    void AlertService::create(pqxx::connection &connection_ptr, const Alert &Alert)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "INSERT INTO alerts (uuid, active, error_code, data, message, timestamp, supervisory_registration) VALUES ('" + W.esc(Alert.uuid) + "', '" + W.esc(Alert.active) + "', '" + W.esc(Alert.error_code) + "', '" + W.esc(Alert.data) + "', '" + W.esc(Alert.message) + "', '" + W.esc(Alert.timestamp) + "', '" + W.esc(Alert.supervisory_registration) + "');";
        W.exec(query);
        W.commit();
    }

    std::vector<Alert> AlertService::getAll(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, uuid, active, error_code, data, message, timestamp, supervisory_registration FROM alerts;";
        pqxx::result R(N.exec(query));

        std::vector<Alert> alerts;

        for (const auto &row : R)
        {
            alerts.emplace_back(
                row[0].as<int>(),
                row[1].as<std::string>(),
                row[2].as<std::string>(),
                row[3].as<std::string>(),
                row[4].as<std::string>(),
                row[5].as<std::string>(),
                row[6].as<std::string>(),
                row[7].as<std::string>());
        }

        return alerts;
    }

    void AlertService::updateSupervisoryRegistration(pqxx::connection &connection_ptr, const std::string &uuid)
    {
        pqxx::nontransaction W(connection_ptr);
        
        std::string check_query = "SELECT COUNT(*) FROM alerts WHERE uuid = '" + W.esc(uuid) + "';";
        pqxx::result R = W.exec(check_query);

        if (!R.empty() && R[0][0].as<int>() > 0)
        {
            std::string update_query = "UPDATE alerts SET supervisory_registration = 'true' WHERE uuid = '" + W.esc(uuid) + "';";
            W.exec(update_query);
            W.commit();
        }
        else
        {
            W.abort();
        }
    }

    Alert AlertService::getById(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, uuid, active, error_code, data, message, timestamp, supervisory_registration FROM alerts WHERE id = " + std::to_string(id) + ";";
        pqxx::result R(N.exec(query));

        if (R.size() != 1)
        {
            throw std::runtime_error("Alert not found");
        }

        auto row = R[0];
        return Alert(row[0].as<int>(), row[1].as<std::string>(), row[2].as<std::string>(), row[3].as<std::string>(), row[4].as<std::string>(), row[5].as<std::string>(), row[6].as<std::string>(), row[7].as<std::string>());
    }

    void AlertService::update(pqxx::connection &connection_ptr, const Alert &Alert)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "UPDATE alerts SET uuid = '" + W.esc(Alert.uuid) + "', active = '" + W.esc(Alert.active) + "', error_code = '" + W.esc(Alert.error_code) + "', data = '" + W.esc(Alert.data) + "', message = '" + W.esc(Alert.message) + "', timestamp = '" + W.esc(Alert.timestamp) + "', supervisory_registration = '" + W.esc(Alert.supervisory_registration) + "' WHERE id = " + std::to_string(Alert.id) + ";";
        W.exec(query);
        W.commit();
    }

    int32_t AlertService::getLastId(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction W(connection_ptr);
        int32_t last_id = 0;
        std::string query = "SELECT COALESCE(MAX(id), 0) AS last_id FROM alerts;";
        pqxx::result R = W.exec(query);

        if (!R.empty())
        {
            last_id = R[0][0].as<int32_t>();
        }

        W.commit();
        return last_id;
    }

    int32_t AlertService::getRecordCount(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction W(connection_ptr);
        int32_t record_count = 0;
        std::string query = "SELECT COUNT(*) AS record_count FROM alerts;";
        pqxx::result R = W.exec(query);

        if (!R.empty())
        {
            record_count = R[0][0].as<int32_t>();
        }

        W.commit();
        return record_count;
    }

    void AlertService::removeFirstRecord(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction W(connection_ptr);

        std::string query_select = "SELECT id FROM alerts ORDER BY id ASC LIMIT 1;";
        pqxx::result R = W.exec(query_select);

        if (!R.empty())
        {
            int32_t first_id = R[0][0].as<int32_t>();
            std::string query_delete = "DELETE FROM alerts WHERE id = " + std::to_string(first_id) + ";";
            W.exec(query_delete);
        }

        W.commit();
    }

    void AlertService::remove(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "DELETE FROM alerts WHERE id = " + std::to_string(id) + ";";
        W.exec(query);
        W.commit();
    }
}

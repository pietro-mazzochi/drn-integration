#include <drn_integration_communication/services/api/settings_service.hpp>
#include <stdexcept>

namespace drn_integration_communication
{
    void SettingService::create(pqxx::connection &connection_ptr, const Setting &Setting)
    {
        printf("banco de dados");
        pqxx::nontransaction W(connection_ptr);
        std::string query = "INSERT INTO settings (key, value) VALUES ('" + W.esc(Setting.key) + "', '" + W.esc(Setting.value) + "');";
        W.exec(query);
        W.commit();
    }

    std::vector<Setting> SettingService::getAll(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, key, value FROM settings";
        pqxx::result R(N.exec(query));

        std::vector<Setting> settings;

        for (const auto &row : R)
        {
            settings.emplace_back(
                row["id"].as<int>(),           
                row["key"].as<std::string>(),  
                row["value"].as<std::string>() 
            );
        }

        return settings;
    }

    Setting SettingService::getById(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, key, value FROM Setting WHERE id = " + std::to_string(id) + ";";
        pqxx::result R(N.exec(query));

        if (R.size() != 1)
        {
            throw std::runtime_error("settings not found");
        }

        auto row = R[0];
        return Setting(row[0].as<int>(), row[1].as<std::string>(), row[2].as<std::string>());
    }

    void SettingService::update(pqxx::connection &connection_ptr, const Setting &Setting)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query =
            "INSERT INTO settings (id, key, value) "
            "VALUES (" +
            std::to_string(Setting.id) + ", '" + W.esc(Setting.key) + "', '" + W.esc(Setting.value) + "') "
                                                                                                      "ON CONFLICT (id) DO UPDATE SET "
                                                                                                      "key = EXCLUDED.key, value = EXCLUDED.value;";

        W.exec(query);
        W.commit();
    }

    void SettingService::remove(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "DELETE FROM settings WHERE id = " + std::to_string(id) + ";";
        W.exec(query);
        W.commit();
    }
}
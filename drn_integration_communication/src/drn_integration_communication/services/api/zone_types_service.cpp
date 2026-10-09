#include <drn_integration_communication/services/api/zone_types_service.hpp>
#include <stdexcept>

namespace drn_integration_communication
{
    void ZoneTypeService::create(pqxx::connection &connection_ptr, const ZoneType &ZoneType)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "INSERT INTO zone_types (name) VALUES ('" + W.esc(ZoneType.name) + "');";
        W.exec(query);
        W.commit();
    }

    std::vector<ZoneType> ZoneTypeService::getAll(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, name FROM zone_types;";
        pqxx::result R(N.exec(query));

        std::vector<ZoneType> zoneTypes;

        for (const auto &row : R)
        {
            zoneTypes.emplace_back(
                row[0].as<int>(),       
                row[1].as<std::string>()
            );
        }

        return zoneTypes;
    }

    ZoneType ZoneTypeService::getById(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, name FROM zone_types WHERE id = " + std::to_string(id) + ";";
        pqxx::result R(N.exec(query));

        if (R.size() != 1)
        {
            throw std::runtime_error("ZoneType not found");
        }

        auto row = R[0];
        return ZoneType(row[0].as<int>(), row[1].as<std::string>());
    }

    void ZoneTypeService::update(pqxx::connection &connection_ptr, const ZoneType &ZoneType)
    {
        pqxx::nontransaction W(connection_ptr);

        std::string query =
            "INSERT INTO zone_types (id, name) "
            "VALUES (" +
            std::to_string(ZoneType.id) + ", '" + W.esc(ZoneType.name) + "') "
                                                                         "ON CONFLICT (id) DO UPDATE SET "
                                                                         "name = EXCLUDED.name;";
        W.exec(query);
        W.commit();
    }

    void ZoneTypeService::remove(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "DELETE FROM zone_types WHERE id = " + std::to_string(id) + ";";
        W.exec(query);
        W.commit();
    }
}
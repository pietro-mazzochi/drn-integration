#include <drn_integration_communication/services/api/anchors_service.hpp>
#include <stdexcept>

namespace drn_integration_communication
{
    void AnchorService::create(pqxx::connection &connection_ptr, const Anchor &Anchor)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "INSERT INTO anchors (name, pos_x, pos_y, angle) VALUES ('" + W.esc(Anchor.name) + "', '" + W.esc(Anchor.pos_x) + "', '" + W.esc(Anchor.pos_y) + "', '" + W.esc(Anchor.angle) + "');";
        W.exec(query);
        W.commit();
    }

    std::vector<Anchor> AnchorService::getAll(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, name, pos_x, pos_y, angle FROM anchors;";
        pqxx::result R(N.exec(query));

        std::vector<Anchor> anchors;

        for (const auto &row : R)
        {
            anchors.emplace_back(
                row[0].as<int>(),         
                row[1].as<std::string>(), 
                row[2].as<std::string>(), 
                row[3].as<std::string>(), 
                row[4].as<std::string>() 
            );
        }

        return anchors;
    }

    Anchor AnchorService::getById(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, name, pos_x, pos_y, angle FROM anchors WHERE id = " + std::to_string(id) + ";";
        pqxx::result R(N.exec(query));

        if (R.size() != 1)
        {
            throw std::runtime_error("anchors not found");
        }

        auto row = R[0];
        return Anchor(row[0].as<int>(), row[1].as<std::string>(), row[2].as<std::string>(), row[3].as<std::string>(), row[4].as<std::string>());
    }

    void AnchorService::update(pqxx::connection &connection_ptr, const Anchor &Anchor)
    {
        pqxx::nontransaction W(connection_ptr);
        // std::string query = "UPDATE anchors SET name = '" + W.esc(Anchor.name) + "', pos_x = '" + W.esc(Anchor.pos_x) + "', pos_y = '" + W.esc(Anchor.pos_y) + "', angle = '" + W.esc(Anchor.angle) + "' WHERE id = " + std::to_string(Anchor.id) + ";";

        std::string query =
            "INSERT INTO anchors (id, name, pos_x, pos_y, angle) "
            "VALUES (" +
            std::to_string(Anchor.id) + ", '" + W.esc(Anchor.name) + "', '" + W.esc(Anchor.pos_x) + "', '" + W.esc(Anchor.pos_y) + "', '" + W.esc(Anchor.angle) + "') "
                                                                                                                                                                  "ON CONFLICT (id) DO UPDATE SET "
                                                                                                                                                                  "name = EXCLUDED.name, pos_x = EXCLUDED.pos_x, pos_y = EXCLUDED.pos_y, angle = EXCLUDED.angle;";

        W.exec(query);
        W.commit();
    }

    void AnchorService::remove(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "DELETE FROM anchors WHERE id = " + std::to_string(id) + ";";
        W.exec(query);
        W.commit();
    }
}
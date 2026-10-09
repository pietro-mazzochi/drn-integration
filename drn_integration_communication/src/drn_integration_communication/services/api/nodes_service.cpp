#include <drn_integration_communication/services/api/nodes_service.hpp>
#include <stdexcept>

namespace drn_integration_communication
{
    void NodeService::create(pqxx::connection &connection_ptr, const Node &Node)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "INSERT INTO nodes (pos_x, pos_y, angle, deleted_supervisory, map_id, node_type, direction, label_id) VALUES ('" +
                            W.esc(Node.pos_x) + "', '" +
                            W.esc(Node.pos_y) + "', '" +
                            W.esc(Node.angle) + "', '" +
                            W.esc(Node.deleted_supervisory) + "', " +
                            std::to_string(Node.map_id) + ", " +
                            std::to_string(Node.node_type) + ", '" +
                            W.esc(Node.direction) + "', '" +
                            W.esc(Node.label_id) + "');";
        W.exec(query);
        W.commit();
    }

    std::vector<Node> NodeService::getAll(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, pos_x, pos_y, angle, deleted_supervisory, map_id, node_type, direction, label_id FROM nodes;";
        pqxx::result R(N.exec(query));

        std::vector<Node> nodes;

        for (const auto &row : R)
        {
            nodes.emplace_back(
                row["id"].as<int>(),
                row["pos_x"].as<std::string>(),
                row["pos_y"].as<std::string>(),
                row["angle"].as<std::string>(),
                row["deleted_supervisory"].as<std::string>(),
                row["map_id"].as<int>(),
                row["node_type"].as<int>(),
                row["direction"].as<std::string>(),
                row["label_id"].as<std::string>());
        }

        return nodes;
    }

    void NodeService::markAllAsDeleted(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "UPDATE nodes SET deleted_supervisory = 'true';";
        W.exec(query);
        W.commit();
    }

    Node NodeService::getById(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, pos_x, pos_y, angle, deleted_supervisory, map_id, node_type, direction, label_id FROM nodes WHERE id = " + std::to_string(id) + ";";
        pqxx::result R(N.exec(query));

        if (R.size() != 1)
        {
            throw std::runtime_error("Node not found");
        }

        auto row = R[0];
        return Node(
            row["id"].as<int>(),
            row["pos_x"].as<std::string>(),
            row["pos_y"].as<std::string>(),
            row["angle"].as<std::string>(),
            row["deleted_supervisory"].as<std::string>(),
            row["map_id"].as<int>(),
            row["node_type"].as<int>(),
            row["direction"].as<std::string>(),
            row["label_id"].as<std::string>());
    }

    void NodeService::update(pqxx::connection &connection_ptr, const Node &Node)
    {
        pqxx::nontransaction W(connection_ptr);

        std::string query =
            "INSERT INTO nodes (id, pos_x, pos_y, angle, deleted_supervisory, map_id, node_type, direction, label_id) "
            "VALUES (" +
            std::to_string(Node.id) + ", '" +
            W.esc(Node.pos_x) + "', '" +
            W.esc(Node.pos_y) + "', '" +
            W.esc(Node.angle) + "', '" +
            W.esc(Node.deleted_supervisory) + "', " +
            std::to_string(Node.map_id) + ", " +
            std::to_string(Node.node_type) + ", '" +
            W.esc(Node.direction) + "', '" +
            W.esc(Node.label_id) + "') "
                                   "ON CONFLICT (id) DO UPDATE SET "
                                   "pos_x = EXCLUDED.pos_x, "
                                   "pos_y = EXCLUDED.pos_y, "
                                   "angle = EXCLUDED.angle, "
                                   "deleted_supervisory = EXCLUDED.deleted_supervisory, "
                                   "map_id = EXCLUDED.map_id, "
                                   "node_type = EXCLUDED.node_type, "
                                   "direction = EXCLUDED.direction, "
                                   "label_id = EXCLUDED.label_id;";

        W.exec(query);
        W.commit();
    }

    void NodeService::remove(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "DELETE FROM nodes WHERE id = " + std::to_string(id) + ";";
        W.exec(query);
        W.commit();
    }
}

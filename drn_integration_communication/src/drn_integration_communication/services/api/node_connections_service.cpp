#include <drn_integration_communication/services/api/node_connections_service.hpp>
#include <stdexcept>

namespace drn_integration_communication
{
    void NodeConnectionService::create(pqxx::connection &connection_ptr, const NodeConnection &NodeConnection)
    {
        pqxx::nontransaction W(connection_ptr);
        nlohmann::json line_json = NodeConnection.line;
        std::string line_str = line_json.dump();
        std::string query = "INSERT INTO node_connections (line, source_node_id, target_node_id) VALUES ('" + W.esc(line_str) + "', '" + W.esc(std::to_string(NodeConnection.source_node_id)) + "', '" + W.esc(std::to_string(NodeConnection.target_node_id)) + "');";
        W.exec(query);
        W.commit();
    }

    std::vector<NodeConnection> NodeConnectionService::getAll(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, line, source_node_id, target_node_id FROM node_connections;";
        pqxx::result R(N.exec(query));

        std::vector<NodeConnection> nodeConnections;

        for (const auto &row : R)
        {
            std::string line_str = row["line"].as<std::string>();
            nlohmann::json line_json = nlohmann::json::parse(line_str);

            std::vector<std::vector<double>> line;
            for (const auto &coord_pair : line_json)
            {
                std::vector<double> coordinates;
                coordinates.push_back(coord_pair[0].get<double>());
                coordinates.push_back(coord_pair[1].get<double>());
                line.push_back(coordinates);
            }

            nodeConnections.emplace_back(
                row["id"].as<int>(),             
                line,                           
                row["source_node_id"].as<int>(),
                row["target_node_id"].as<int>()  
            );
        }

        return nodeConnections;
    }

    NodeConnection NodeConnectionService::getById(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, line, source_node_id, target_node_id FROM node_connections WHERE id = " + std::to_string(id) + ";";
        pqxx::result R(N.exec(query));

        if (R.size() != 1)
        {
            throw std::runtime_error("node_connections not found");
        }

        auto row = R[0];

        std::string line_str = row["line"].as<std::string>();
        nlohmann::json line_json = nlohmann::json::parse(line_str);

        std::vector<std::vector<double>> line;
        for (const auto &coord_pair : line_json)
        {
            std::vector<double> coordinates;
            coordinates.push_back(coord_pair[0].get<double>());
            coordinates.push_back(coord_pair[1].get<double>());
            line.push_back(coordinates);
        }

        return NodeConnection(row["id"].as<int>(), line, row["source_node_id"].as<int>(), row["target_node_id"].as<int>());
    }

    void NodeConnectionService::update(pqxx::connection &connection_ptr, const NodeConnection &NodeConnection)
    {
        pqxx::nontransaction W(connection_ptr);

        nlohmann::json line_json = NodeConnection.line;
        std::string line_str = line_json.dump();

        std::string query =
            "INSERT INTO node_connections (id, line, source_node_id, target_node_id) "
            "VALUES (" +
            std::to_string(NodeConnection.id) + ", '" + W.esc(line_str) + "', '" + W.esc(std::to_string(NodeConnection.source_node_id)) + "', '" + W.esc(std::to_string(NodeConnection.target_node_id)) + "') "
                                                                                                                                                                                                          "ON CONFLICT (id) DO UPDATE SET "
                                                                                                                                                                                                          "line = EXCLUDED.line, source_node_id = EXCLUDED.source_node_id, target_node_id = EXCLUDED.target_node_id;";
        W.exec(query);
        W.commit();
    }

    void NodeConnectionService::remove(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "DELETE FROM node_connections WHERE id = " + std::to_string(id) + ";";
        W.exec(query);
        W.commit();
    }
}
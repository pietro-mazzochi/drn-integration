#include <drn_integration_communication/services/api/node_types_service.hpp>
#include <stdexcept>

namespace drn_integration_communication
{
    void NodeTypeService::create(pqxx::connection &connection_ptr, const NodeType &NodeType)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "INSERT INTO node_types (name) VALUES ('" + W.esc(NodeType.name) + "');";
        W.exec(query);
        W.commit();
    }

    std::vector<NodeType> NodeTypeService::getAll(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, name FROM node_types;";
        pqxx::result R(N.exec(query));

        std::vector<NodeType> nodeTypes;

        for (const auto &row : R)
        {
            nodeTypes.emplace_back(
                row["id"].as<int>(),          
                row["name"].as<std::string>() 
            );
        }

        return nodeTypes;
    }

    NodeType NodeTypeService::getById(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, name FROM node_types WHERE id = " + std::to_string(id) + ";";
        pqxx::result R(N.exec(query));

        if (R.size() != 1)
        {
            throw std::runtime_error("node_types not found");
        }

        auto row = R[0];
        return NodeType(row[0].as<int>(), row[1].as<std::string>());
    }

    void NodeTypeService::update(pqxx::connection &connection_ptr, const NodeType &NodeType)
    {
        pqxx::nontransaction W(connection_ptr);
        // std::string query = "UPDATE node_types SET name = '" + W.esc(NodeType.name) + "' WHERE id = " + std::to_string(NodeType.id) + ";";
        std::string query =
            "INSERT INTO node_types (id, name) "
            "VALUES (" +
            std::to_string(NodeType.id) + ", '" + W.esc(NodeType.name) + "') "
                                                                         "ON CONFLICT (id) DO UPDATE SET "
                                                                         "name = EXCLUDED.name;";
        W.exec(query);
        W.commit();
    }

    void NodeTypeService::remove(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "DELETE FROM node_types WHERE id = " + std::to_string(id) + ";";
        W.exec(query);
        W.commit();
    }
}
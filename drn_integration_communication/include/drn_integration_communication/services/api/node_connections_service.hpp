#ifndef DRN_INTEGRATION_COMUNICATION_NODECONNECTIONSERVICE_H
#define DRN_INTEGRATION_COMUNICATION_NODECONNECTIONSERVICE_H

#include <drn_integration_communication/entities/api/node_connections.hpp>
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>

namespace drn_integration_communication
{
    class NodeConnectionService 
    {
        public:
            static void create(pqxx::connection &connection_ptr, const NodeConnection& nodeConnection);
            static std::vector<NodeConnection> getAll(pqxx::connection &connection_ptr);
            static NodeConnection getById(pqxx::connection &connection_ptr, int id);
            static void update(pqxx::connection &connection_ptr, const NodeConnection& nodeConnection);
            static void remove(pqxx::connection &connection_ptr, int id);
    };
}
#endif
#ifndef DRN_INTEGRATION_COMUNICATION_NODESERVICE_H
#define DRN_INTEGRATION_COMUNICATION_NODESERVICE_H

#include <drn_integration_communication/entities/api/nodes.hpp>
#include <pqxx/pqxx>

namespace drn_integration_communication
{
    class NodeService 
    {
        public:
            static void create(pqxx::connection &connection_ptr, const Node& node);
            static std::vector<Node> getAll(pqxx::connection &connection_ptr);
            static Node getById(pqxx::connection &connection_ptr, int id);
            static void update(pqxx::connection &connection_ptr, const Node& node);
            static void remove(pqxx::connection &connection_ptr, int id);
            static void markAllAsDeleted(pqxx::connection &connection_ptr);
    };
}
#endif
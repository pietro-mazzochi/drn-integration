#ifndef DRN_INTEGRATION_COMUNICATION_NODETYPESERVICE_H
#define DRN_INTEGRATION_COMUNICATION_NODETYPESERVICE_H

#include <drn_integration_communication/entities/api/node_types.hpp>
#include <pqxx/pqxx>

namespace drn_integration_communication
{
    class NodeTypeService 
    {
        public:
            static void create(pqxx::connection &connection_ptr, const NodeType& nodeType);
            static std::vector<NodeType> getAll(pqxx::connection &connection_ptr);
            static NodeType getById(pqxx::connection &connection_ptr, int id);
            static void update(pqxx::connection &connection_ptr, const NodeType& nodeType);
            static void remove(pqxx::connection &connection_ptr, int id);
    };
}
#endif
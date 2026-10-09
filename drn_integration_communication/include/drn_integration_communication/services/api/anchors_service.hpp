#ifndef DRN_INTEGRATION_COMUNICATION_ANCHORSERVICE_H
#define DRN_INTEGRATION_COMUNICATION_ANCHORSERVICE_H

#include <drn_integration_communication/entities/api/anchors.hpp>
#include <pqxx/pqxx>

namespace drn_integration_communication
{
    class AnchorService 
    {
        public:
            static void create(pqxx::connection &connection_ptr, const Anchor& anchor);
            static std::vector<Anchor> getAll(pqxx::connection &connection_ptr);
            static Anchor getById(pqxx::connection &connection_ptr, int id);
            static void update(pqxx::connection &connection_ptr, const Anchor& anchor);
            static void remove(pqxx::connection &connection_ptr, int id);
    };
}
#endif
#ifndef DRN_INTEGRATION_COMUNICATION_ALERTSERVICE_H
#define DRN_INTEGRATION_COMUNICATION_ALERTSERVICE_H

#include <drn_integration_communication/entities/api/alerts.hpp>
#include <pqxx/pqxx>

namespace drn_integration_communication
{
    class AlertService 
    {
        public:
            static void create(pqxx::connection &connection_ptr, const Alert& alert);
            static std::vector<Alert> getAll(pqxx::connection &connection_ptr);
            static Alert getById(pqxx::connection &connection_ptr, int id);
            static void update(pqxx::connection &connection_ptr, const Alert& alert);
            static int32_t getLastId(pqxx::connection &connection_ptr);
            static void removeFirstRecord(pqxx::connection &connection_ptr);
            static int32_t getRecordCount(pqxx::connection &connection_ptr);
            static void remove(pqxx::connection &connection_ptr, int id);
            static void updateSupervisoryRegistration(pqxx::connection &connection_ptr, const std::string &uuid);
    };
}
#endif
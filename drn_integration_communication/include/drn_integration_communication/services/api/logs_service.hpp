#ifndef DRN_INTEGRATION_COMUNICATION_LOGSSERVICE_H
#define DRN_INTEGRATION_COMUNICATION_LOGSSERVICE_H

#include <drn_integration_communication/entities/api/logs.hpp>
#include <pqxx/pqxx>

namespace drn_integration_communication
{
    class LogService 
    {
        public:
            static void create(pqxx::connection &connection_ptr, const Log& log);
            static void robot(pqxx::connection &connection_ptr, const Log &log);
            static std::vector<Log> getAll(pqxx::connection &connection_ptr);
            static Log getById(pqxx::connection &connection_ptr, int id);
            static void update(pqxx::connection &connection_ptr, const Log& log);
            static int32_t getLastId(pqxx::connection &connection_ptr);
            static void removeFirstRecord(pqxx::connection &connection_ptr);
            static void remove(pqxx::connection &connection_ptr, int id);
            static int32_t getRecordCount(pqxx::connection &connection_ptr);
            bool existId(pqxx::connection &connection_ptr, int id);
            static void updateSupervisoryRegistration(pqxx::connection &connection_ptr, const std::string &uuid);
    };
}
#endif
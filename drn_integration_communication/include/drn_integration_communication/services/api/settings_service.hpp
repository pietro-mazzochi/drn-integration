#ifndef DRN_INTEGRATION_COMUNICATION_SETTINGSERVICE_H
#define DRN_INTEGRATION_COMUNICATION_SETTINGSERVICE_H

#include <drn_integration_communication/entities/api/settings.hpp>
#include <pqxx/pqxx>

namespace drn_integration_communication
{
    class SettingService 
    {
        public:
            static void create(pqxx::connection &connection_ptr, const Setting& setting);
            static std::vector<Setting> getAll(pqxx::connection &connection_ptr);
            static Setting getById(pqxx::connection &connection_ptr, int id);
            static void update(pqxx::connection &connection_ptr, const Setting& setting);
            static void remove(pqxx::connection &connection_ptr, int id);
    };
}
#endif
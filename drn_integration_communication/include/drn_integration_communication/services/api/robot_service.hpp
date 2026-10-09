#ifndef DRN_INTEGRATION_COMUNICATION_ROBOTSERVICE_H
#define DRN_INTEGRATION_COMUNICATION_ROBOTSERVICE_H

#include <drn_integration_communication/entities/api/robot.hpp>
#include <pqxx/pqxx>

namespace drn_integration_communication
{
    class RobotService 
    {
        public:
            static void create(pqxx::connection &connection_ptr, const Robot& robot);
            static void robot(pqxx::connection &connection_ptr, const Robot &robot);
            static std::vector<Robot> getAll(pqxx::connection &connection_ptr);
            static Robot getById(pqxx::connection &connection_ptr, int id);
            static void update(pqxx::connection &connection_ptr, const Robot& robot);
            static void remove(pqxx::connection &connection_ptr, int id);
            bool existId(pqxx::connection &connection_ptr, int id);
    };
}
#endif
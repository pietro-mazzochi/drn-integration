#ifndef DRN_INTEGRATION_COMUNICATION_DATABASEMIGRATION_H
#define DRN_INTEGRATION_COMUNICATION_DATABASEMIGRATION_H

#include <stdio.h>
#include <iostream>
#include <pqxx/pqxx>

namespace drn_integration_communication
{
    class DatabasemMigration
    {
        public:
            void createTablesIfNotExist(pqxx::connection &connection_ptr);
    };        
}
#endif
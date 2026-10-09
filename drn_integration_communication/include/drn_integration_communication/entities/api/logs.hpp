#ifndef DRN_INTEGRATION_COMUNICATION_LOGS_H
#define DRN_INTEGRATION_COMUNICATION_LOGS_H

#include <stdio.h>
#include <numeric> 
#include <string>

namespace drn_integration_communication
{
    class Log
    {
        public:
            int id;            
            std::string uuid;
            std::string data;
            std::string timestamp;
            std::string supervisory_registration;

            Log(int id, const std::string& uuid, const std::string& data, const std::string& timestamp, const std::string& supervisory_registration);
    };        
}
#endif
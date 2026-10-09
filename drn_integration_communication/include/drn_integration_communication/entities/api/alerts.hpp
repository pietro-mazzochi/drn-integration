#ifndef DRN_INTEGRATION_COMUNICATION_ALERTS_H
#define DRN_INTEGRATION_COMUNICATION_ALERTS_H

#include <stdio.h>
#include <numeric> 
#include <string>

namespace drn_integration_communication
{
    class Alert
    {
        public:
            int id;
            std::string uuid;
            std::string active;
            std::string error_code;
            std::string data;
            std::string message;
            std::string timestamp;
            std::string supervisory_registration;

            Alert(int id, const std::string& uuid, const std::string& active, const std::string& error_code, const std::string& data, const std::string& message, const std::string& timestamp, const std::string& supervisory_registration);
    };        
}
#endif
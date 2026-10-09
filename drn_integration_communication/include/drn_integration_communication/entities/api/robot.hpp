#ifndef DRN_INTEGRATION_COMUNICATION_ROBOT_H
#define DRN_INTEGRATION_COMUNICATION_ROBOT_H

#include <stdio.h>
#include <numeric> 
#include <string>

namespace drn_integration_communication
{
    class Robot
    {
        public:
            int id;            
            std::string uuid;
            std::string data;
            std::string timestamp;

            Robot(int id, const std::string& uuid, const std::string& data, const std::string& timestamp);
    };        
}
#endif
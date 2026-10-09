#ifndef DRN_INTEGRATION_COMUNICATION_TELEMETRIES_H
#define DRN_INTEGRATION_COMUNICATION_TELEMETRIES_H

#include <stdio.h>
#include <numeric> 
#include <string>

namespace drn_integration_communication
{
    class Telemetrie
    {
        public:
            int id;
            std::string data;
            std::string pos_x;
            std::string pos_y;
            std::string angle;
            std::string battery;
            std::string tension;
            std::string velocity;            

            Telemetrie(int id, const std::string& data, const std::string& pos_x, const std::string& pos_y, const std::string& angle, const std::string& battery, const std::string& tension, const std::string& velocity);
    };        
}
#endif
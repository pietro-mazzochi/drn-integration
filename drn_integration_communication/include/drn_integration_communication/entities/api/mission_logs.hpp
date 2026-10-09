#ifndef DRN_INTEGRATION_COMUNICATION_MISSIONLOGS_H
#define DRN_INTEGRATION_COMUNICATION_MISSIONLOGS_H

#include <stdio.h>
#include <numeric> 
#include <string>

namespace drn_integration_communication
{
    class MissionLog
    {
        public:
            int id;
            std::string uuid;
            std::string status;
            std::string data;
            std::string timestamp;
            std::string supervisory_registration;
            int mission_id;

            MissionLog(int id, const std::string& uuid, const std::string& status, const std::string& data, const std::string& timestamp, const std::string& supervisory_registration, int mission_id);
    };        
}
#endif
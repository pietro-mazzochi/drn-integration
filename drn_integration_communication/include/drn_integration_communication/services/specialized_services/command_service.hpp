#ifndef DRN_INTEGRATION_COMUNICATION_COMMAND_H
#define DRN_INTEGRATION_COMUNICATION_COMMAND_H

#include <stdio.h>
#include <vector>
#include <std_msgs/msg/string.hpp>
#include <std_msgs/msg/int32.hpp>
#include <functional>
#include <numeric>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <drn_integration_communication/database/database.hpp>
#include <drn_integration_communication/communication/communication.hpp>

namespace drn_integration_communication
{
    class Command
    {
    public:
        struct Result
        {
            std::string code;
            bool status;
        };

        Result command(Database &database, u_int64_t code_ref, std::string &msg);
        void StartMappgin();
        void PauseMapping();
        void StartMission();
        void CancelMission();
        void Heartbit();
    };
}
#endif
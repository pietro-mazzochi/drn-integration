#ifndef DRN_INTEGRATION_COMUNICATION_COMUNICATION_H
#define DRN_INTEGRATION_COMUNICATION_COMUNICATION_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <pqxx/pqxx>

namespace navigation
{
    constexpr int finishMappingAndSaveMap = 0;
    constexpr int startMapping = 1;
    constexpr int cancelMapping = 2;
    constexpr int startCostmap = 3;
    constexpr int stopCostmap = 4;
    constexpr int restartCostmap = 5;
}

namespace map
{
    constexpr int blockedAreas = 0;
    constexpr int unknownAreas = 205;
    constexpr int freeAreas = 255;
    constexpr int sendMapping = 2;
    constexpr int sendSavedMap = 1;
    constexpr int notSendMap = 0;
}

namespace supervisory_command
{
    const std::string stopMapping = "0xC7";
    const std::string startMapping = "0xC8";
    const std::string pauseMapping = "0xC9";
    const std::string cancelMapping = "0xD3";
    const std::string startMission = "0xCA";
    const std::string pauseMission = "0xD2";
    const std::string continueMission = "0xD1";
    const std::string cancelMission = "0xCB";
    const std::string settings = "0xCC";
    const std::string heartbit = "0xCD";
    const std::string vehicleRegistration = "0xCF";
    const std::string mapUpdate = "0xCE";
    const std::string initialPose = "0xD4";
    const std::string finishMission = "0xD5";
    const std::string registeredLog = "0xD6";
    const std::string registeredAlert = "0xD7";
    const std::string registeredMissionLog = "0xD8";
    const std::string initialParam = "0x00";
}

namespace vehicles
{
    constexpr int notRegistered = 0;
    constexpr int confirmationRegistration = 1;
    constexpr int registered = 2;
    constexpr int stopRegistration = 3;
}

namespace plc_command
{
    const std::string shutdown = "shutdown";
    const std::string startMission = "mission_start";
    const std::string cancelMission = "mission_cancel";
    constexpr int shutdownResponse = 112;
}

namespace mission_status_plc
{
    constexpr int noMission = 0;
    constexpr int running = 1;
    constexpr int error = 2;
    constexpr int pause = 3;
    constexpr int cancel = 4;
    constexpr int finish = 5;
}

namespace message_code_plc
{
    constexpr int waiting = 0;
    constexpr int running = 1;    
    constexpr int pause = 210;
    constexpr int cancel = 203;
    constexpr int finish = 213;
}

namespace robot_type_plc
{
    constexpr int info = 0;
    constexpr int warning = 1;
    constexpr int error = 2;
    constexpr int fatalError = 2;
}

#endif
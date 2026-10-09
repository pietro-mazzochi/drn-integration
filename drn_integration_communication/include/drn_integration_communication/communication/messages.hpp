#ifndef DRN_INTEGRATION_COMUNICATION_MESSAGES_H
#define DRN_INTEGRATION_COMUNICATION_MESSAGES_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <pqxx/pqxx>

namespace mqtt
{
    const std::string msnConnect = "Attempting to connect to MQTT broker...";
    const std::string msnSuccessfully = "Connected to MQTT broker successfully";
    const std::string msnReconnect = "MQTT broker disconnected! Trying to reconnect..."; 
}

namespace message_plc
{
    const std::string msnWaiting = "Aguardando!";
    const std::string msnExecutingMission = "Executando missao!";
    const std::string msnObstacleDetected = "Obstaculo detectado!";
    const std::string msnCancel = "Missao cancelada!";
    const std::string msnPause = "Missao pausada!";
    const std::string msnFinish = "Missao finalizada!";
}


#endif
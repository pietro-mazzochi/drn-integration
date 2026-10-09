#include <drn_integration_communication/services/specialized_services/command_service.hpp>

namespace drn_integration_communication
{
    Command::Result Command::command(Database &database, u_int64_t CODE_REF, std::string &msg)
    {
        std::string msn = "command";
        Result result;
        if (CODE_REF == 0)
        {
            std::string response = database.vehicleFindOne(1);

            if (response != "0")
            {
                result.code = response;
                result.status = true;

                return result;
            }
            else
            {
                result.code = "0";
                result.status = false;

                return result;
            }
        }
        else
        {
            nlohmann::json json_message = nlohmann::json::parse(msg);
            if (json_message.contains("code") && json_message["code"].is_number() && json_message.contains("command") && json_message["command"].is_string())
            {
                u_int64_t code = json_message["code"];
                std::string command_str = json_message["command"];

                if (code == CODE_REF)
                {
                    if (command_str == supervisory_command::startMapping)
                    {
                        result.code = command_str;
                        result.status = true;
                        return result;
                    }
                    else if (command_str == supervisory_command::stopMapping)
                    {
                        result.code = command_str;
                        result.status = true;
                        return result;
                    }
                    else if (command_str == supervisory_command::pauseMapping)
                    {
                        result.code = command_str;
                        result.status = true;
                        return result;
                    }
                    else if (command_str == supervisory_command::startMission)
                    {
                        result.code = command_str;
                        result.status = true;
                        return result;
                    }
                    else if (command_str == supervisory_command::cancelMission)
                    {
                        result.code = command_str;
                        result.status = true;
                        return result;
                    }
                    else if (command_str == supervisory_command::pauseMission)
                    {
                        result.code = command_str;
                        result.status = true;
                        return result;
                    }
                    else if (command_str == supervisory_command::continueMission)
                    {
                        result.code = command_str;
                        result.status = true;
                        return result;
                    }
                    else if (command_str == supervisory_command::cancelMapping)
                    {
                        result.code = command_str;
                        result.status = true;
                        return result;
                    }
                    else if (command_str == supervisory_command::heartbit)
                    {
                        result.code = command_str;
                        result.status = true;
                        return result;
                    }
                    else if (command_str == supervisory_command::initialPose)
                    {
                        result.code = command_str;
                        result.status = true;
                        return result;
                    }
                    else if (command_str == supervisory_command::registeredLog)
                    {
                        if (json_message.contains("data"))
                        {
                            auto log = json_message["data"];

                            if (log.contains("uuid"))
                            {
                                std::string uuid = log["uuid"];
                                database.updateLogSupervisory(uuid);
                            }
                        }
                    }
                    else if (command_str == supervisory_command::registeredAlert)
                    {
                        if (json_message.contains("data"))
                        {
                            auto alert = json_message["data"];

                            if (alert.contains("uuid"))
                            {
                                std::string uuid = alert["uuid"];
                                database.updateAlertSupervisory(uuid);
                            }
                        }
                    }
                    else if (command_str == supervisory_command::registeredMissionLog)
                    {
                        if (json_message.contains("data"))
                        {
                            auto missionLog = json_message["data"];

                            if (missionLog.contains("uuid"))
                            {
                                std::string uuid = missionLog["uuid"];
                                database.updateMissionLogSupervisory(uuid);
                            }
                        }
                    }
                    else if (command_str == supervisory_command::mapUpdate || command_str == supervisory_command::settings)
                    {
                        if (json_message.contains("data"))
                        {
                            bool status = database.create(json_message);

                            result.code = command_str;
                            result.status = status;
                            return result;
                        }
                        else
                        {
                            result.code = command_str;
                            result.status = false;
                            return result;
                        }
                    }
                    else if (command_str == supervisory_command::vehicleRegistration)
                    {
                        if (json_message.contains("data"))
                        {
                            auto vehicle = json_message["data"];

                            if (vehicle.contains("vehicle"))
                            {
                                std::string response = vehicle["vehicle"];

                                if (response == "ok")
                                {
                                    database.create(json_message);

                                    result.code = command_str;
                                    result.status = true;
                                    return result;
                                }
                                else
                                {
                                    result.code = command_str;
                                    result.status = false;
                                    return result;
                                }
                            }
                        }
                        else
                        {
                            result.code = command_str;
                            result.status = false;
                            return result;
                        }
                    }
                    else
                    {
                        result.code = command_str;
                        result.status = false;
                        return result;
                    }
                }
            }
        }
        result.code = "0";
        result.status = false;
        return result;
    }
}
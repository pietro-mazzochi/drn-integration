#ifndef DRN_INTEGRATION_COMUNICATION_DATABASE_H
#define DRN_INTEGRATION_COMUNICATION_DATABASE_H

#include <stdio.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <pqxx/pqxx>
#include <opencv2/opencv.hpp>
#include <optional>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <drn_integration_communication/services/specialized_services/general_service.hpp>
#include <drn_integration_communication/entities/api/pose.hpp>
#include <drn_integration_communication/database/database_migration.hpp>
#include <drn_integration_communication/services/api/alerts_service.hpp>
#include <drn_integration_communication/entities/api/alerts.hpp>
#include <drn_integration_communication/services/api/anchors_service.hpp>
#include <drn_integration_communication/entities/api/anchors.hpp>
#include <drn_integration_communication/services/api/logs_service.hpp>
#include <drn_integration_communication/entities/api/logs.hpp>
#include <drn_integration_communication/services/api/robot_service.hpp>
#include <drn_integration_communication/entities/api/robot.hpp>
#include <drn_integration_communication/services/api/maps_service.hpp>
#include <drn_integration_communication/entities/api/maps.hpp>
#include <drn_integration_communication/services/api/missions_service.hpp>
#include <drn_integration_communication/entities/api/missions.hpp>
#include <drn_integration_communication/services/api/mission_logs_service.hpp>
#include <drn_integration_communication/entities/api/mission_logs.hpp>
#include <drn_integration_communication/services/api/node_connections_service.hpp>
#include <drn_integration_communication/entities/api/node_connections.hpp>
#include <drn_integration_communication/services/api/node_types_service.hpp>
#include <drn_integration_communication/entities/api/node_types.hpp>
#include <drn_integration_communication/services/api/nodes_service.hpp>
#include <drn_integration_communication/entities/api/nodes.hpp>
#include <drn_integration_communication/services/api/settings_service.hpp>
#include <drn_integration_communication/entities/api/settings.hpp>
#include <drn_integration_communication/services/api/telemetries_service.hpp>
#include <drn_integration_communication/entities/api/telemetries.hpp>
#include <drn_integration_communication/services/api/vehicles_service.hpp>
#include <drn_integration_communication/entities/api/vehicles.hpp>
#include <drn_integration_communication/services/api/zone_types_service.hpp>
#include <drn_integration_communication/entities/api/zone_types.hpp>
#include <drn_integration_communication/services/api/zones_service.hpp>
#include <drn_integration_communication/entities/api/zones.hpp>
#include <drn_integration_communication/services/api/pose_service.hpp>
#include <drn_integration_communication/entities/api/pose.hpp>

namespace drn_integration_communication
{
    class Database
    {
    private:
        pqxx::connection *connection_ptr;

    public:
        Database(const std::string &connection_string);
        ~Database();
        pqxx::connection &database();
        bool create(nlohmann::json json_message);
        void createLocal(std::string &table_name, std::string &table_string);
        void migration();
        void processData(std::string &key, int id, nlohmann::json data_object);
        void clearMapTables();
        bool mapsUpdate(nlohmann::json maps);
        std::string vehicleGetAll();
        std::string vehicleFindOne(int id);
        std::string alertGetAll();
        std::string missionLogGetAll();
        std::string anchortGetAll();
        std::string logGetAll();
        std::string mapGetAll();
        std::string nodeConnectionGetAll();
        std::string nodeTypeGetAll();
        std::string nodeGetAll();
        std::vector<nlohmann::json> settingGetAll();
        std::string telemetrieGetAll();
        std::string zoneTypeGetAll();
        std::string zoneGetAll();
        std::string logFindOne(int id);
        std::string robotFindOne(int id);
        nlohmann::json missionFindOne(int id);
        nlohmann::json getLastMission();
        int32_t getLastAlertId();
        void removeFirstAlertRecord();
        int32_t getLastMissionLogId();
        void removeFirstMissionLogRecord();
        int32_t getLastLogId();
        bool missionExistId(int id);
        int32_t getLastMissionId();
        void removeFirstMissionRecord();
        void removeFirstLogRecord();
        int32_t getRecordCountMissionId();
        int32_t getRecordCountAlertId();
        int32_t getRecordCountLogId();
        double settingGetValueAsDouble(const std::string &key);
        void updateAlertSupervisory(const std::string &uuid);
        void updateLogSupervisory(const std::string &uuid);
        void updateMissionLogSupervisory(const std::string &uuid);
        std::optional<Pose> getPoseById(int id);
        void createPoseWithId(Pose pose);
        int32_t getLastMapId();
    };
}
#endif
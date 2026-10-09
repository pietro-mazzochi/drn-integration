#include <drn_integration_communication/database/database.hpp>

namespace drn_integration_communication
{

    drn_integration_communication::General *general = new drn_integration_communication::General();

    Database::Database(const std::string &connection_string)
    {
        std::string conn_str = connection_string.empty() ? "dbname = drcdrn-amr-dev user = drcdrn password = drcdrn hostaddr = 10.167.1.198 port = 5433" : connection_string;

        connection_ptr = new pqxx::connection(conn_str);
    }
    Database::~Database()
    {
        delete connection_ptr;
    }

    pqxx::connection &Database::database()
    {
        if (connection_ptr && connection_ptr->is_open())
        {
            return *connection_ptr;
        }
        else
        {
            throw std::runtime_error("Connection is not open");
        }
    }

    bool Database::create(nlohmann::json json_message)
    {
        nlohmann::json data_object = json_message["data"];
        std::string key = "";

        key = "maps";
        int id = 0;
        if (data_object.contains(key))
        {
            auto maps = data_object[key];
            id = maps["id"].get<int>();
            int idOld = -1;
            std::string mapsOld = mapGetAll();
            nlohmann::json jsonArray = nlohmann::json::parse(mapsOld);
            if (!jsonArray.empty() && jsonArray.is_array())
            {
                idOld = jsonArray[0]["id"].get<int>();
            }

            if (id != idOld || idOld == -1)
            {
                this->clearMapTables();
            }
            else
            {
                NodeService nodeService;
                nodeService.markAllAsDeleted(*this->connection_ptr);
            }

            bool status = this->mapsUpdate(maps);

            if (status)
            {
                key = "nodes";
                if (maps.contains(key))
                {
                    nlohmann::json nodes = maps[key];
                    this->processData(key, id, nodes);
                }

                key = "nodeConnections";
                if (maps.contains(key))
                {
                    auto nodeConnections = maps[key];
                    this->processData(key, id, nodeConnections);
                }

                key = "zones";
                if (maps.contains(key))
                {
                    auto zones = maps[key];
                    this->processData(key, id, zones);
                }
            }
            else
            {
                return false;
            }
        }

        key = "anchors";
        if (data_object.contains(key))
        {
            auto settings = data_object[key];
            this->processData(key, id, settings);
        }

        key = "settings";
        if (data_object.contains(key))
        {
            auto settings = data_object[key];
            this->processData(key, id, settings);
        }

        key = "vehicle";
        if (data_object.contains(key))
        {
            this->processData(key, id, json_message);
        }

        return true;
    }

    void Database::createLocal(std::string &table_name, std::string &table_string)
    {
        nlohmann::json table = nlohmann::json::parse(table_string);

        if (table_name == "alerts")
        {
            AlertService alertService;
            Alert alert(table["id"], table["uuid"], table["active"], table["error_code"], table["data"], table["message"], table["timestamp"], table["supervisory_registration"]);
            alertService.create(*this->connection_ptr, alert);
        }
        else if (table_name == "logs")
        {
            LogService logService;
            Log log(table["id"], table["uuid"], table["data"], table["timestamp"], table["supervisory_registration"]);
            logService.create(*this->connection_ptr, log);
        }
        else if (table_name == "robot")
        {
            RobotService robotService;
            Robot robot(table["id"], table["uuid"], table["data"], table["timestamp"]);
            robotService.robot(*this->connection_ptr, robot);
        }
        else if (table_name == "maps")
        {
            MapService mapService;
            Map map(table["id"], table["name"], table["mode"], table["resolution"], table["origin_x"], table["origin_y"], table["origin_z"], table["negate"], table["occupied_thresh"], table["free_thresh"]);
            mapService.create(*this->connection_ptr, map);
        }
        else if (table_name == "mission_logs")
        {
            MissionLogService missionLogService;
            MissionLog missionLog(table["id"], table["uuid"], table["status"], table["data"], table["timestamp"], table["supervisory_registration"], table["mission_id"]);
            missionLogService.create(*this->connection_ptr, missionLog);
        }
        else if (table_name == "mission")
        {
            MissionService missionService;
            Mission mission(table["id"], table["title"], table["type"], table["end_id"], table["mission_direction"]);
            missionService.create(*this->connection_ptr, mission);
        }
        else if (table_name == "telemetries")
        {
            TelemetrieService telemetrieService;
            Telemetrie telemetrie(table["id"], table["data"], table["pos_x"], table["pos_y"], table["angle"], table["battery"], table["tension"], table["velocity"]);
            telemetrieService.create(*this->connection_ptr, telemetrie);
        }
        else if (table_name == "vehicles")
        {
            VehicleService vehicleService;
            Vehicle vehicle(table["id"], table["code"], table["name"], table["model"]);
            vehicleService.create(*this->connection_ptr, vehicle);
        }
    }

    void Database::migration()
    {
        DatabasemMigration databasemMigration;
        databasemMigration.createTablesIfNotExist(*this->connection_ptr);
    }

    void Database::processData(std::string &key, int map_id, nlohmann::json data_object)
    {
        for (const auto &jt : data_object)
        {
            if (key == "nodeTypes")
            {
                int id = jt.at("id").get<int>();
                std::string name = jt.at("name").get<std::string>();

                NodeType nodeType(id, name);
                NodeTypeService nodeTypeService;
                nodeTypeService.update(*this->connection_ptr, nodeType);
            }
            else if (key == "nodes")
            {
                int id = jt.at("id").get<int>();
                double x = std::round(jt.at("posX").get<double>() * 100.0) / 100.0;
                double y = std::round(jt.at("posY").get<double>() * 100.0) / 100.0;
                std::stringstream ss_x, ss_y;
                ss_x << std::fixed << std::setprecision(2) << x;
                ss_y << std::fixed << std::setprecision(2) << y;
                std::string pos_x = ss_x.str();
                std::string pos_y = ss_y.str();
                auto angle = "1"; // std::string angle = std::to_string(jt->at("angle").get<int>());
                int node_type = jt.at("nodeType").at("id").get<int>();
                std::string node_type_name = jt.at("nodeType").at("name").get<std::string>();
                std::string direction = jt.at("direction").get<std::string>();
                std::string label;
                if (jt.contains("label") && !jt["label"].is_null()) {
                    label = jt.at("label").get<std::string>();
                } else {
                    label = ""; 
                }
                NodeType nodeType(node_type, node_type_name);
                NodeTypeService nodeTypeService;
                nodeTypeService.update(*this->connection_ptr, nodeType);
                std::string deleted_supervisory = "false";

                Node node(id, pos_x, pos_y, angle, deleted_supervisory, map_id, node_type, direction, label);
                NodeService nodeService;
                nodeService.update(*this->connection_ptr, node);
            }
            else if (key == "anchors")
            {
                int id = jt.at("id").get<int>();
                std::string name = jt.at("name").get<std::string>();
                std::string pos_x = std::to_string(jt.at("posX").get<int>());
                std::string pos_y = std::to_string(jt.at("posY").get<int>());
                std::string angle = std::to_string(jt.at("angle").get<int>());

                Anchor anchor(id, name, pos_x, pos_y, angle);
                AnchorService anchorService;
                anchorService.update(*this->connection_ptr, anchor);
            }
            else if (key == "nodeConnections")
            {
                int id = jt.at("id").get<int>();

                nlohmann::json line_json = jt.at("line");
                std::vector<std::vector<double>> line;
                for (const auto &coord_pair : line_json)
                {
                    double x = 0; // std::round(coord_pair[0].get<double>() * 100.0) / 100.0;
                    double y = 0; // std::round(coord_pair[1].get<double>() * 100.0) / 100.0;
                    line.push_back({x, y});
                }

                int source_node_id = jt.at("sourceNode").at("id").get<int>();
                int target_node_id = jt.at("targetNode").at("id").get<int>();

                NodeConnection nodeConnection(id, line, source_node_id, target_node_id);
                NodeConnectionService nodeConnectionService;
                nodeConnectionService.update(*this->connection_ptr, nodeConnection);
            }
            else if (key == "settings")
            {
                int id = data_object.at("id").get<int>();
                std::string key_ = data_object.at("key").get<std::string>();
                std::string value_ = data_object.at("value").get<std::string>();

                Setting setting(id, key_, value_);
                SettingService settingService;
                settingService.update(*this->connection_ptr, setting);
            }
            else if (key == "zoneType")
            {
                int id = jt.at("id").get<int>();
                std::string name = jt.at("name").get<std::string>();

                ZoneType zoneType(id, name);
                ZoneTypeService zoneTypeService;
                zoneTypeService.update(*this->connection_ptr, zoneType);
            }
            else if (key == "zones")
            {
                int id = jt.at("id").get<int>();
                std::string max_velocity = "0";

                if (!jt.at("maxVelocity").is_null())
                {
                    max_velocity = std::to_string(jt.at("maxVelocity").get<double>());
                }

                nlohmann::json area_json = jt.at("area");
                std::vector<std::vector<double>> area;
                for (const auto &coord_pair : area_json)
                {
                    double x = std::round(coord_pair[0].get<double>() * 100.0) / 100.0;
                    double y = std::round(coord_pair[1].get<double>() * 100.0) / 100.0;
                    area.push_back({x, y});
                }

                int type_zone_id = jt.at("zoneType").at("id").get<int>();
                std::string type_zone_name = jt.at("zoneType").at("name").get<std::string>();
                ZoneType zoneType(type_zone_id, type_zone_name);
                ZoneTypeService zoneTypeService;
                zoneTypeService.update(*this->connection_ptr, zoneType);

                Zone zone(id, max_velocity, area, map_id, type_zone_id);
                ZoneService zoneService;
                zoneService.update(*this->connection_ptr, zone);
            }
            else if (key == "vehicle")
            {
                uint64_t code_ = data_object["code"];
                std::string code = std::to_string(code_);

                Vehicle vehicle(1, code, "name", "modelo");
                VehicleService vehicleService;
                vehicleService.update(*this->connection_ptr, vehicle);
            }
        }
    }

    bool Database::mapsUpdate(nlohmann::json maps)
    {
        std::stringstream stream_resolution, stream_x, stream_y, stream_z, stream_negate, stream_occupied_thresh, stream_free_thresh;
        int id = maps["id"].get<int>();
        std::string name = maps["name"].get<std::string>();
        std::string image = maps["image"].get<std::string>();
        std::string mode = maps["mode"].get<std::string>();
        stream_resolution << std::fixed << std::setprecision(2) << maps["resolution"].get<float>();
        stream_x << std::fixed << std::setprecision(2) << maps["originX"].get<float>();
        stream_y << std::fixed << std::setprecision(2) << maps["originY"].get<float>();
        stream_z << std::fixed << std::setprecision(0) << maps["originZ"].get<float>();
        stream_negate << std::fixed << std::setprecision(0) << maps["negate"].get<float>();
        stream_occupied_thresh << std::fixed << std::setprecision(2) << maps["occupiedThresh"].get<float>();
        stream_free_thresh << std::fixed << std::setprecision(2) << maps["freeThresh"].get<float>();

        const std::string package_share_directory = ament_index_cpp::get_package_share_directory("drn_navigation_mapping");
        const std::string map_yaml_path = std::filesystem::path(package_share_directory) / "maps" / "map" / "map.yaml";
        const std::string map_pgm_path = std::filesystem::path(package_share_directory) / "maps" / "map" / "map.pgm";
        const std::string map_keepout_yaml_path = std::filesystem::path(package_share_directory) / "maps" / "map_keepout" / "map.yaml";
        const std::string map_keepout_pgm_path = std::filesystem::path(package_share_directory) / "maps" / "map_keepout" / "map.pgm";
        const std::string map_speeds_yaml_path = std::filesystem::path(package_share_directory) / "maps" / "map_speeds" / "map.yaml";
        const std::string map_speeds_pgm_path = std::filesystem::path(package_share_directory) / "maps" / "map_speeds" / "map.pgm";

        bool s1 = general->saveAsYaml("map", map_yaml_path, mode, stream_resolution, stream_x, stream_y, stream_z, stream_negate, stream_occupied_thresh, stream_free_thresh);
        bool s2 = general->saveAsYaml("map_keepout", map_keepout_yaml_path, mode, stream_resolution, stream_x, stream_y, stream_z, stream_negate, stream_occupied_thresh, stream_free_thresh);
        bool s3 = general->saveAsYaml("map_speeds", map_speeds_yaml_path, mode, stream_resolution, stream_x, stream_y, stream_z, stream_negate, stream_occupied_thresh, stream_free_thresh);

        std::vector<unsigned char> decoded_data = general->base64Decode(image);
        std::vector<unsigned char> data(decoded_data.begin(), decoded_data.end());
        cv::Mat png_image = cv::imdecode(data, cv::IMREAD_GRAYSCALE);

        double maximumReferenceSpeed = this->settingGetValueAsDouble("maximumReferenceSpeed");
        bool s4 = general->saveAsPGM(png_image, map_pgm_path);
        bool s5 = general->saveFiltersAsPGM(png_image, "map_keepout", map_keepout_pgm_path, maps, stream_resolution, maximumReferenceSpeed);
        bool s6 = general->saveFiltersAsPGM(png_image, "map_speeds", map_speeds_pgm_path, maps, stream_resolution, maximumReferenceSpeed);

        if (s1 && s2 && s3 && s4 && s5 && s6)
        {
            Map map(id, name, mode, stream_resolution.str(), stream_x.str(), stream_y.str(), stream_z.str(), stream_negate.str(), stream_occupied_thresh.str(), stream_free_thresh.str());
            MapService mapService;
            mapService.update(*this->connection_ptr, map);

            return true;
        }
        else
        {
            return false;
        }
    }

    void Database::clearMapTables()
    {
        MapService mapService;
        mapService.clearMapTable(*this->connection_ptr);
    }

    std::string Database::alertGetAll()
    {
        AlertService alertService;
        auto alerts = alertService.getAll(*this->connection_ptr);

        std::vector<nlohmann::json> alertsJson;
        for (const auto &alert : alerts)
        {
            nlohmann::json alertJson = {
                {"id", alert.id},
                {"active", alert.active},
                {"data", alert.data},
                {"error_code", alert.error_code},
                {"message", alert.message},
                {"timestamp", alert.timestamp},
                {"uuid", alert.uuid},
                {"supervisory_registration", alert.supervisory_registration}};

            alertsJson.push_back(alertJson);
        }
        nlohmann::json resultJson = alertsJson;
        return resultJson.dump();
    }

    void Database::updateAlertSupervisory(const std::string &uuid)
    {
        AlertService alertService;
        alertService.updateSupervisoryRegistration(*this->connection_ptr, uuid);
    }

    void Database::updateLogSupervisory(const std::string &uuid)
    {
        LogService logService;
        logService.updateSupervisoryRegistration(*this->connection_ptr, uuid);
    }

    void Database::updateMissionLogSupervisory(const std::string &uuid)
    {
        MissionLogService missionLogService;
        missionLogService.updateSupervisoryRegistration(*this->connection_ptr, uuid);
    }

    std::string Database::anchortGetAll()
    {
        AnchorService anchorService;
        auto anchors = anchorService.getAll(*this->connection_ptr);

        std::vector<nlohmann::json> anchorsJson;
        for (const auto &anchor : anchors)
        {
            nlohmann::json anchorJson = {
                {"id", anchor.id},
                {"angle", anchor.angle},
                {"name", anchor.name},
                {"pos_x", anchor.pos_x},
                {"pos_y", anchor.pos_y},
            };

            anchorsJson.push_back(anchorJson);
        }
        nlohmann::json resultJson = anchorsJson;
        return resultJson.dump();
    }

    std::string Database::logGetAll()
    {
        LogService logService;
        auto logs = logService.getAll(*this->connection_ptr);

        std::vector<nlohmann::json> logsJson;
        for (const auto &log : logs)
        {
            nlohmann::json logJson = {
                {"id", log.id},
                {"data", log.data},
                {"uuid", log.uuid},
                {"timestamp", log.timestamp},
                {"supervisory_registration", log.supervisory_registration},
            };

            logsJson.push_back(logJson);
        }
        nlohmann::json resultJson = logsJson;
        return resultJson.dump();
    }

    std::string Database::mapGetAll()
    {
        MapService mapService;
        auto maps = mapService.getAll(*this->connection_ptr);

        std::vector<nlohmann::json> mapsJson;
        for (const auto &map : maps)
        {
            nlohmann::json mapJson = {
                {"id", map.id},
                {"name", map.name},
                {"mode", map.mode},
                {"resolution", map.resolution},
                {"origin_x", map.origin_x},
                {"origin_y", map.origin_y},
                {"origin_z", map.origin_z},
                {"negate", map.negate},
                {"occupied_thresh", map.occupied_thresh},
                {"free_thresh", map.free_thresh},
            };
            mapsJson.push_back(mapJson);
        }
        nlohmann::json resultJson = mapsJson;
        return resultJson.dump();
    }

    std::string Database::missionLogGetAll()
    {
        MissionLogService missionLogService;
        auto missionLogs = missionLogService.getAll(*this->connection_ptr);

        std::vector<nlohmann::json> missionLogsJson;
        for (const auto &missionLog : missionLogs)
        {
            nlohmann::json missionLogJson = {
                {"id", missionLog.id},
                {"uuid", missionLog.uuid},
                {"status", missionLog.status},
                {"data", missionLog.data},
                {"timestamp", missionLog.timestamp},
                {"supervisory_registration", missionLog.supervisory_registration},
                {"mission_id", missionLog.mission_id},
            };

            missionLogsJson.push_back(missionLogJson);
        }
        nlohmann::json resultJson = missionLogsJson;
        return resultJson.dump();
    }

    nlohmann::json Database::missionFindOne(int id)
    {
        MissionService missionService;
        Mission mission = missionService.getById(*this->connection_ptr, id);

        nlohmann::json missionJson = {
            {"id", mission.id},
            {"title", mission.title},
            {"type", mission.type},
            {"end_id", mission.end_id},
            {"mission_direction", mission.mission_direction},
        };

        return missionJson;
    }

    nlohmann::json Database::getLastMission()
    {
        MissionService missionService;
        Mission mission = missionService.getLastMission(*this->connection_ptr);

        if (mission.id != 0)
        {
            nlohmann::json missionJson = {
                {"id", mission.id},
                {"title", mission.title},
                {"type", mission.type},
                {"end_id", mission.end_id},
                {"mission_direction", mission.mission_direction}};

            return missionJson;
        }
        else
        {
            return nlohmann::json::object();
        }
    }

    int32_t Database::getLastAlertId()
    {
        AlertService alertService;
        int32_t id = alertService.getLastId(*this->connection_ptr);

        return id;
    }

    int32_t Database::getLastMapId()
    {
        MapService mapService;
        return mapService.getLastId(*this->connection_ptr);
    }

    int32_t Database::getRecordCountMissionId()
    {
        MissionService missionService;
        int32_t count = missionService.getRecordCount(*this->connection_ptr);

        return count;
    }

    int32_t Database::getRecordCountAlertId()
    {
        AlertService alertService;
        int32_t count = alertService.getRecordCount(*this->connection_ptr);

        return count;
    }

    int32_t Database::getRecordCountLogId()
    {
        LogService logService;
        int32_t count = logService.getRecordCount(*this->connection_ptr);

        return count;
    }

    void Database::removeFirstMissionRecord()
    {
        MissionService missionService;
        MissionLogService missionLogService;
        auto id = missionService.getFirstId(*this->connection_ptr);
        missionLogService.removeMissionLogs(*this->connection_ptr, id);
        missionService.remove(*this->connection_ptr, id);
    }

    void Database::removeFirstAlertRecord()
    {
        AlertService alertService;
        alertService.removeFirstRecord(*this->connection_ptr);
    }

    int32_t Database::getLastMissionLogId()
    {
        MissionLogService missionLogService;
        int32_t id = missionLogService.getLastId(*this->connection_ptr);

        return id;
    }

    bool Database::missionExistId(int id)
    {
        MissionService missionService;
        return missionService.existId(*this->connection_ptr, id);
    }

    void Database::removeFirstMissionLogRecord()
    {
        MissionLogService missionLogService;
        missionLogService.removeFirstRecord(*this->connection_ptr);
    }

    int32_t Database::getLastLogId()
    {
        LogService logService;
        int32_t id = logService.getLastId(*this->connection_ptr);

        return id;
    }

    std::optional<Pose> Database::getPoseById(int id)
    {
        PoseService poseService;
        return poseService.getById(*this->connection_ptr, id);
    }

    void Database::createPoseWithId(Pose pose)
    {
        PoseService poseService;
        poseService.createWithId(*this->connection_ptr, pose);
    }

    void Database::removeFirstLogRecord()
    {
        LogService logService;
        logService.removeFirstRecord(*this->connection_ptr);
    }

    std::string Database::nodeConnectionGetAll()
    {
        NodeConnectionService nodeConnectionService;
        auto nodeConnections = nodeConnectionService.getAll(*this->connection_ptr);

        std::vector<nlohmann::json> nodeConnectionsJson;
        for (const auto &nodeConnection : nodeConnections)
        {
            nlohmann::json nodeConnectionJson = {
                {"id", nodeConnection.id},
                {"line", nodeConnection.line},
                {"source_node_id", nodeConnection.source_node_id},
                {"target_node_id", nodeConnection.target_node_id},
            };

            nodeConnectionsJson.push_back(nodeConnectionJson);
        }
        nlohmann::json resultJson = nodeConnectionsJson;
        return resultJson.dump();
    }

    std::string Database::nodeTypeGetAll()
    {
        NodeTypeService nodeTypeService;
        auto nodeTypes = nodeTypeService.getAll(*this->connection_ptr);

        std::vector<nlohmann::json> nodeTypesJson;
        for (const auto &nodeType : nodeTypes)
        {
            nlohmann::json nodeTypeJson = {
                {"id", nodeType.id},
                {"name", nodeType.name},
            };

            nodeTypesJson.push_back(nodeTypeJson);
        }
        nlohmann::json resultJson = nodeTypesJson;
        return resultJson.dump();
    }

    std::string Database::nodeGetAll()
    {
        NodeService nodeService;
        auto nodes = nodeService.getAll(*this->connection_ptr);

        std::vector<nlohmann::json> nodesJson;
        for (const auto &node : nodes)
        {
            nlohmann::json nodeJson = {
                {"id", node.id},
                {"angle", node.angle},
                {"map_id", node.map_id},
                {"node_type", node.node_type},
                {"pos_x", node.pos_x},
                {"pos_y", node.pos_y},
                {"deleted_supervisory", node.deleted_supervisory},
                {"direction", node.direction},
                {"label_id", node.label_id}};

            nodesJson.push_back(nodeJson);
        }

        nlohmann::json resultJson = nodesJson;
        return resultJson.dump();
    }

    std::vector<nlohmann::json> Database::settingGetAll()
    {
        SettingService settingService;
        auto settings = settingService.getAll(*this->connection_ptr);

        std::vector<nlohmann::json> settingsJson;
        for (const auto &setting : settings)
        {
            nlohmann::json settingJson = {
                {"id", setting.id},
                {"key", setting.key},
                {"value", setting.value},
            };

            settingsJson.push_back(settingJson);
        }
        return settingsJson;
    }

    double Database::settingGetValueAsDouble(const std::string &key)
    {
        SettingService settingService;
        auto settings = settingService.getAll(*this->connection_ptr);

        for (const auto &setting : settings)
        {
            if (setting.key == key)
            {
                try
                {
                    return std::stod(setting.value);
                }
                catch (const std::invalid_argument &e)
                {
                    std::cerr << "Error: Cannot convert value to double for key: " << key << std::endl;
                    return 0.0;
                }
                catch (const std::out_of_range &e)
                {
                    std::cerr << "Error: Value out of range for key: " << key << std::endl;
                    return 0.0;
                }
            }
        }

        return 0.0;
    }

    std::string Database::telemetrieGetAll()
    {
        TelemetrieService telemetrieService;
        auto telemetries = telemetrieService.getAll(*this->connection_ptr);

        std::vector<nlohmann::json> telemetriesJson;
        for (const auto &telemetrie : telemetries)
        {
            nlohmann::json telemetrieJson = {
                {"id", telemetrie.id},
                {"angle", telemetrie.angle},
                {"battery", telemetrie.battery},
                {"data", telemetrie.data},
                {"pos_x", telemetrie.pos_x},
                {"pos_y", telemetrie.pos_y},
                {"tension", telemetrie.tension},
                {"velocity", telemetrie.velocity}};

            telemetriesJson.push_back(telemetrieJson);
        }
        nlohmann::json resultJson = telemetriesJson;
        return resultJson.dump();
    }

    std::string Database::vehicleGetAll()
    {
        VehicleService vehicleService;
        auto vehicles = vehicleService.getAll(*this->connection_ptr);

        std::vector<nlohmann::json> vehiclesJson;
        for (const auto &vehicle : vehicles)
        {
            nlohmann::json vehicleJson = {
                {"id", vehicle.id},
                {"code", vehicle.code},
                {"name", vehicle.name},
                {"model", vehicle.model}};

            vehiclesJson.push_back(vehicleJson);
        }
        nlohmann::json resultJson = vehiclesJson;
        return resultJson.dump();
    }

    std::string Database::zoneTypeGetAll()
    {
        ZoneTypeService zoneTypeService;
        auto zoneTypes = zoneTypeService.getAll(*this->connection_ptr);

        std::vector<nlohmann::json> zoneTypesJson;
        for (const auto &zoneType : zoneTypes)
        {
            nlohmann::json zoneTypeJson = {
                {"id", zoneType.id},
                {"name", zoneType.name},
            };

            zoneTypesJson.push_back(zoneTypeJson);
        }
        nlohmann::json resultJson = zoneTypesJson;
        return resultJson.dump();
    }

    std::string Database::zoneGetAll()
    {
        ZoneService zoneService;
        auto zones = zoneService.getAll(*this->connection_ptr);

        std::vector<nlohmann::json> zonesJson;
        for (const auto &zone : zones)
        {
            nlohmann::json zoneJson = {
                {"id", zone.id},
                {"area", zone.area},
                {"map_id", zone.map_id},
                {"max_velocity", zone.max_velocity},
                {"type_zone_id", zone.type_zone_id},
            };

            zonesJson.push_back(zoneJson);
        }
        nlohmann::json resultJson = zonesJson;
        return resultJson.dump();
    }

    std::string Database::vehicleFindOne(int id)
    {
        VehicleService vehicleService;
        Vehicle vehicle = vehicleService.getById(*this->connection_ptr, id);

        return vehicle.code;
    }

    std::string Database::logFindOne(int id)
    {
        LogService logService;
        auto result = logService.existId(*this->connection_ptr, id);

        if (result)
        {
            Log log = logService.getById(*this->connection_ptr, id);
            return log.data;
        }
        else
        {
            return "3";
        }
    }

    std::string Database::robotFindOne(int id)
    {
        RobotService robotService;
        auto result = robotService.existId(*this->connection_ptr, id);

        if (result)
        {
            Robot robot = robotService.getById(*this->connection_ptr, id);
            return robot.data;
        }
        else
        {
            return "3";
        }
    }
}

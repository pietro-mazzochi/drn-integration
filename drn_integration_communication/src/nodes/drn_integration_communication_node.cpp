#include <drn_integration_communication/nodes/drn_integration_communication_node.hpp>
namespace drn_integration_communication
{
    CommunicationMqttNode::CommunicationMqttNode() : Node("mqtt_node"), client_(config.mqttServerAddress, CLIENT_ID), callback_(nullptr)
    {
        publisher_ = this->create_publisher<std_msgs::msg::String>("ros_topic", 10);
        subscription_ = this->create_subscription<std_msgs::msg::String>(
            "mqtt_topic", 10, std::bind(&CommunicationMqttNode::rosCallback, this, std::placeholders::_1));

        callback_ = std::make_shared<CommunicationMqttCallback>(publisher_);
        client_.set_callback(*callback_);

        mqtt::connect_options connOpts;

        /**
         * Comentar quando não utilizar usuário e senha
         */
        connOpts.set_user_name(config.mqttUsername);
        connOpts.set_password(config.mqttPassword);
        connOpts.set_keep_alive_interval(config.setKeepAliveInterval);
        connOpts.set_clean_session(false);

        connectToMqttBroker(connOpts);
        reconfigurationRobot();
        initVehicleCode();
        databaseToRosParams(supervisory_command::initialParam);
        
        topic_analysis_thread_ = std::thread(&CommunicationMqttNode::topicAnalysis, this);
        periodic_publish_thread_ = std::thread(&CommunicationMqttNode::periodicMqttPublish, this);

        globalMapReaderNode();
        poseReaderNode();
        amclPoseReaderNode();
        // velocityReaderNode();
        // batteryReaderNode();
        heartbitReader();
        plcRequestNode();
        siemenst200spNode();
        auxNode();

        alert_service_ = this->create_service<drn_integration_msgs::srv::Alert>("alert_service", std::bind(&CommunicationMqttNode::handle_alert_service, this, _1, _2));
        log_service_ = this->create_service<drn_integration_msgs::srv::Log>("log_service", std::bind(&CommunicationMqttNode::handle_log_service, this, _1, _2));
        mission_log_service_ = this->create_service<drn_integration_msgs::srv::MissionLog>("mission_log_service", std::bind(&CommunicationMqttNode::handle_mission_log_service, this, _1, _2));
        mission_service_ = this->create_service<drn_integration_msgs::srv::Mission>("mission_service", std::bind(&CommunicationMqttNode::handle_mission_service, this, _1, _2));
        robot_mode_service_ = this->create_service<drn_integration_msgs::srv::RobotMode>("robot_mode_service", std::bind(&CommunicationMqttNode::handle_robot_mode_service, this, _1, _2));
        robot_status_service_ = this->create_service<drn_integration_msgs::srv::RobotStatus>("robot_status_service", std::bind(&CommunicationMqttNode::handle_robot_status_service, this, _1, _2));
        reconnect_thread_ = std::thread(&CommunicationMqttNode::mqttReconnectLoop, this);

        cancelUnfinishedMissionsCorrectly();
    }
    CommunicationMqttNode::~CommunicationMqttNode()
    {
        if (topic_analysis_thread_.joinable())
        {
            topic_analysis_thread_.join();
        }
        if (periodic_publish_thread_.joinable())
        {
            periodic_publish_thread_.join();
        }
    }
    void CommunicationMqttNode::connectToMqttBroker(mqtt::connect_options &connOpts)
    {
        while (rclcpp::ok())
        {
            try
            {
                RCLCPP_INFO(this->get_logger(), mqtt::msnConnect.c_str());
                client_.connect(connOpts)->wait();
                RCLCPP_INFO(this->get_logger(), mqtt::msnSuccessfully.c_str());
                return;
            }
            catch (const mqtt::exception &exc)
            {
                std::this_thread::sleep_for(std::chrono::seconds(config.sleepMqttReconnectLoop));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
        }
    }
    void CommunicationMqttNode::mqttReconnectLoop()
    {
        while (rclcpp::ok())
        {
            if (!client_.is_connected())
            {
                RCLCPP_WARN(this->get_logger(), mqtt::msnReconnect.c_str());
                mqtt::connect_options connOpts;
                connOpts.set_user_name(config.mqttUsername);
                connOpts.set_password(config.mqttPassword);
                connOpts.set_keep_alive_interval(config.setKeepAliveInterval);
                connOpts.set_clean_session(false);
                connectToMqttBroker(connOpts);
                reconfigurationRobot();
                initVehicleCode();
            }
            std::this_thread::sleep_for(std::chrono::seconds(config.sleepMqttReconnectLoop));
        }
    }
    
    void CommunicationMqttNode::handle_alert_service(const std::shared_ptr<drn_integration_msgs::srv::Alert::Request> request, const std::shared_ptr<drn_integration_msgs::srv::Alert::Response> response)
    {
        std::string table_name = "alerts";
        response->success = true;
        response->feedback = "Alert received!";

        nlohmann::json json_obj = {
            {"data", {{"uuid", request->uuid}, {"active", request->active}, {"errorcode", static_cast<int16_t>(request->errorcode)}, {"data", request->data}, {"message", request->message}, {"timestamp", request->timestamp}}}};
        std::string payload = json_obj.dump();

        auto topic = "/" + std::to_string(CODE_REF) + config.topicTelemetryAlert;
        mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
        client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));

        nlohmann::json json_obj2 = {
            {"id", 0},
            {"uuid", std::string(request->uuid)},
            {"active", request->active ? "true" : "false"},
            {"error_code", std::to_string(request->errorcode)},
            {"data", std::string(request->data)},
            {"message", std::string(request->message)},
            {"timestamp", std::string(request->timestamp)},
            {"supervisory_registration", std::string("")}};
        std::string table_string = json_obj2.dump();

        auto countId = database->getRecordCountAlertId();
        if (countId > config.buffer)
        {
            database->removeFirstAlertRecord();
            database->createLocal(table_name, table_string);
        }
        else
        {
            database->createLocal(table_name, table_string);
        }
    }
    void CommunicationMqttNode::handle_log_service(const std::shared_ptr<drn_integration_msgs::srv::Log::Request> request, const std::shared_ptr<drn_integration_msgs::srv::Log::Response> response)
    {
        std::string table_name = "logs";
        response->success = true;
        response->feedback = "Log received!";

        nlohmann::json json_obj = {
            {"data", {{"uuid", request->uuid}, {"data", request->data}, {"timestamp", request->timestamp}}}};

        std::string payload = json_obj.dump();

        auto topic = "/" + std::to_string(CODE_REF) + config.topicTelemetryLogs;
        mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
        client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));

        nlohmann::json json_obj2 = {
            {"id", 0},
            {"uuid", std::string(request->uuid)},
            {"data", std::string(request->data)},
            {"timestamp", std::string(request->timestamp)},
            {"supervisory_registration", std::string("")},
        };

        auto countId = database->getRecordCountLogId();
        std::string table_string = json_obj2.dump();
        if (countId > config.buffer)
        {
            database->removeFirstLogRecord();
            database->createLocal(table_name, table_string);
        }
        else
        {
            database->createLocal(table_name, table_string);
        }
    }
    void CommunicationMqttNode::handle_mission_log_service(const std::shared_ptr<drn_integration_msgs::srv::MissionLog::Request> request, const std::shared_ptr<drn_integration_msgs::srv::MissionLog::Response> response)
    {
        std::string table_name = "mission_logs";

        auto result = database->missionExistId(request->id);

        if (result)
        {
            response->success = true;
            response->feedback = "Mission log received!";

            nlohmann::json json_obj = {
                {"data", {{"mission_id", static_cast<int16_t>(request->id)}, {"uuid", request->uuid}, {"status", request->status}, {"data", request->data}, {"timestamp", request->timestamp}}}};

            std::string payload = json_obj.dump();

            auto topic = "/" + std::to_string(CODE_REF) + config.topicMissionLog;
            mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
            client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));

            nlohmann::json json_obj2 = {
                {"id", 1},
                {"uuid", std::string(request->uuid)},
                {"status", std::string(request->status)},
                {"data", std::string(request->data)},
                {"timestamp", std::string(request->timestamp)},
                {"supervisory_registration", std::string("")},
                {"mission_id", request->id}};

            std::string table_string = json_obj2.dump();

            database->createLocal(table_name, table_string);
        }
        else
        {
            response->success = false;
            response->feedback = "Mission ID does not exist!";
        }
    }
    void CommunicationMqttNode::handle_mission_service(const std::shared_ptr<drn_integration_msgs::srv::Mission::Request> request, const std::shared_ptr<drn_integration_msgs::srv::Mission::Response> response)
    {
        try
        {
            nlohmann::json json_obj = {
                {"code", CODE_REF},
                {"command", request->command},
                {"data", {{"id", static_cast<int16_t>(request->id)}, {"response", static_cast<bool>(request->status)}}}};

            std::string payload = json_obj.dump();

            auto topic = "/" + std::to_string(CODE_REF) + config.topicRemoteSystemResponse;
            mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
            client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));

            if (request->command == supervisory_command::finishMission)
            {
                publisherToPlcMission(mission_status_plc::finish, robot_type_plc::info, message_code_plc::finish, message_plc::msnFinish);
            }
            else if (request->command == supervisory_command::cancelMission)
            {
                publisherToPlcMission(mission_status_plc::cancel, robot_type_plc::error, message_code_plc::cancel, message_plc::msnCancel);
            }

            response->success = true;
        }
        catch (const mqtt::exception &exc)
        {
            response->success = false;
        }
    }
    void CommunicationMqttNode::handle_robot_mode_service(const std::shared_ptr<drn_integration_msgs::srv::RobotMode::Request> request, const std::shared_ptr<drn_integration_msgs::srv::RobotMode::Response> response)
    {
        std::string table_name = "robot";

        nlohmann::json json_obj = {
            {"id", config.codeRobotMode},
            {"uuid", "0"},
            {"data", request->autonomous ? "true" : "false"},
            {"timestamp", "0"}};

        std::string table_string = json_obj.dump();
        database->createLocal(table_name, table_string);

        response->success = true;
        response->feedback = "Robot mode received!";

        nlohmann::json json_obj2 = {
            {"autonomous", request->autonomous}};

        std::string payload = json_obj2.dump();

        auto topic = "/" + std::to_string(CODE_REF) + config.topicRobotMode;
        mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
        client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));
    }
    void CommunicationMqttNode::handle_robot_status_service(const std::shared_ptr<drn_integration_msgs::srv::RobotStatus::Request> request, const std::shared_ptr<drn_integration_msgs::srv::RobotStatus::Response> response)
    {
        if (request->command == true)
        {
            std::string table_name = "robot";
            nlohmann::json json_obj = {
                {"id", config.codeRobotStatus},
                {"uuid", "0"},
                {"data", request->status ? "true" : "false"},
                {"timestamp", "0"}};

            std::string table_string = json_obj.dump();
            database->createLocal(table_name, table_string);

            response->success = true;
            response->feedback = "Robot status received!";
        }
        else
        {
            try
            {
                auto status = database->robotFindOne(config.codeRobotStatus);
                response->success = true;

                std::string ref = "true";
                if (status == ref)
                {
                    response->feedback = "true";
                    request->status = true;
                }
                else if (status == "3")
                {
                    response->feedback = "null";
                    request->status = true;
                }
                else
                {
                    response->feedback = "false";
                    request->status = false;
                }
            }
            catch (const mqtt::exception &exc)
            {
                RCLCPP_ERROR(this->get_logger(), "Robot status Error: %s", exc.what());
            }
        }

        std::string mqtt_message = std::string("{\"status\":") + (request->status ? "true" : "false") + "}";

        auto topic = "/" + std::to_string(CODE_REF) + config.topicRobotStatus;
        mqtt::message_ptr pubmsg = mqtt::make_message(topic, mqtt_message);
        client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));
    }
    void CommunicationMqttNode::globalMapReaderNode()
    {
        this->subscription_occupancy_grid_ = this->create_subscription<nav_msgs::msg::OccupancyGrid>(
            "/map", 10, std::bind(&CommunicationMqttNode::globalMapReadeCallback, this, std::placeholders::_1));

        timerGlobalMap_ = this->create_wall_timer(
            std::chrono::milliseconds(config.timerGlobalMapPublish), std::bind(&CommunicationMqttNode::globalMapPublish, this));
    }
    void CommunicationMqttNode::auxNode()
    {
        this->publisher_switch_nav_mode = this->create_publisher<std_msgs::msg::Int32>("/switch_nav_mode", 10);
        this->publisher_mission = this->create_publisher<drn_integration_msgs::msg::Mission>("/mission", 10);
        this->publisher_initial_pose = this->create_publisher<geometry_msgs::msg::PoseWithCovarianceStamped>("/initialpose", 10);
        this->publisher_to_plc_mission = this->create_publisher<drc_flexnav_controllers_msgs::msg::PlcMissionResponse>("/to_plc_mission", 10);
        this->publisher_to_plc_command = this->create_publisher<drc_flexnav_controllers_msgs::msg::PlcCommand>("/to_plc_command", 10);
    }
    void CommunicationMqttNode::globalMapReadeCallback(const nav_msgs::msg::OccupancyGrid::SharedPtr msg)
    {
        // RCLCPP_WARN(this->get_logger(), "Recebido occupancyGrid_");
        occupancyGrid_ = msg;
    }
    void CommunicationMqttNode::globalMapPublish()
    {
        if (mapOperation == map::sendSavedMap)
        {
            const std::string package_share_directory = ament_index_cpp::get_package_share_directory("drn_navigation_mapping");
            const std::string map_yaml_path = std::filesystem::path(package_share_directory) / "maps" / "map" / "map.yaml";
            const std::string map_pgm_path = std::filesystem::path(package_share_directory) / "maps" / "map" / "map.pgm";

            if (std::filesystem::exists(map_pgm_path) && std::filesystem::exists(map_yaml_path))
            {
                mapOperation = map::notSendMap;
                cv::Mat pgm_image = cv::imread(map_pgm_path, cv::IMREAD_GRAYSCALE);
                if (pgm_image.empty())
                {
                    throw std::runtime_error("Erro ao carregar a imagem PGM.");
                }

                std::vector<uchar> buffer;
                cv::imencode(".png", pgm_image, buffer);
                std::string base64_str2 = general->base64Encode2(buffer);

                YAML::Node yaml = YAML::LoadFile(map_yaml_path);
                std::string mode = yaml["mode"].as<std::string>();
                double resolution = yaml["resolution"].as<double>();
                double origin_x = yaml["origin"][0].as<double>();
                double origin_y = yaml["origin"][1].as<double>();
                double origin_z = yaml["origin"][2].as<double>();
                int negate = yaml["negate"].as<int>();
                double occupied_thresh = yaml["occupied_thresh"].as<double>();
                double free_thresh = yaml["free_thresh"].as<double>();

                std::ostringstream oss;
                oss << "{"
                    << "\"code\":" << CODE_REF << ","
                    << "\"command\":\"0xC7\","
                    << "\"data\":{"
                    << "\"maps\":{"
                    << "\"image\":\"" << base64_str2 << "\","
                    << "\"mode\":\"" << mode << "\","
                    << "\"resolution\":" << resolution << ","
                    << "\"origin_x\":" << origin_x << ","
                    << "\"origin_y\":" << origin_y << ","
                    << "\"origin_z\":" << origin_z << ","
                    << "\"negate\":" << negate << ","
                    << "\"occupied_thresh\":" << occupied_thresh << ","
                    << "\"free_thresh\":" << free_thresh
                    << "}}}";
                std::string map_data_str = oss.str();

                auto topic = "/" + std::to_string(CODE_REF) + config.topicRemoteSystemResponse;
                std_msgs::msg::String message;
                message.data = map_data_str;

                mqtt::message_ptr pubmsg = mqtt::make_message(topic, message.data);
                pubmsg->set_qos(1);

                client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));

                base64_str2 = "";
            }
        }
        else if (mapOperation == map::sendMapping)
        {
            if ((occupancyGrid_ != nullptr))
            {
                cv::Mat map_image(occupancyGrid_->info.height, occupancyGrid_->info.width, CV_8UC1);

                for (size_t i = 0; i < occupancyGrid_->data.size(); ++i)
                {
                    int value = static_cast<int>(occupancyGrid_->data[i]);
                    if (value == -1)
                    {
                        map_image.data[i] = map::unknownAreas;
                    }
                    else if (value == 0)
                    {
                        map_image.data[i] = map::freeAreas;
                    }
                    else
                    {
                        map_image.data[i] = map::blockedAreas;
                    }
                }

                std::vector<uchar> buffer;

                cv::Mat correctedImage;
                cv::Mat correctedImage2;
                cv::Mat correctedImage3;
                cv::flip(map_image, correctedImage3, 0);
                cv::imencode(".png", correctedImage3, buffer);

                std::string base64_str = general->base64Encode2(buffer);

                std::ostringstream oss;
                oss << "{"
                    << "\"resolution\":" << occupancyGrid_->info.resolution << ","
                    << "\"width\":" << occupancyGrid_->info.width << ","
                    << "\"height\":" << occupancyGrid_->info.height << ","
                    << "\"image_base64\":\"" << base64_str << "\""
                    << "}";
                std::string map_data_str = oss.str();

                auto topic = "/" + std::to_string(CODE_REF) + config.topicMappingMap;
                std_msgs::msg::String message;
                message.data = map_data_str;
                mqtt::message_ptr pubmsg = mqtt::make_message(topic, message.data);
                pubmsg->set_qos(1);

                try
                {
                    client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));
                }
                catch (const mqtt::exception &exc)
                {
                    RCLCPP_ERROR(this->get_logger(), "MQTT Publish Error: %s", exc.what());
                }
                catch (const std::exception &e)
                {
                    RCLCPP_INFO(this->get_logger(), "Error mapping!");
                }
            }
            else
            {
                // RCLCPP_INFO(this->get_logger(), "Cannot publish Pose: Occupancy grid is null! [globalMapPublish]");
            }
        }
    }
    void CommunicationMqttNode::poseReaderNode()
    {
        this->subscription_pose_ = this->create_subscription<geometry_msgs::msg::PoseWithCovarianceStamped>(
            "/pose", 10, std::bind(&CommunicationMqttNode::poseCallback, this, std::placeholders::_1));

        timerPose_ = this->create_wall_timer(
            std::chrono::milliseconds(config.timerGlobalPosePublish), std::bind(&CommunicationMqttNode::posePublish, this));
    }
    void CommunicationMqttNode::poseCallback(const geometry_msgs::msg::PoseWithCovarianceStamped::SharedPtr msg)
    {
        pose_ = msg;
    }
    void CommunicationMqttNode::posePublish()
    {
        if ((pose_ != nullptr && occupancyGrid_ != nullptr) && (public_map))
        {
            double angle = 0.0;
            double pos_x = ((pose_->pose.pose.position.x - occupancyGrid_->info.origin.position.x) / occupancyGrid_->info.resolution);
            double pos_y = ((pose_->pose.pose.position.y - occupancyGrid_->info.origin.position.y) / occupancyGrid_->info.resolution);
            double x_ = pose_->pose.pose.orientation.x;
            double y_ = pose_->pose.pose.orientation.y;
            double z_ = pose_->pose.pose.orientation.z;
            double w_ = pose_->pose.pose.orientation.w;

            tf2::Quaternion q(x_, y_, z_, w_);
            angle = atan2(2.0 * (q.w() * q.z() + q.x() * q.y()), 1.0 - 2.0 * (q.y() * q.y() + q.z() * q.z()));
            angle = angle * 180.0 / M_PI;
            auto angleAdjust = general->normalizeAndOffsetAngle(angle);
            pos_x = std::round(pos_x * 1.0) / 1.0;
            pos_y = std::round(pos_y * 1.0) / 1.0;
            angleAdjust = std::round(angleAdjust * 1.0) / 1.0;

            Pose pose(1, pos_x, pos_y, 0.0, x_, y_, z_, w_);
            //database->createPoseWithId(pose); //TODO: verificar se essa pose pode utilizada para inciar amcl

            nlohmann::json json_obj = {
                {"data", {{"x", pos_x}, {"y", pos_y}, {"angle", angleAdjust}}}};

            std::string payload = json_obj.dump();

            auto topic = "/" + std::to_string(CODE_REF) + config.topicTelemetryPose;
            mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
            client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));

            pose_ = nullptr;
        }
    }
    void CommunicationMqttNode::amclPoseReaderNode()
    {
        this->subscription_amcl_pose_ = this->create_subscription<geometry_msgs::msg::PoseWithCovarianceStamped>(
            "/amcl_pose", 10, std::bind(&CommunicationMqttNode::amclPoseCallback, this, std::placeholders::_1));

        timerAmclPose_ = this->create_wall_timer(
            std::chrono::milliseconds(config.timerGlobalAmclPosePublish), std::bind(&CommunicationMqttNode::amclPosePublish, this));
    }
    void CommunicationMqttNode::amclPoseCallback(const geometry_msgs::msg::PoseWithCovarianceStamped::SharedPtr msg)
    {
        amcl_pose_ = msg;
    }
    void CommunicationMqttNode::amclPosePublish()
    {
        if ((amcl_pose_ != nullptr) && (!public_map))
        {
            auto mapString = database->mapGetAll();
            if (!mapString.empty())
            {
                nlohmann::json mapJson = nlohmann::json::parse(mapString);
                if (!mapJson.empty())
                {
                    double resolution = std::stod(mapJson[0]["resolution"].get<std::string>());
                    double angle = 0.0;
                    double x = ((amcl_pose_->pose.pose.position.x) / resolution);
                    double y = ((amcl_pose_->pose.pose.position.y) / resolution);
                    double x_ = amcl_pose_->pose.pose.orientation.x;
                    double y_ = amcl_pose_->pose.pose.orientation.y;
                    double z_ = amcl_pose_->pose.pose.orientation.z;
                    double w_ = amcl_pose_->pose.pose.orientation.w;

                    tf2::Quaternion q(x_, y_, z_, w_);
                    angle = atan2(2.0 * (q.w() * q.z() + q.x() * q.y()), 1.0 - 2.0 * (q.y() * q.y() + q.z() * q.z()));
                    angle = angle * 180.0 / M_PI;
                    auto angleAjust = general->normalizeAndOffsetAngle(angle);
                    x = std::round(x * 1.0) / 1.0;
                    y = std::round(y * 1.0) / 1.0;
                    angleAjust = std::round(angleAjust * 1.0) / 1.0;

                    Pose pose(
                        1,
                        amcl_pose_->pose.pose.position.x,
                        amcl_pose_->pose.pose.position.y, 0.0,
                        amcl_pose_->pose.pose.orientation.x,
                        amcl_pose_->pose.pose.orientation.y,
                        amcl_pose_->pose.pose.orientation.z,
                        amcl_pose_->pose.pose.orientation.w);
                    database->createPoseWithId(pose);

                    nlohmann::json json_obj = {
                        {"data", {{"x", x}, {"y", y}, {"angle", angleAjust}}}};

                    std::string payload = json_obj.dump();

                    auto topic = "/" + std::to_string(CODE_REF) + config.topicTelemetryPose;
                    mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
                    client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));
                }
            }
            amcl_pose_ = nullptr;
        }
    }
    void CommunicationMqttNode::heartbitReader()
    {
        timerHeartbit_ = this->create_wall_timer(
            std::chrono::milliseconds(config.timerHearbitPublish), std::bind(&CommunicationMqttNode::heartbitPublish, this));
    }
    void CommunicationMqttNode::heartbitPublish()
    {
        if (!hearbit)
        {
            timeout++;
            if (timeout > config.timeoutHardbit)
            {
                returnDatabase = true;
                timeout = 0;
            }
        }
        else
        {
            hearbit = false;

            if (returnDatabase)
            {
                returnDatabase = false;
                auto alertas = database->alertGetAll();
                std::vector<nlohmann::json> alertasJson = nlohmann::json::parse(alertas);
                auto logs = database->logGetAll();
                std::vector<nlohmann::json> logsJson = nlohmann::json::parse(logs);
                auto missionLogs = database->missionLogGetAll();
                std::vector<nlohmann::json> missionLogsJson = nlohmann::json::parse(missionLogs);

                for (const auto &alert : alertasJson)
                {
                    if (alert["supervisory_registration"] != "true")
                    {
                        nlohmann::json json_obj = {
                            {"data", {{"uuid", alert["uuid"]}, {"active", alert["active"]}, {"errorcode", alert["error_code"]}, {"data", alert["data"]}, {"message", alert["message"]}, {"timestamp", alert["timestamp"]}}}};
                        std::string payload = json_obj.dump();
                        auto topic = "/" + std::to_string(CODE_REF) + config.topicTelemetryAlert;
                        mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
                        client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));
                    }
                }

                for (const auto &log : logsJson)
                {
                    if (log["supervisory_registration"] != "true")
                    {
                        nlohmann::json json_obj = {
                            {"data", {{"uuid", log["uuid"]}, {"data", log["data"]}, {"timestamp", log["timestamp"]}}}};
                        std::string payload = json_obj.dump();

                        auto topic = "/" + std::to_string(CODE_REF) + config.topicTelemetryLogs;
                        mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
                        client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));
                    }
                }

                for (const auto &missionLog : missionLogsJson)
                {
                    if (missionLog["supervisory_registration"] != "true")
                    {
                        nlohmann::json json_obj = {
                            {"data", {{"mission_id", static_cast<int16_t>(missionLog["mission_id"])}, {"uuid", missionLog["uuid"]}, {"status", missionLog["status"]}, {"data", missionLog["data"]}, {"timestamp", missionLog["timestamp"]}}}};

                        std::string payload = json_obj.dump();
                        auto topic = "/" + std::to_string(CODE_REF) + config.topicMissionStatus;
                        mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
                        client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));
                    }
                }
            }
        }
    }
    void CommunicationMqttNode::velocityReaderNode()
    {
        this->subscription_velocity_ = this->create_subscription<std_msgs::msg::Float64>(
            "/velocity", 10, std::bind(&CommunicationMqttNode::velocityCallback, this, std::placeholders::_1));

        timerVelocity_ = this->create_wall_timer(
            std::chrono::milliseconds(config.timerGlobalVelocityPublish), std::bind(&CommunicationMqttNode::velocityPublish, this));
    }
    void CommunicationMqttNode::velocityCallback(const std_msgs::msg::Float64::SharedPtr msg)
    {
        velocity_ = msg;
    }
    void CommunicationMqttNode::velocityPublish()
    {
        if ((velocity_ == nullptr))
        {
        }
        else
        {
            nlohmann::json json_obj = {
                {"data", {{"velocity", 123.4}}}};

            std::string payload = json_obj.dump();

            auto topic = "/" + std::to_string(CODE_REF) + config.topicTelemetryVelocity;
            mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
            client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));
        }
    }
    void CommunicationMqttNode::batteryReaderNode()
    {
        this->subscription_battery_ = this->create_subscription<std_msgs::msg::Float64>(
            "/battery", 1, std::bind(&CommunicationMqttNode::batteryCallback, this, std::placeholders::_1));

        timerBattery_ = this->create_wall_timer(
            std::chrono::milliseconds(config.timerGlobalBatteryPublish), std::bind(&CommunicationMqttNode::batteryPublish, this));
    }
    void CommunicationMqttNode::batteryCallback(const std_msgs::msg::Float64::SharedPtr msg)
    {
        battery_ = msg;
    }
    void CommunicationMqttNode::batteryPublish()
    {
        if ((battery_ == nullptr))
        {
        }
        else
        {
            nlohmann::json json_obj = {
                {"data", {{"battery", 123.4}}}};

            std::string payload = json_obj.dump();

            auto topic = "/" + std::to_string(CODE_REF) + config.topicTelemetryBattery;
            mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
            client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));
        }
    }
    void CommunicationMqttNode::plcRequestNode()
    {
        this->subscription_plc_request_ = this->create_subscription<drc_flexnav_controllers_msgs::msg::PlcRequest>(
            "/plc_request", 1, std::bind(&CommunicationMqttNode::plcCallback, this, std::placeholders::_1));
    }
    void CommunicationMqttNode::plcCallback(const drc_flexnav_controllers_msgs::msg::PlcRequest::SharedPtr msg)
    {
        if ((msg != nullptr))
        {
            RCLCPP_INFO(this->get_logger(), "msg: %s", msg->command.c_str());

            if (msg->command == plc_command::startMission)
            {
                publisherToPlcMission(mission_status_plc::noMission, robot_type_plc::info, message_code_plc::waiting, message_plc::msnWaiting);

                int32_t map_id = database->getLastMapId();

                nlohmann::json json_obj = {
                    {"code", CODE_REF},
                    {"command", supervisory_command::startMission},
                    {"data", {
                        {"map_id", map_id}, 
                        {"mission_id", 0}, 
                        {"label", msg->mission_label}, 
                        {"type", msg->mission_type}, 
                        {"direction", msg->mission_direction}
                    }}};

                std::string payload = json_obj.dump();
                auto topic = config.topicRemoteSystemRequest;
                mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
                client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));
            }
            else if (msg->command == plc_command::cancelMission)
            {
                auto mission = database->getLastMission();
                int mission_id = 0;
                if (mission.contains("id") && !mission["id"].is_null())
                {
                    mission_id = mission["id"].get<int>();
                }

                try
                {
                    auto msnMission = drn_integration_msgs::msg::Mission();
                    msnMission.id = mission_id;
                    msnMission.command = supervisory_command::cancelMission;
                    msnMission.type = "";
                    msnMission.end_id = 0;
                    msnMission.mission_direction = "";
                    publisher_mission->publish(msnMission);
                }

                catch (const nlohmann::json::type_error &e)
                {
                    std::cerr << "Type error in JSON parsing: " << e.what() << std::endl;
                }
            }
            else if (msg->command == plc_command::shutdown)
            {
                call_script();
                drc_flexnav_controllers_msgs::msg::PlcCommand plc_command;
                plc_command.command = plc_command::shutdownResponse;
                publisher_to_plc_command->publish(plc_command);
            }
        }
    }
    void CommunicationMqttNode::siemenst200spNode()
    {
        this->subscription_siemens_et200sp_data_ = this->create_subscription<siemens_et200sp_modbus_driver_msgs::msg::SiemensEtspModbusDriver>(
            "/siemens_et200sp", 10, std::bind(&CommunicationMqttNode::siemenst200spCallback, this, std::placeholders::_1));
    }
    void CommunicationMqttNode::siemenst200spCallback(const siemens_et200sp_modbus_driver_msgs::msg::SiemensEtspModbusDriver::SharedPtr msg)
    {
        //RCLCPP_INFO(this->get_logger(), "MODO ROBO: auto_sw: %d | man_sw: %d", msg->auto_sw, msg->man_sw);

        if (!msg)
            return;

        std::optional<bool> new_autonomous;
        if (msg->auto_sw && !msg->man_sw)
        {
            new_autonomous = true;
        }
        else if (!msg->auto_sw && msg->man_sw)
        {
            new_autonomous = false;
        }
        else
        {
            new_autonomous = std::nullopt;
        }

        if (new_autonomous == last_autonomous_)
        {
            return;
        }

        if(new_autonomous)
        {
            setPoseInitial();
        }

        nlohmann::json json_obj;
        if (new_autonomous.has_value())
        {
            json_obj = {{"data", {{"autonomous", *new_autonomous}}}};
        }
        else
        {
            json_obj = {{"data", {{"autonomous", nullptr}}}};
        }

        const std::string payload = json_obj.dump();
        const auto topic = "/" + std::to_string(CODE_REF) + config.topicRobotMode;

        mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
        auto tok = client_.publish(pubmsg);
        if (tok)
        {
            tok->wait_for(std::chrono::milliseconds(config.publishTimeout));
        }

        last_autonomous_ = new_autonomous;
    }
    void CommunicationMqttNode::rosCallback(const std_msgs::msg::String::SharedPtr msg)
    {
        mqtt::message_ptr pubmsg = mqtt::make_message(dynamic_topics_.front(), msg->data);
        pubmsg->set_qos(1);
        try
        {
            client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));
        }
        catch (const mqtt::exception &exc)
        {
            RCLCPP_ERROR(this->get_logger(), "MQTT Publish Error: %s", exc.what());
        }
    }
    void CommunicationMqttNode::publishReceivedMessageToMqtt(const std::string &msg)
    {
        const std::string TOPIC_MQTT_RECEIVED = "mqtt_topic_received";
        mqtt::message_ptr pubmsg = mqtt::make_message(TOPIC_MQTT_RECEIVED, msg);
        pubmsg->set_qos(1);
        try
        {
            client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));
        }
        catch (const mqtt::exception &exc)
        {
            RCLCPP_ERROR(this->get_logger(), "MQTT Publish Error: %s", exc.what());
        }
    }
    void CommunicationMqttNode::topicAnalysis()
    {
        std::string topic;
        std::string payload;
        auto msnSwitchNav = std_msgs::msg::Int32();
        auto msnMission = drn_integration_msgs::msg::Mission();

        while (rclcpp::ok())
        {
            if (callback_->get_message(topic, payload))
            {
                // RCLCPP_INFO(this->get_logger(), "topic: %s", topic.c_str());
                // RCLCPP_INFO(this->get_logger(), "payload: %s", payload.c_str());
                if (topic.find("REQUEST") != std::string::npos)
                {
                    try
                    {
                        drn_integration_communication::Command::Result request = command->command(*database, CODE_REF, payload);
                        //RCLCPP_INFO(this->get_logger(), "Command request: %s", request.code.c_str());

                        if (request.code == supervisory_command::initialPose)
                        {
                            nlohmann::json json_message = nlohmann::json::parse(payload);
                            auto object_message = json_message["data"];
                            float pos_x = object_message["posX"].get<float>();
                            float pos_y = object_message["posY"].get<float>();
                            float angle = object_message["angle"].get<float>();
                            double x_, y_, z_, w_;
                            double angle_original = general->inverseNormalizeAndOffsetAngle(angle);
                            double theta = angle_original * M_PI / 180.0;

                            tf2::Quaternion q;
                            q.setRPY(0, 0, theta);

                            x_ = q.x();
                            y_ = q.y();
                            z_ = q.z();
                            w_ = q.w();

                            Pose pose(1, pos_x, pos_y, 0.0, x_, y_, z_, w_);
                            database->createPoseWithId(pose);
                            setPose(pos_x, pos_y, x_, y_, z_, w_);
                        }
                        else if (request.code == supervisory_command::mapUpdate)
                        {
                            nlohmann::json json_obj;
                            if (request.status)
                            {
                                databaseToRosParams(request.code);
                                //publishNavigation(navigation::restartCostmap);

                                json_obj = {
                                    {"code", CODE_REF},
                                    {"command", supervisory_command::mapUpdate},
                                    {"data", {{"response", true}}}};
                            }
                            else
                            {
                                json_obj = {
                                    {"code", CODE_REF},
                                    {"command", supervisory_command::mapUpdate},
                                    {"data", {{"response", false}}}};
                            }
                            std::string payload = json_obj.dump();
                            auto topic = "/" + std::to_string(CODE_REF) + config.topicRemoteSystemResponse;
                            mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
                            client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));
                        }
                        else if (request.code == supervisory_command::startMapping)
                        {
                            RCLCPP_INFO(this->get_logger(), "startMapping");
                            public_map = false;
                            publishNavigation(navigation::startMapping);
                            public_map = true;
                            mapOperation = map::sendMapping;
                        }
                        else if (request.code == supervisory_command::stopMapping)
                        {
                            public_map = false;
                            publishNavigation(navigation::finishMappingAndSaveMap);
                            mapOperation = map::sendSavedMap; 
                        }
                        else if (request.code == supervisory_command::cancelMapping)
                        {
                            public_map = false;
                            mapOperation = map::notSendMap;
                        }
                        else if (request.code == supervisory_command::pauseMapping)
                        {
                            publishNavigation(navigation::finishMappingAndSaveMap);
                            mapOperation = map::notSendMap;
                        }
                        else if (request.code == supervisory_command::startMission)
                        {
                            try
                            {
                                setPoseInitial();
                                
                                std::string table_name = "mission";
                                nlohmann::json json_message = nlohmann::json::parse(payload);
                                auto object_message = json_message["data"];
                                int id = object_message["id"].get<int>();
                                std::string type = object_message["type"].get<std::string>();
                                int end_id = object_message["end_id"].get<int>();
                                std::string mission_direction = object_message["mission_direction"].get<std::string>();

                                nlohmann::json json_obj = {
                                    {"id", id},
                                    {"title", supervisory_command::startMission},
                                    {"type", type},
                                    {"end_id", end_id},
                                    {"mission_direction", mission_direction}};

                                std::string table_string = json_obj.dump();

                                auto countId = database->getRecordCountMissionId();
                                if (countId > config.buffer)
                                {
                                    database->removeFirstMissionRecord();
                                    database->createLocal(table_name, table_string);
                                }
                                else
                                {
                                    database->createLocal(table_name, table_string);
                                }

                                msnMission.id = id;
                                msnMission.command = supervisory_command::startMission;
                                msnMission.type = type;
                                msnMission.end_id = end_id;
                                msnMission.mission_direction = mission_direction;
                                publisher_mission->publish(msnMission);
                                publisherToPlcMission(mission_status_plc::running, robot_type_plc::warning, message_code_plc::running, message_plc::msnExecutingMission);
                            }
                            catch (const nlohmann::json::type_error &e)
                            {
                                std::cerr << "Type error in JSON parsing: " << e.what() << std::endl;
                            }
                        }
                        else if (request.code == supervisory_command::cancelMission)
                        {
                            try
                            {
                                std::string table_name = "mission";
                                nlohmann::json json_message = nlohmann::json::parse(payload);
                                auto object_message = json_message["data"];
                                int id = object_message["id"].get<int>();

                                auto mission = database->missionFindOne(id);

                                nlohmann::json json_obj = {
                                    {"id", id},
                                    {"title", supervisory_command::cancelMission},
                                    {"type", mission["type"]},
                                    {"end_id", mission["end_id"]},
                                    {"mission_direction", mission["direction"]}};

                                std::string table_string = json_obj.dump();
                                //database->createLocal(table_name, table_string);

                                msnMission.id = id;
                                msnMission.command = supervisory_command::cancelMission;
                                msnMission.type = mission["type"];
                                msnMission.end_id = mission["end_id"];
                                msnMission.mission_direction = mission["mission_direction"];
                                publisher_mission->publish(msnMission);

                                publisherToPlcMission(mission_status_plc::cancel, robot_type_plc::error, message_code_plc::cancel, message_plc::msnCancel);
                            }

                            catch (const nlohmann::json::type_error &e)
                            {
                                std::cerr << "Type error in JSON parsing: " << e.what() << std::endl;
                            }
                        }
                        else if (request.code == supervisory_command::pauseMission)
                        {
                            try
                            {
                                std::string table_name = "mission";
                                nlohmann::json json_message = nlohmann::json::parse(payload);
                                auto object_message = json_message["data"];
                                int id = object_message["id"].get<int>();

                                auto mission = database->missionFindOne(id);
                                nlohmann::json json_obj = {
                                    {"id", id},
                                    {"title", supervisory_command::pauseMission},
                                    {"type", mission["type"]},
                                    {"end_id", mission["end_id"]},
                                    {"mission_direction", mission["mission_direction"]}};

                                std::string table_string = json_obj.dump();
                                database->createLocal(table_name, table_string);

                                msnMission.id = id;
                                msnMission.command = supervisory_command::pauseMission;
                                msnMission.type = mission["type"];
                                msnMission.end_id = mission["end_id"];
                                msnMission.mission_direction = mission["mission_direction"];
                                publisher_mission->publish(msnMission);

                                publisherToPlcMission(mission_status_plc::pause, robot_type_plc::info, message_code_plc::pause, message_plc::msnPause);
                            }
                            catch (const nlohmann::json::type_error &e)
                            {
                                std::cerr << "Type error in JSON parsing: " << e.what() << std::endl;
                            }
                        }
                        else if (request.code == supervisory_command::continueMission)
                        {
                            try
                            {
                                std::string table_name = "mission";
                                nlohmann::json json_message = nlohmann::json::parse(payload);
                                auto object_message = json_message["data"];
                                int id = object_message["id"].get<int>();

                                auto mission = database->missionFindOne(id);
                                nlohmann::json json_obj = {
                                    {"id", id},
                                    {"title", supervisory_command::continueMission},
                                    {"type", mission["type"]},
                                    {"end_id", mission["end_id"]},
                                    {"mission_direction", mission["mission_direction"]}};

                                std::string table_string = json_obj.dump();
                                database->createLocal(table_name, table_string);

                                msnMission.id = id;
                                msnMission.command = supervisory_command::continueMission;
                                msnMission.type = mission["type"];
                                msnMission.end_id = mission["end_id"];
                                msnMission.mission_direction = mission["mission_direction"];
                                publisher_mission->publish(msnMission);
                            }
                            catch (const nlohmann::json::type_error &e)
                            {
                                std::cerr << "Type error in JSON parsing: " << e.what() << std::endl;
                            }
                        }
                        else if (request.code == supervisory_command::heartbit)
                        {
                            nlohmann::json json_obj = {
                                {"code", CODE_REF},
                                {"command", supervisory_command::heartbit},
                                {"data", {{"response", "ok"}}}};

                            std::string payload = json_obj.dump();

                            auto topic = "/" + std::to_string(CODE_REF) + config.topicRemoteSystemResponse;
                            mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
                            client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));

                            hearbit = true;
                        }
                    }
                    catch (const std::exception &e)
                    {
                        RCLCPP_ERROR(this->get_logger(), "Error in topic analysis: %s", e.what());
                    }
                }
                else if (topic.find("RESPONSE") != std::string::npos)
                {
                    try
                    {
                        drn_integration_communication::Command::Result response = command->command(*database, CODE_REF, payload);
                        // RCLCPP_INFO(this->get_logger(), "Command response: %s", response.code.c_str());

                        if (response.code == supervisory_command::vehicleRegistration)
                        {
                            if (response.status)
                            {
                                STATUS_CODE = vehicles::confirmationRegistration;
                                databaseToRosParams(response.code);
                            }
                            else
                            {
                                databaseToRosParams("nok");
                            }
                        }
                        else if (response.code == supervisory_command::startMission)
                        {
                            try
                            {
                                std::string table_name = "mission";
                                nlohmann::json json_message = nlohmann::json::parse(payload);
                                auto object_message = json_message["data"];
                                bool response = object_message["response"].get<bool>();
                                std::string message_ = object_message["message"].get<std::string>();

                                if (!response)
                                {
                                    publisherToPlcMission(mission_status_plc::noMission, robot_type_plc::warning, message_code_plc::cancel, message_);
                                }
                            }
                            catch (const nlohmann::json::type_error &e)
                            {
                                std::cerr << "Type error in JSON parsing: " << e.what() << std::endl;
                            }
                        }
                    }
                    catch (const std::exception &e)
                    {
                        RCLCPP_ERROR(this->get_logger(), "Error in topic analysis: %s", e.what());
                    }
                }
            }
                
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
        }
    }
    void CommunicationMqttNode::setPose(double pos_x, double pos_y, double x_, double y_, double z_, double w_)
    {
        geometry_msgs::msg::PoseWithCovarianceStamped initial_msg;

        initial_msg.header.frame_id = "map";
        initial_msg.header.stamp = this->get_clock()->now();
        initial_msg.pose.pose.position.x = pos_x;
        initial_msg.pose.pose.position.y = pos_y;
        initial_msg.pose.pose.position.z = 0.0;
        initial_msg.pose.pose.orientation.x = x_;
        initial_msg.pose.pose.orientation.y = y_;
        initial_msg.pose.pose.orientation.z = z_;
        initial_msg.pose.pose.orientation.w = w_;

        for (size_t i = 0; i < 36; i++)
        {
            initial_msg.pose.covariance[i] = (i % 7 == 0) ? 0.1 : 0.0;
        }

        publisher_initial_pose->publish(initial_msg);
    }
    void CommunicationMqttNode::periodicMqttPublish()
    {
        while (rclcpp::ok())
        {
            if (CODE_REF == 0)
            {
                CODE_REF = general->createUniqueCode();
                dynamic_topics_ = {"/" + std::to_string(CODE_REF) + "/REQUEST", "/" + std::to_string(CODE_REF) + "/RESPONSE"};

                for (const auto &topic : dynamic_topics_)
                {
                    client_.subscribe(topic, 1);
                }
            }
            if (STATUS_CODE == vehicles::notRegistered)
            {
                registerRobot();
            }
            else if (STATUS_CODE == vehicles::registered)
            {
                STATUS_CODE = vehicles::stopRegistration;

                dynamic_topics_ = {"/" + std::to_string(CODE_REF) + "/REQUEST", "/" + std::to_string(CODE_REF) + "/RESPONSE"};

                for (const auto &topic : dynamic_topics_)
                {
                    client_.subscribe(topic, 1);
                }

                registerRobot();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
        }
    }
    void CommunicationMqttNode::publishNavigation(int select)
    {
        auto msn = std_msgs::msg::Int32();
        if (select == navigation::finishMappingAndSaveMap)
        {
            msn.data = navigation::finishMappingAndSaveMap;
            publisher_switch_nav_mode->publish(msn);
        }
        else if (select == navigation::startMapping)
        {
            msn.data = navigation::startMapping;
            publisher_switch_nav_mode->publish(msn);
        }
        else if (select == navigation::cancelMapping)
        {
            msn.data = navigation::cancelMapping;
            publisher_switch_nav_mode->publish(msn);
        }
        else if (select == navigation::startCostmap)
        {
            msn.data = navigation::startCostmap;
            publisher_switch_nav_mode->publish(msn);
        }
        else if (select == navigation::restartCostmap)
        {

            msn.data = navigation::stopCostmap;
            publisher_switch_nav_mode->publish(msn);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            msn.data = navigation::startCostmap;
            publisher_switch_nav_mode->publish(msn);
        }
        else if (select == navigation::stopCostmap)
        {
            msn.data = navigation::stopCostmap;
            publisher_switch_nav_mode->publish(msn);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    void CommunicationMqttNode::removeRosParams()
    {
        auto parameter_list = this->list_parameters({}, 10);
        for (const auto &param : parameter_list.names)
        {
            if (this->has_parameter(param))
            {
                try
                {
                    this->undeclare_parameter(param);
                    RCLCPP_INFO(this->get_logger(), "Parâmetro removido: %s", param.c_str());
                }
                catch (const std::exception &e)
                {
                    RCLCPP_WARN(this->get_logger(), "Falha ao remover parâmetro %s: %s", param.c_str(), e.what());
                }
            }
        }
    }
    void CommunicationMqttNode::databaseToRosParams(std::string code)
    {
        auto nameAnchort = "anchort";
        auto nameMap = "map";
        auto nameNodeConnection = "node_connection";
        auto nameNode = "node";
        auto nameZoneType = "zone_type";
        auto nameNodeType = "node_type";
        auto nameZone = "zone";
        auto nameVehicle = "vehicle";

        if (code == supervisory_command::mapUpdate || code == supervisory_command::vehicleRegistration)
        {
            auto anchort = database->anchortGetAll();
            if (this->has_parameter(nameAnchort))
            {
                this->set_parameter(rclcpp::Parameter(nameAnchort, anchort));
            }

            auto map = database->mapGetAll();
            if (this->has_parameter(nameMap))
            {
                this->set_parameter(rclcpp::Parameter(nameMap, map));
                rosParamAccessControl->setRosParamService(ros_param::decisionMaker, nameMap, map);
            }

            auto nodeConnection = database->nodeConnectionGetAll();
            if (this->has_parameter(nameNodeConnection))
            {
                this->set_parameter(rclcpp::Parameter(nameNodeConnection, nodeConnection));
                rosParamAccessControl->setRosParamService(service::routePlanningService, nameNodeConnection, nodeConnection);
            }

            auto nodeType = database->nodeTypeGetAll();
            if (this->has_parameter(nameNodeType))
            {
                this->set_parameter(rclcpp::Parameter(nameNodeType, nodeType));
                rosParamAccessControl->setRosParamService(service::routePlanningService, nameNodeType, nodeType);
            }

            auto node = database->nodeGetAll();
            nlohmann::json nodeJson = nlohmann::json::parse(node);

            if (this->has_parameter(nameNode))
            {
                nlohmann::json filteredNodes = nlohmann::json::array();
                for (const auto &n : nodeJson)
                {
                    if (n.contains("deleted_supervisory") && n["deleted_supervisory"] == "false")
                    {
                        filteredNodes.push_back(n);
                    }
                }

                if (!filteredNodes.empty())
                {
                    std::string filteredNodesStr = filteredNodes.dump();
                    this->set_parameter(rclcpp::Parameter(nameNode, filteredNodesStr));
                    rosParamAccessControl->setRosParamService(service::routePlanningService, nameNode, filteredNodesStr);
                }
            }

            std::vector<nlohmann::json> settings = database->settingGetAll();
            for (const auto &setting : settings)
            {
                if (setting.contains("key") && setting.contains("value"))
                {
                    std::string key = setting["key"].get<std::string>();
                    std::string value = setting["value"].get<std::string>();
                    std::string hierarchical_key = "setting/" + key;
                    if (this->has_parameter(hierarchical_key))
                    {
                        this->set_parameter(rclcpp::Parameter(hierarchical_key, value));
                    }
                    else
                    {
                        this->declare_parameter(hierarchical_key, value);
                        this->set_parameter(rclcpp::Parameter(hierarchical_key, value));
                    }

                    if (key == "dockDetectionAttempts")
                    {
                        rosParamAccessControl->setRosParamService(ros_param::decisionMaker, hierarchical_key, value);
                    }
                }
            }

            auto zoneType = database->zoneTypeGetAll();
            if (this->has_parameter(nameZoneType))
            {
                this->set_parameter(rclcpp::Parameter(nameZoneType, zoneType));
            }

            auto zone = database->zoneGetAll();
            if (this->has_parameter(nameZone))
            {
                this->set_parameter(rclcpp::Parameter(nameZone, zone));
            }

            RCLCPP_INFO(this->get_logger(), "Parâmetros publicados com sucesso.");
        }
        else if (code == supervisory_command::initialParam)
        {
            RCLCPP_INFO(this->get_logger(), "Declarando e publicando parâmetros...");

            auto vehicle = database->vehicleGetAll();
            this->declare_parameter(nameVehicle, vehicle);
            this->set_parameter(rclcpp::Parameter(nameVehicle, vehicle));
            rosParamAccessControl->setRosParamService(ros_param::decisionMaker, nameVehicle, vehicle);

            auto anchort = database->anchortGetAll();
            this->declare_parameter(nameAnchort, anchort);
            this->set_parameter(rclcpp::Parameter(nameAnchort, anchort));

            auto map = database->mapGetAll();
            this->declare_parameter(nameMap, map);
            this->set_parameter(rclcpp::Parameter(nameMap, map));
            rosParamAccessControl->setRosParamService(ros_param::decisionMaker, nameMap, map);

            auto nodeConnection = database->nodeConnectionGetAll();
            this->declare_parameter(nameNodeConnection, nodeConnection);
            this->set_parameter(rclcpp::Parameter(nameNodeConnection, nodeConnection));
            rosParamAccessControl->setRosParamService(service::routePlanningService, nameNodeConnection, nodeConnection);

            auto nodeType = database->nodeTypeGetAll();
            this->declare_parameter(nameNodeType, nodeType);
            this->set_parameter(rclcpp::Parameter(nameNodeType, nodeType));
            rosParamAccessControl->setRosParamService(service::routePlanningService, nameNodeType, nodeType);

            auto node = database->nodeGetAll();
            nlohmann::json nodeJson = nlohmann::json::parse(node);
            this->declare_parameter(nameNode, node);
            rosParamAccessControl->setRosParamService(service::routePlanningService, nameNode, node);
            if (!nodeJson.empty() && nodeJson[0]["deleted_supervisory"] == "false")
            {
                this->set_parameter(rclcpp::Parameter(nameNode, node));
                rosParamAccessControl->setRosParamService(service::routePlanningService, nameNode, node);
            }

            std::vector<nlohmann::json> settings = database->settingGetAll();

            for (const auto &setting : settings)
            {
                if (setting.contains("key") && setting.contains("value"))
                {
                    std::string key = setting["key"].get<std::string>();
                    std::string value = setting["value"].get<std::string>();
                    std::string hierarchical_key = "setting/" + key;
                    this->declare_parameter(hierarchical_key, value);
                    this->set_parameter(rclcpp::Parameter(hierarchical_key, value));

                    if (key == "dockDetectionAttempts")
                    {
                        rosParamAccessControl->setRosParamService(ros_param::decisionMaker, hierarchical_key, value);
                    }
                }
            }
            auto zoneType = database->zoneTypeGetAll();
            this->declare_parameter(nameZoneType, zoneType);
            this->set_parameter(rclcpp::Parameter(nameZoneType, zoneType));

            auto zone = database->zoneGetAll();
            this->declare_parameter(nameZone, zone);
            this->set_parameter(rclcpp::Parameter(nameZone, zone));

            RCLCPP_INFO(this->get_logger(), "Parâmetros publicados com sucesso!");
        }
    }
    void CommunicationMqttNode::initVehicleCode()
    {
        std::string msn = "";
        drn_integration_communication::Command::Result response = command->command(*database, 0, msn);

        if (response.status)
        {
            CODE_REF = std::stoull(response.code);
            STATUS_CODE = vehicles::registered;
        }
    }
    void CommunicationMqttNode::reconfigurationRobot()
    {
        delete database;
        delete command;
        delete general;

        database = new drn_integration_communication::Database(config.dbConnectionString);
        command = new drn_integration_communication::Command();
        general = new drn_integration_communication::General();
        database->migration();

        auto vehicle = database->getLastMission();
    }

    void CommunicationMqttNode::cancelUnfinishedMissionsCorrectly()
    {
        auto mission = database->getLastMission();
        int mission_id = 0;
        if (mission.contains("id") && !mission["id"].is_null())
        {
            mission_id = mission["id"].get<int>();
            try
            {
                nlohmann::json json_obj = {
                {"code", CODE_REF},
                {"command", supervisory_command::cancelMission},
                {"data", {{"id", static_cast<int16_t>(mission_id)}, {"response", static_cast<bool>(true)}}}};

                std::string payload = json_obj.dump();
                auto topic = "/" + std::to_string(CODE_REF) + config.topicRemoteSystemResponse;
                mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
                client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));
            }
            catch (const nlohmann::json::type_error &e)
            {
                std::cerr << "Type error in JSON parsing: " << e.what() << std::endl;
            }
        }
    }

    void CommunicationMqttNode::registerRobot()
    {
        auto message = std_msgs::msg::String();
        std::string codeStr = std::to_string(CODE_REF);
        message.data = "{\"code\":\"" + codeStr + "\",\"command\":\"0xCF\",\"data\":{\"code\":\"" + codeStr + "\",\"model\":\"" + config.model + "\"}}";

        mqtt::message_ptr pubmsg = mqtt::make_message(config.topicRemoteSystemRequest, message.data);
        pubmsg->set_qos(1);
        try
        {
            client_.publish(pubmsg)->wait_for(std::chrono::milliseconds(config.publishTimeout));
        }
        catch (const mqtt::exception &exc)
        {
            RCLCPP_ERROR(this->get_logger(), "MQTT Publish Error: %s", exc.what());
        }

        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    void CommunicationMqttNode::publisherToPlcMission(int missionStatus, int messageType, int messageCode, std::string message)
    {
        drc_flexnav_controllers_msgs::msg::PlcMissionResponse plc_mission_response;

        plc_mission_response.header.stamp = this->get_clock()->now();
        plc_mission_response.header.frame_id = "";
        plc_mission_response.mission_status = missionStatus;
        plc_mission_response.message_type = messageType;
        plc_mission_response.message_code = messageCode;
        plc_mission_response.message = message;

        RCLCPP_INFO(this->get_logger(), "Publishing PLC mission response: %s", plc_mission_response.message.c_str());

        publisher_to_plc_mission->publish(plc_mission_response);
    }
    void CommunicationMqttNode::setPoseInitial()
    {
        RCLCPP_INFO(this->get_logger(), "Set initial pose");

        auto poseOpt = database->getPoseById(1);
        if (poseOpt.has_value())
        {
            Pose pose = poseOpt.value();
            setPose(pose.position_x, pose.position_y, pose.orientation_x, pose.orientation_y, pose.orientation_z, pose.orientation_w);
        }
        else
        {
            RCLCPP_ERROR(this->get_logger(), "Starting pose not found");
        }
    }
    void CommunicationMqttNode::call_script()
    {
        const std::string pkg = ament_index_cpp::get_package_share_directory("drc_drn_bringup");
        const std::filesystem::path shutdown     = std::filesystem::path(pkg) / "scripts" / "shutdown.bash";

        auto make_executable_and_run = [this](const std::filesystem::path& scriptPath)
        {
            RCLCPP_INFO(this->get_logger(), "Script path: %s", scriptPath.string().c_str());

            std::error_code ec;
            std::filesystem::permissions(
                scriptPath,
                std::filesystem::perms::owner_exec | std::filesystem::perms::owner_read,
                std::filesystem::perm_options::add,
                ec
            );

            if (ec)
            {
                RCLCPP_WARN(this->get_logger(), "Falha ao ajustar permissões de %s: %s",
                            scriptPath.string().c_str(), ec.message().c_str());
            }

            std::string cmd = "bash \"" + scriptPath.string() + "\"";
            int rc = std::system(cmd.c_str());

            if (rc != 0)
            {
                RCLCPP_ERROR(this->get_logger(), "Falha ao executar script (%d): %s", rc, cmd.c_str());
            }
        };

        make_executable_and_run(shutdown);
    }
}
int main(int argc, char *argv[])
{
    const std::string packagePath = ament_index_cpp::get_package_share_directory("drn_integration_communication");
    const std::string configFilePath = packagePath + "/config/config.yaml";
    config = general->loadConfig(configFilePath);

    rclcpp::init(argc, argv);
    rclcpp::executors::MultiThreadedExecutor executor;
    auto node = std::make_shared<drn_integration_communication::CommunicationMqttNode>();

    node->setPoseInitial();

    executor.add_node(node);
    executor.spin();

    rclcpp::shutdown();
    printf("Commmunication integration");
    return 0;
}

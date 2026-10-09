#include <drn_integration_communication/services/specialized_services/general_service.hpp>

namespace drn_integration_communication
{
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::uint64_t General::createUniqueCode()
    {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        auto positive_milliseconds = std::abs(milliseconds);

        std::cout << "Timestamp atual (positivo) em milissegundos: " << positive_milliseconds << std::endl;

        return milliseconds;
    }

    bool General::is_base64(unsigned char c)
    {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }

    std::vector<unsigned char> General::base64Decode(const std::string &encoded_string)
    {
        int in_len = encoded_string.size();
        int i = 0;
        int j = 0;
        int in_ = 0;
        unsigned char char_array_4[4], char_array_3[3];
        std::vector<unsigned char> decoded_data;

        while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
        {
            char_array_4[i++] = encoded_string[in_];
            in_++;
            if (i == 4)
            {
                for (i = 0; i < 4; i++)
                    char_array_4[i] = base64_chars.find(char_array_4[i]);

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; (i < 3); i++)
                    decoded_data.push_back(char_array_3[i]);
                i = 0;
            }
        }

        if (i)
        {
            for (j = i; j < 4; j++)
                char_array_4[j] = 0;

            for (j = 0; j < 4; j++)
                char_array_4[j] = base64_chars.find(char_array_4[j]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (j = 0; (j < i - 1); j++)
                decoded_data.push_back(char_array_3[j]);
        }

        return decoded_data;
    }

    std::string General::base64Encode2(const std::vector<uchar> &buffer)
    {
        std::string encoded_string;
        unsigned char const *bytes_to_encode = buffer.data();
        size_t in_len = buffer.size();
        int i = 0;
        int j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];

        while (in_len--)
        {
            char_array_3[i++] = *(bytes_to_encode++);
            if (i == 3)
            {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; (i < 4); i++)
                    encoded_string += base64_chars[char_array_4[i]];
                i = 0;
            }
        }

        if (i)
        {
            for (j = i; j < 3; j++)
                char_array_3[j] = '\0';

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

            for (j = 0; (j < i + 1); j++)
                encoded_string += base64_chars[char_array_4[j]];

            while ((i++ < 3))
                encoded_string += '=';
        }

        return encoded_string;
    }

    bool General::saveAsPGM(const cv::Mat &image, const std::string &output_path)
    {
        if (image.channels() != 1)
        {
            std::cerr << "Error: The image is not in grayscale (1 channel) format." << std::endl;
            return false;
        }

        std::ofstream file(output_path, std::ios::binary);
        if (!file.is_open())
        {
            std::cerr << "Error opening file to save in PGM." << std::endl;
            return false;
        }

        file << "P5\n"
             << image.cols << " " << image.rows << "\n"
             << "255\n";

        file.write(reinterpret_cast<const char *>(image.data), image.total());
        file.close();
        std::cout << "Image saved as PGM in: " << output_path << std::endl;

        if (fileExists(output_path))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool General::saveFiltersAsPGM(const cv::Mat &image_, const std::string &type, const std::string &output_path, const nlohmann::json maps,
                                   const std::stringstream &stream_resolution, const double maximumReferenceSpeed)
    {
        auto zones = maps["zones"];
        // cv::Mat image = image_.clone();
        cv::Mat image(image_.size(), image_.type(), cv::Scalar(255, 255, 255));

        for (const auto &jt : zones)
        {
            std::string type_zone_name = jt.at("zoneType").at("name").get<std::string>();
            if (type_zone_name == "Limite de Velocidade" || type_zone_name == "Restrita")
            {
                int gray_color = 0;
                int id = jt.at("id").get<int>();
                std::string max_velocity = "0";

                if (!jt.at("maxVelocity").is_null())
                {
                    if (type == "map_keepout")
                    {
                        continue;
                    }
                    else
                    {
                        max_velocity = std::to_string(jt.at("maxVelocity").get<double>());
                        gray_color = calculateVelocityGradient(max_velocity, maximumReferenceSpeed);
                    }
                }
                else
                {
                    if (type == "map_speeds")
                    {
                        continue;
                    }
                }

                nlohmann::json area_json = jt.at("area");
                std::vector<std::vector<double>> area;
                std::vector<cv::Point> points;
                for (const auto &coord_pair : area_json)
                {
                    double x = coord_pair[0].get<double>();
                    double y = coord_pair[1].get<double>();
                    // std::cerr << "point x: " << x << std::endl;
                    // std::cerr << "point y: " << y << std::endl;
                    // std::cerr << "image.rows: " << image.rows << std::endl;
                    x = x / std::stod(stream_resolution.str());
                    x = image.rows - x;
                    y = y / std::stod(stream_resolution.str());
                    points.push_back(cv::Point(static_cast<int>(y), static_cast<int>(x)));
                    // std::cerr << "point: " << points[0] << std::endl;
                }

                std::vector<std::vector<cv::Point>> polygons = {points};
                // std::cerr << "point gray_color: " << gray_color << std::endl;
                cv::fillPoly(image, polygons, cv::Scalar(gray_color));
            }
        }
        // cv::imshow(type, image);
        // cv::waitKey(0);

        if (image.channels() != 1)
        {
            std::cerr << "Error: The image is not in grayscale (1 channel) format." << std::endl;
            return false;
        }

        std::ofstream file(output_path, std::ios::binary);
        if (!file.is_open())
        {
            std::cerr << "Error opening file to save in PGM." << std::endl;
            return false;
        }

        file << "P5\n"
             << image.cols << " " << image.rows << "\n"
             << "255\n";

        file.write(reinterpret_cast<const char *>(image.data), image.total());
        file.close();
        std::cout << "Image saved as PGM in: " << output_path << std::endl;

        if (fileExists(output_path))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    double General::normalizeAndOffsetAngle(double angle)
    {
        double angle_adjusted = fmod(90 - angle, 360.0);
        if (angle_adjusted < 0)
        {
            angle_adjusted += 360.0;
        }
        return angle_adjusted;
    }

    std::int32_t General::calculateVelocityGradient(const std::string max_velocity, double maximumReferenceSpeed)
    {
        double velocity = std::stof(max_velocity);
        int32_t result = (velocity * 255) / maximumReferenceSpeed;

        return result;
    }

    double General::inverseNormalizeAndOffsetAngle(double angle)
    {
        double angle_original = 90.0 - angle;
        if (angle_original < 0)
        {
            angle_original += 360.0;
        }
        return angle_original;
    }

    bool General::saveAsYaml(const std::string &type, const std::string &output_path, const std::string &mode,
                             const std::stringstream &stream_resolution,
                             const std::stringstream &stream_x,
                             const std::stringstream &stream_y,
                             const std::stringstream &stream_z,
                             const std::stringstream &stream_negate,
                             const std::stringstream &stream_occupied_thresh,
                             const std::stringstream &stream_free_thresh)
    {
        YAML::Node yaml;
        yaml["image"] = "map.pgm";
        yaml["mode"] = type == "map_speeds" ? "scale" : mode;
        yaml["resolution"] = stream_resolution.str();
        yaml["origin"] = YAML::Node(YAML::NodeType::Sequence);
        yaml["origin"].push_back(0); // stream_x.str()
        yaml["origin"].push_back(0); // stream_y.str()
        yaml["origin"].push_back(0); // stream_z.str()
        yaml["origin"].SetStyle(YAML::EmitterStyle::Flow);
        yaml["negate"] = stream_negate.str();
        yaml["occupied_thresh"] = type == "map_speeds" ? "1.0" : stream_occupied_thresh.str();
        yaml["free_thresh"] = type == "map_speeds" ? "0.0" : stream_free_thresh.str();
        std::ofstream fout(output_path);
        fout << yaml;
        fout.close();

        if (fileExists(output_path))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool General::fileExists(const std::string &filename)
    {
        std::ifstream file(filename);
        return file.good();
    }

    Config General::loadConfig(const std::string &filePath)
    {
        Config config;
        try
        {
            YAML::Node configFile = YAML::LoadFile(filePath);

            config.mqttServerAddress = configFile["mqtt"]["server_address"].as<std::string>();
            config.mqttUsername = configFile["mqtt"]["username"].as<std::string>();
            config.mqttPassword = configFile["mqtt"]["password"].as<std::string>();
            config.dbConnectionString = configFile["database"]["connection_string"].as<std::string>();
            config.topicRemoteSystemRequest = configFile["parameters"]["topic_remote_system_request"].as<std::string>();
            config.topicRemoteSystemResponse = configFile["parameters"]["topic_remote_system_response"].as<std::string>();
            config.topicMappingMap = configFile["parameters"]["topic_mapping_map"].as<std::string>();
            config.topicMissionStatus = configFile["parameters"]["topic_mission_status"].as<std::string>();
            config.topicMissionLog = configFile["parameters"]["topic_mission_log"].as<std::string>();
            config.topicTelemetryPose = configFile["parameters"]["topic_telemetry_pose"].as<std::string>();
            config.topicTelemetryBattery = configFile["parameters"]["topic_telemetry_battery"].as<std::string>();
            config.topicTelemetryVelocity = configFile["parameters"]["topic_telemetry_velocity"].as<std::string>();
            config.topicTelemetryAlert = configFile["parameters"]["topic_telemetry_alert"].as<std::string>();
            config.topicTelemetryLogs = configFile["parameters"]["topic_telemetry_logs"].as<std::string>();
            config.topicRobotStatus = configFile["parameters"]["topic_robot_status"].as<std::string>();
            config.topicRobotMode = configFile["parameters"]["topic_robot_mode"].as<std::string>();
            config.codeRobotStatus = configFile["parameters"]["code_robot_status"].as<uint64_t>();
            config.codeRobotMode = configFile["parameters"]["code_robot_mode"].as<uint64_t>();
            config.model = configFile["parameters"]["model"].as<std::string>();
            config.timerGlobalMapPublish = configFile["parameters"]["timer_global_map_publish"].as<int16_t>();
            config.timerGlobalPosePublish = configFile["parameters"]["timer_global_pose_publish"].as<int16_t>();
            config.timerGlobalAmclPosePublish = configFile["parameters"]["timer_global_amcl_pose_publish"].as<int16_t>();
            config.timerGlobalVelocityPublish = configFile["parameters"]["timer_global_velocity_publish"].as<int16_t>();
            config.timerGlobalBatteryPublish = configFile["parameters"]["timer_global_battery_publish"].as<int16_t>();
            config.timerPlcPublish = configFile["parameters"]["timer_plc_publish"].as<int16_t>(); 
            config.timerHearbitPublish = configFile["parameters"]["timer_hearbit_publish"].as<int16_t>();
            config.timeoutHardbit = configFile["parameters"]["timeout_hardbit"].as<int16_t>();           
            config.buffer = configFile["parameters"]["buffer"].as<int16_t>();
            config.publishTimeout = configFile["parameters"]["publish_timeout"].as<int16_t>();
            config.setKeepAliveInterval = configFile["parameters"]["set_keep_alive_interval"].as<int16_t>();
            config.sleepMqttReconnectLoop = configFile["parameters"]["sleep_mqtt_reconnect_loop"].as<int16_t>();
        }
        catch (const YAML::Exception &e)
        {
            std::cerr << "Erro ao carregar arquivo de configuração: " << e.what() << std::endl;
        }
        return config;
    }
}
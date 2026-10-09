#ifndef DRN_INTEGRATION_COMUNICATION_GENERAL_H
#define DRN_INTEGRATION_COMUNICATION_GENERAL_H

#include <stdio.h>
#include <vector>
#include <functional>
#include <numeric>
#include <iostream>
#include <string>
#include <chrono>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>

namespace drn_integration_communication
{
    struct Config
    {
        std::string mqttServerAddress;
        std::string mqttUsername;
        std::string mqttPassword;
        std::string dbConnectionString;
        std::string topicRemoteSystemRequest;
        std::string topicRemoteSystemResponse;
        std::string topicMappingMap;
        std::string topicMissionStatus;
        std::string topicMissionLog;
        std::string topicTelemetryPose;
        std::string topicTelemetryBattery;
        std::string topicTelemetryVelocity;
        std::string topicTelemetryAlert;
        std::string topicTelemetryLogs;
        std::string topicRobotStatus;
        std::string topicRobotMode;
        uint64_t codeRobotStatus;
        uint64_t codeRobotMode;
        std::string model;
        int16_t timerGlobalMapPublish;
        int16_t timerGlobalPosePublish;
        int16_t timerGlobalAmclPosePublish;
        int16_t timerGlobalVelocityPublish;
        int16_t timerGlobalBatteryPublish;
        int16_t timerPlcPublish;
        int16_t timerHearbitPublish;
        int16_t timeoutHardbit;
        int16_t buffer;
        int16_t publishTimeout;
        int16_t setKeepAliveInterval;
        int16_t sleepMqttReconnectLoop;
    };

    class General
    {
    public:
        std::uint64_t createUniqueCode();
        bool is_base64(unsigned char c);
        std::vector<unsigned char> base64Decode(const std::string &encoded_string);
        std::string base64Encode2(const std::vector<uchar> &buffer);
        bool saveAsPGM(const cv::Mat &image, const std::string &output_path);
        bool fileExists(const std::string &filename);
        bool saveAsYaml(const std::string &type, const std::string &output_path, const std::string &mode,
                        const std::stringstream &stream_resolution,
                        const std::stringstream &stream_x,
                        const std::stringstream &stream_y,
                        const std::stringstream &stream_z,
                        const std::stringstream &stream_negate,
                        const std::stringstream &stream_occupied_thresh,
                        const std::stringstream &stream_free_thresh);

        std::int32_t calculateVelocityGradient(const std::string max_velocity, const double maximumReferenceSpeed);
        bool saveFiltersAsPGM(const cv::Mat &image, const std::string &type, const std::string &output_path, const nlohmann::json maps, const std::stringstream &stream_resolution, const double maximumReferenceSpeed);
        double normalizeAndOffsetAngle(double angle);
        double normalizeAndOffsetAngle(double x_, double y_, double z_, double w_);
        Config loadConfig(const std::string &filePath);
        double inverseNormalizeAndOffsetAngle(double angle);
    };
}
#endif
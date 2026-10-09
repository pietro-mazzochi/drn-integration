
#ifndef DRN_INTEGRATION_ROS_PARAM_CONTROL_SERVICE_H
#define DRN_INTEGRATION_ROS_PARAM_CONTROL_SERVICE_H

#include <chrono>
#include <memory>
#include <functional>
#include "rclcpp/parameter_client.hpp"
#include <rclcpp/rclcpp.hpp>

namespace drn_integration_communication
{
    class RosParamAccessControlService
    {
    public:
        std::string getRosParamService(std::string node, std::string param);
        bool setRosParamService(std::string node, std::string param, std::string value);
    };
}
#endif
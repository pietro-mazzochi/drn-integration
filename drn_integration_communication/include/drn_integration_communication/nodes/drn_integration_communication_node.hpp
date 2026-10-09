#ifndef DRN_INTEGRATION_NODE_HPP
#define DRN_INTEGRATION_NODE_HPP

#include <rclcpp/rclcpp.hpp>
#include "nav_msgs/msg/occupancy_grid.hpp"
#include "geometry_msgs/msg/pose_with_covariance_stamped.hpp"
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <std_msgs/msg/string.hpp>
#include <std_msgs/msg/int32.hpp>
#include "std_msgs/msg/float64.hpp"
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <yaml-cpp/yaml.h>
#include <cmath>
#include <mqtt/async_client.h>
#include <drn_integration_communication/database/database.hpp>
#include <drn_integration_communication/services/specialized_services/command_service.hpp>
#include <drn_integration_communication/services/specialized_services/general_service.hpp>
#include <drn_integration_communication/services/specialized_services/ros_param_access_control_service.hpp>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <exception>
#include <iomanip>
#include <iterator>
#include <filesystem>
#include <boost/beast/core/detail/base64.hpp>
#include "std_srvs/srv/trigger.hpp"
#include <drn_integration_msgs/msg/mission.hpp>
#include <drc_flexnav_controllers_msgs/msg/plc_mission_response.hpp>
#include <drc_flexnav_controllers_msgs/msg/plc_request.hpp>
#include <drc_flexnav_controllers_msgs/msg/plc_command.hpp> 
#include <drn_integration_msgs/srv/alert.hpp>
#include <drn_integration_msgs/srv/log.hpp>
#include <drn_integration_msgs/srv/mission_log.hpp>
#include <drn_integration_msgs/srv/mission.hpp>
#include <drn_integration_msgs/srv/robot_status.hpp>
#include <drn_integration_msgs/srv/robot_mode.hpp>
#include <drn_integration_communication/communication/communication.hpp>
#include <drn_integration_communication/communication/messages.hpp>
#include <drn_integration_communication/nodes/node_names.hpp>
#include <mqtt/message.h> 
#include "siemens_et200sp_modbus_driver_msgs/msg/siemens_etsp_modbus_driver.hpp"

using namespace std::chrono_literals;
using std::placeholders::_1;
using std::placeholders::_2;
using std_srvs::srv::Trigger;

bool hearbit = false;
int mapOperation = map::notSendMap;
const std::string CLIENT_ID("ros2_mqtt_node");
drn_integration_communication::Config config;
drn_integration_communication::General *general;
drn_integration_communication::RosParamAccessControlService *rosParamAccessControl;

namespace drn_integration_communication
{
    class CommunicationMqttCallback : public virtual mqtt::callback
    {
    private:
        rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;        
        std::queue<mqtt::const_message_ptr> message_queue_; 
        std::mutex queue_mutex_;
        std::condition_variable queue_cv_;

    public:
        CommunicationMqttCallback(rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher)
            : publisher_(publisher) {}

        void message_arrived(mqtt::const_message_ptr msg) override
        {
            auto ros_msg = std_msgs::msg::String();
            ros_msg.data = msg->to_string();
            publisher_->publish(ros_msg);

            {
                std::lock_guard<std::mutex> lock(queue_mutex_);
                message_queue_.push(msg);  // agora armazenamos o ponteiro completo
            }
            queue_cv_.notify_one();
        }
        bool get_message(std::string &topic, std::string &payload)
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            queue_cv_.wait(lock, [this]
                        { return !message_queue_.empty(); });

            if (!message_queue_.empty())
            {
                auto msg_ptr = message_queue_.front();
                message_queue_.pop();

                topic = msg_ptr->get_topic();
                payload = msg_ptr->to_string();  // ou get_payload_str() se preferir

                return true;
            }

            return false;
        }
    };

    class CommunicationMqttNode : public rclcpp::Node
    {
    public:
        CommunicationMqttNode();
        ~CommunicationMqttNode();

        void globalMapReaderNode();
        void poseReaderNode();
        void amclPoseReaderNode();
        void plcRequestNode();
        void velocityReaderNode();
        void batteryReaderNode();
        void siemenst200spNode();
        void heartbitReader();
        void auxNode();
        void initVehicleCode();
        void reconfigurationRobot();
        void cancelUnfinishedMissionsCorrectly();
        void registerRobot();
        void publishNavigation(int select);
        void databaseToRosParams(std::string code);
        void removeRosParams();
        void connectToMqttBroker(mqtt::connect_options &connOpts);
        void mqttReconnectLoop();
        void setPose(double pos_x, double pos_y, double x_, double y_, double z_, double w_);
        void setPoseInitial();
        void call_script();
        void publisherToPlcMission(int missionStatus, int messageType, int messageCode, std::string message);

        void handle_alert_service(const std::shared_ptr<drn_integration_msgs::srv::Alert::Request> request, const std::shared_ptr<drn_integration_msgs::srv::Alert::Response> response);
        void handle_log_service(const std::shared_ptr<drn_integration_msgs::srv::Log::Request> request, const std::shared_ptr<drn_integration_msgs::srv::Log::Response> response);
        void handle_mission_log_service(const std::shared_ptr<drn_integration_msgs::srv::MissionLog::Request> request, const std::shared_ptr<drn_integration_msgs::srv::MissionLog::Response> response);
        void handle_mission_service(const std::shared_ptr<drn_integration_msgs::srv::Mission::Request> request, const std::shared_ptr<drn_integration_msgs::srv::Mission::Response> response);
        void handle_robot_mode_service(const std::shared_ptr<drn_integration_msgs::srv::RobotMode::Request> request, const std::shared_ptr<drn_integration_msgs::srv::RobotMode::Response> response);
        void handle_robot_status_service(const std::shared_ptr<drn_integration_msgs::srv::RobotStatus::Request> request, const std::shared_ptr<drn_integration_msgs::srv::RobotStatus::Response> response);

    private:
        void rosCallback(const std_msgs::msg::String::SharedPtr msg);
        void publishTestMessage();
        void topicAnalysis();
        void publishReceivedMessageToMqtt(const std::string &msg);
        void periodicMqttPublish();
        void globalMapReadeCallback(const nav_msgs::msg::OccupancyGrid::SharedPtr msg);
        void globalMapPublish();
        void poseCallback(const geometry_msgs::msg::PoseWithCovarianceStamped::SharedPtr msg);
        void posePublish();
        void amclPoseCallback(const geometry_msgs::msg::PoseWithCovarianceStamped::SharedPtr msg);
        void amclPosePublish();
        void heartbitPublish();
        void velocityCallback(const std_msgs::msg::Float64::SharedPtr msg);
        void velocityPublish();
        void batteryCallback(const std_msgs::msg::Float64::SharedPtr msg);
        void batteryPublish();
        void plcCallback(const drc_flexnav_controllers_msgs::msg::PlcRequest::SharedPtr msg);
        void siemenst200spCallback(const siemens_et200sp_modbus_driver_msgs::msg::SiemensEtspModbusDriver::SharedPtr msg);
        bool returnDatabase = false;
        int timeout = 0;
        int STATUS_CODE = vehicles::notRegistered;
        std::uint64_t CODE_REF = 0;
        bool public_map = false;

        drn_integration_communication::Database *database;
        drn_integration_communication::Command *command;

        /**
         * Topics
         */
        rclcpp::Subscription<nav_msgs::msg::OccupancyGrid>::SharedPtr subscription_occupancy_grid_;
        rclcpp::Subscription<geometry_msgs::msg::PoseWithCovarianceStamped>::SharedPtr subscription_pose_;
        rclcpp::Subscription<geometry_msgs::msg::PoseWithCovarianceStamped>::SharedPtr subscription_amcl_pose_;
        rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr subscription_velocity_;
        rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr subscription_battery_;
        rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
        rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr publisher_switch_nav_mode;
        rclcpp::Publisher<drn_integration_msgs::msg::Mission>::SharedPtr publisher_mission;
        rclcpp::Publisher<geometry_msgs::msg::PoseWithCovarianceStamped>::SharedPtr publisher_initial_pose;
        rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
        /*Communication PLC*/
        rclcpp::Publisher<drc_flexnav_controllers_msgs::msg::PlcMissionResponse>::SharedPtr publisher_to_plc_mission;
        rclcpp::Publisher<drc_flexnav_controllers_msgs::msg::PlcCommand>::SharedPtr publisher_to_plc_command;
        rclcpp::Subscription<drc_flexnav_controllers_msgs::msg::PlcRequest>::SharedPtr subscription_plc_request_;
        rclcpp::Subscription<siemens_et200sp_modbus_driver_msgs::msg::SiemensEtspModbusDriver>::SharedPtr subscription_siemens_et200sp_data_;

        /**
         *
         * services */
        rclcpp::Service<drn_integration_msgs::srv::Alert>::SharedPtr alert_service_;
        rclcpp::Service<drn_integration_msgs::srv::Log>::SharedPtr log_service_;
        rclcpp::Service<drn_integration_msgs::srv::MissionLog>::SharedPtr mission_log_service_;
        rclcpp::Service<drn_integration_msgs::srv::Mission>::SharedPtr mission_service_;
        rclcpp::Service<drn_integration_msgs::srv::RobotMode>::SharedPtr robot_mode_service_;
        rclcpp::Service<drn_integration_msgs::srv::RobotStatus>::SharedPtr robot_status_service_;       

        /**
         * Aux
         */
        mqtt::async_client client_;
        std::shared_ptr<CommunicationMqttCallback> callback_;
        rclcpp::TimerBase::SharedPtr timerGlobalMap_;
        rclcpp::TimerBase::SharedPtr timerPose_;
        rclcpp::TimerBase::SharedPtr timerAmclPose_;
        rclcpp::TimerBase::SharedPtr timerHeartbit_;
        rclcpp::TimerBase::SharedPtr timerVelocity_;
        rclcpp::TimerBase::SharedPtr timerBattery_;
        rclcpp::TimerBase::SharedPtr timePlc_;
        std::thread topic_analysis_thread_;
        std::thread periodic_publish_thread_;
        std::vector<std::string> dynamic_topics_;
        nav_msgs::msg::OccupancyGrid::SharedPtr occupancyGrid_;
        geometry_msgs::msg::PoseWithCovarianceStamped::SharedPtr pose_;
        geometry_msgs::msg::PoseWithCovarianceStamped::SharedPtr amcl_pose_;
        std_msgs::msg::Float64::SharedPtr velocity_;
        std_msgs::msg::Float64::SharedPtr battery_;
        std::thread reconnect_thread_;
        std::optional<bool> last_autonomous_; 
        std::optional<bool> navigation_ = false; 
    };
}

#endif
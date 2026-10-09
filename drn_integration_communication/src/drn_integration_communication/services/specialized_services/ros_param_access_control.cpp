#include <drn_integration_communication/services/specialized_services/ros_param_access_control_service.hpp>

namespace drn_integration_communication
{
    std::string RosParamAccessControlService::getRosParamService(std::string node, std::string param)
    {
        auto values = std::string();
        int retries = 0;
        const int max_retries = 10;

        auto temp_node = std::make_shared<rclcpp::Node>("temp_param_client");
        auto param_client = std::make_shared<rclcpp::SyncParametersClient>(temp_node, node);

        while (!param_client->wait_for_service(std::chrono::seconds(1)) && rclcpp::ok())
        {
            std::cout << "Waiting for param" << std::endl;
            if (++retries >= max_retries)
            {
                std::cerr << "[ROS PARAM] Timeout waiting for parameter service on node: " << node << std::endl;
                return "";
            }
        }

        try
        {
            values = param_client->get_parameter<std::string>(param);
        }
        catch (const std::exception &e)
        {
            return "";
        }

        return values;
    }

    bool RosParamAccessControlService::setRosParamService(std::string node, std::string param_name, std::string value)
    {

        std::cout << "[ROS PARAM] Waiting for parameter service on node: " << node << std::endl;

        rclcpp::NodeOptions options;
        options.allow_undeclared_parameters(true);
        options.automatically_declare_parameters_from_overrides(true);
    
        auto temp_node = std::make_shared<rclcpp::Node>("temp_param_client", options);
        auto param_client = std::make_shared<rclcpp::SyncParametersClient>(temp_node, node);
    
        const int max_retries = 10;
        int retries = 0;
    
        while (!param_client->wait_for_service(std::chrono::seconds(1)) && rclcpp::ok())
        {
            std::cout << "[ROS PARAM] Waiting for parameter service on node: " << node << std::endl;
    
            if (++retries >= max_retries)
            {
                std::cerr << "[ROS PARAM] Timeout waiting for parameter service on node: " << node << std::endl;
                return false;
            }
        }
    
        try
        {
            rclcpp::Parameter param(param_name, value);
    
            auto results = param_client->set_parameters({param});
    
            if (!results.empty() && results[0].successful)
            {
                std::cout << "[ROS PARAM] Successfully set param: " << param_name << " = " << value << std::endl;
                return true;
            }
            else
            {
                std::cerr << "[ROS PARAM] Failed to set param: " << param_name;
                if (!results.empty())
                {
                    std::cerr << " - Reason: " << results[0].reason << std::endl;
                }
                else
                {
                    std::cerr << " - No result returned" << std::endl;
                }
                return false;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "[ROS PARAM] Exception while setting param '" << param_name << "': " << e.what() << std::endl;
            return false;
        }
    }
    
}

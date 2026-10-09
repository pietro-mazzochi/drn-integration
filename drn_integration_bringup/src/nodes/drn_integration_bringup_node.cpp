#include <rclcpp/rclcpp.hpp>
#include <chrono>
#include <memory>
#include <cstdio>
#include <iostream>
#include <functional>
#include <thread>
#include <mutex>
#include <std_msgs/msg/string.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.hpp>

using namespace std::chrono_literals;
using std::placeholders::_1; 

int main(int argc, char ** argv)
{  
  rclcpp::init(argc, argv);

  (void) argc;
  (void) argv;
  
  printf("hello world bringup package\n");
  return 0;
}
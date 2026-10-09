cmake_minimum_required(VERSION 3.8)

project(drn_integration_communication)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(image_transport REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(rcl_interfaces REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(drn_integration_msgs REQUIRED)
find_package(drc_flexnav_controllers_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(OpenCV REQUIRED)
find_package(siemens_et200sp_modbus_driver_msgs REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

include_directories(
  /usr/local/include
  include
  ${rclcpp_INCLUDE_DIRS}
  ${sensor_msgs_INCLUDE_DIRS}
  ${geometry_msgs_INCLUDE_DIRS}
  ${cv_bridge_INCLUDE_DIRS}
  ${image_transport_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${PostgreSQL_INCLUDE_DIRS}
)

# define a library target called my_value_converter_library
add_library(${PROJECT_NAME} SHARED
  src/${PROJECT_NAME}/database/database.cpp
  src/${PROJECT_NAME}/database/database_migration.cpp
  src/${PROJECT_NAME}/entities/api/alerts.cpp
  src/${PROJECT_NAME}/entities/api/robot.cpp
  src/${PROJECT_NAME}/entities/api/anchors.cpp
  src/${PROJECT_NAME}/entities/api/logs.cpp
  src/${PROJECT_NAME}/entities/api/maps.cpp
  src/${PROJECT_NAME}/entities/api/mission_logs.cpp
  src/${PROJECT_NAME}/entities/api/missions.cpp
  src/${PROJECT_NAME}/entities/api/node_connections.cpp
  src/${PROJECT_NAME}/entities/api/node_types.cpp
  src/${PROJECT_NAME}/entities/api/nodes.cpp
  src/${PROJECT_NAME}/entities/api/settings.cpp
  src/${PROJECT_NAME}/entities/api/telemetries.cpp
  src/${PROJECT_NAME}/entities/api/vehicles.cpp
  src/${PROJECT_NAME}/entities/api/zone_types.cpp
  src/${PROJECT_NAME}/entities/api/zones.cpp
  src/${PROJECT_NAME}/entities/api/pose.cpp
  src/${PROJECT_NAME}/services/api/alerts_service.cpp
  src/${PROJECT_NAME}/services/api/anchors_service.cpp
  src/${PROJECT_NAME}/services/api/logs_service.cpp
  src/${PROJECT_NAME}/services/api/robot_service.cpp
  src/${PROJECT_NAME}/services/api/maps_service.cpp
  src/${PROJECT_NAME}/services/api/mission_logs_service.cpp
  src/${PROJECT_NAME}/services/api/missions_service.cpp
  src/${PROJECT_NAME}/services/api/node_connections_service.cpp
  src/${PROJECT_NAME}/services/api/node_types_service.cpp
  src/${PROJECT_NAME}/services/api/nodes_service.cpp
  src/${PROJECT_NAME}/services/api/settings_service.cpp
  src/${PROJECT_NAME}/services/api/telemetries_service.cpp
  src/${PROJECT_NAME}/services/api/vehicles_service.cpp
  src/${PROJECT_NAME}/services/api/zone_types_service.cpp
  src/${PROJECT_NAME}/services/api/zones_service.cpp
  src/${PROJECT_NAME}/services/api/pose_service.cpp
  src/${PROJECT_NAME}/services/specialized_services/command_service.cpp
  src/${PROJECT_NAME}/services/specialized_services/general_service.cpp
  src/${PROJECT_NAME}/services/specialized_services/ros_param_access_control.cpp
)

ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  rcl_interfaces
  std_msgs
  sensor_msgs
  geometry_msgs
  cv_bridge
  image_transport
  OpenCV
  drn_integration_msgs
  drc_flexnav_controllers_msgs
  tf2
  siemens_et200sp_modbus_driver_msgs
)

# this line to exports the library
ament_export_targets(${PROJECT_NAME} HAS_LIBRARY_TARGET)

# install the include/my_cpp_library directory to the install/include/my_cpp_library
install(
  DIRECTORY include/${PROJECT_NAME}
  DESTINATION include
)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

add_executable(${PROJECT_NAME}_node src/nodes/${PROJECT_NAME}_node.cpp)
target_link_libraries(${PROJECT_NAME}_node ${PROJECT_NAME} pqxx pq /usr/local/lib/libpaho-mqttpp3.so /usr/local/lib/libpaho-mqtt3as.so yaml-cpp)
target_link_directories(${PROJECT_NAME}_node PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/lib)
ament_target_dependencies(${PROJECT_NAME}_node
  OpenCV
  rclcpp
  rcl_interfaces
  std_srvs
  std_msgs
  nav_msgs
  sensor_msgs
  geometry_msgs
  yaml-cpp
  cv_bridge
  image_transport
  drn_integration_msgs
  drc_flexnav_controllers_msgs
  tf2
  tf2_ros
  siemens_et200sp_modbus_driver_msgs
  )

install(TARGETS
  ${PROJECT_NAME}_node
  DESTINATION lib/${PROJECT_NAME}/
)

install(DIRECTORY
  launch
  config
  DESTINATION share/${PROJECT_NAME})

ament_package()

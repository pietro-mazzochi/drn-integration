cmake_minimum_required(VERSION 3.8)

project(drn_integration_bringup)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
 add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rcl_interfaces REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(image_transport REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
ament_lint_auto_find_test_dependencies()
endif()

include_directories(include
  ${rclcpp_INCLUDE_DIRS}
  ${sensor_msgs_INCLUDE_DIRS}
  ${geometry_msgs_INCLUDE_DIRS}
  ${cv_bridge_INCLUDE_DIRS}
  ${image_transport_INCLUDE_DIRS} 
)

# define a library target called my_value_converter_library
add_library(${PROJECT_NAME} SHARED
  src/${PROJECT_NAME}/libexample.cpp
  #...
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
target_link_libraries(${PROJECT_NAME}_node ${PROJECT_NAME})
target_link_directories(${PROJECT_NAME}_node PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/lib)
ament_target_dependencies(${PROJECT_NAME}_node rclcpp rcl_interfaces std_msgs sensor_msgs geometry_msgs cv_bridge image_transport)

install(TARGETS
  ${PROJECT_NAME}_node
  DESTINATION lib/${PROJECT_NAME}/
)

install(DIRECTORY 
  launch 
  DESTINATION share/${PROJECT_NAME})  

ament_package()
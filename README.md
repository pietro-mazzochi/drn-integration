
# DRN Integration

## Overview

This project integrates the main navigation launches for mapping, localization.

### Launch Command

To start the navigation system with mapping, localization, use the following command:

```bash
ros2 launch drn_integration_bringup drn_integration_bringup_launch.py
```

## Installation

To ensure all necessary dependencies are installed, follow the instructions below.

### System Dependencies

1. **Update system packages:**

   ```bash
   sudo apt update
   sudo apt install -y build-essential cmake libssl-dev
   ```

2. **Install PostgreSQL C++ library (libpqxx):**

   ```bash
   sudo apt-get install libpqxx-dev
   ```

3. **Install ROS 2 Humble Packages:**

   - **Point Cloud to Laser Scan Conversion:**

     ```bash
     sudo apt-get install ros-humble-pointcloud-to-laserscan
     ```

   - **Navigation2 Package:**

     ```bash
     sudo apt-get install ros-humble-navigation2
     ```

   - **Nav2 Bringup Package:**

     ```bash
     sudo apt-get install ros-humble-nav2-bringup
     ```

   - **SLAM Toolbox:**

     ```bash
     sudo apt-get install ros-humble-slam-toolbox
     ```

   - **Joy (Joystick) Package:**

     ```bash
     sudo apt-get install -y ros-humble-joy*
     ```

4. **Install Paho MQTT C Library:**

   ```bash
   git clone https://github.com/eclipse/paho.mqtt.c.git
   cd paho.mqtt.c
   cmake -Bbuild -H. -DPAHO_WITH_SSL=ON
   sudo cmake --build build/ --target install
   ```
5. **Install Paho MQTT C++ Library:**

   ```bash
   git clone https://github.com/eclipse/paho.mqtt.cpp.git
   cd paho.mqtt.cpp
   cmake -Bbuild -H. -DPAHO_BUILD_DOCUMENTATION=FALSE -DPAHO_BUILD_SAMPLES=TRUE
   sudo cmake --build build/ --target install
   ```
6. **Run the ldconfig command:**

   ```bash
   sudo ldconfig
   ```
##

**Testing Paho MQTT C++ Installation**

You can use the following C++ code to test if Paho MQTT C++ is installed correctly:

```cpp
#include <mqtt/async_client.h>
#include <iostream>

int main() {
  const std::string SERVER_ADDRESS("tcp://localhost:1883");
  const std::string CLIENT_ID("paho_cpp_async_publish");
  mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);
  std::cout << "Paho MQTT C++ installed successfully!" << std::endl;
  return 0;
}
```

**Compiling the test code:**

Use the following command to compile the test code:

```bash
g++ mqtt_test.cpp -o mqtt_test -lpaho-mqttpp3 -lpaho-mqtt3as
```
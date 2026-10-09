# DRN - integration

## Starting the Communication and Navigation System
To begin communication and navigation, run the following command:
```bash
ros2 launch drn_integration_bringup drn_integration_bringup_launch.py
```

## Start localization with AMCL e costmap

Command to initialize the location, the robot's pose in relation to the global map will be published. The global map will also be published.

```bash
ros2 topic pub --once /switch_nav_mode std_msgs/Int32 '{data: 3}'
```

## Start mapping your environment with Slam Toolbox

Initializes global mapping.

```bash
ros2 topic pub --once /switch_nav_mode std_msgs/Int32 '{data: 1}'
```

## Finalize mapping with Slam Toolbox

Finish mapping and save the map. Two files will be generated: PGM and YAML. In the root directory in the directory called maps.

```bash
ros2 topic pub --once /switch_nav_mode std_msgs/Int32 '{data: 0}'
```

## Start local mapping with Slam Toolbox

Initializes the local map.

```bash
ros2 topic pub --once /switch_nav_mode std_msgs/Int32 '{data: 1}'

```
## Finalize the local map e localuzation

Ends costmap. NOTE: the local map will not be saved.

```bash
ros2 topic pub --once /switch_nav_mode std_msgs/Int32 '{data: 2}'
```
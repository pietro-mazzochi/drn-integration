from launch import LaunchDescription
from launch.actions import (IncludeLaunchDescription,TimerAction,ExecuteProcess,DeclareLaunchArgument,SetEnvironmentVariable)
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_xml.launch_description_sources import XMLLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
from launch.substitutions import LaunchConfiguration
import os

def generate_launch_description():        
    drn_navigation_localization_launch = os.path.join(
        get_package_share_directory("drn_navigation_mapping"),
        "launch",
        "drn_navigation_localization_launch.py",
    )

    drn_navigation_pathplanner_launch = os.path.join(
        get_package_share_directory("drn_navigation_mapping"),
        "launch",
        "drn_navigation_pathplanner_launch.py",
    )              
    
    drn_integration_communication_launch = os.path.join(
        get_package_share_directory("drn_integration_communication"),
        "launch",
        "drn_integration_communication_launch.py",
    )
    
    pointcloud2_to_laserscan_launch = os.path.join(
        get_package_share_directory("drn_integration_pointcloud_to_laserscan"),
        "launch",
        "pointcloud2_to_laserscan_launch.py",
    )

    drn_navigation_mapping_launch = os.path.join(
        get_package_share_directory("drn_navigation_mapping"),
        "launch",
        "drn_navigation_mapping_launch.py",
    )
    drn_navigation_route_planning_launch = os.path.join(
        get_package_share_directory("drn_navigation_route_planning"),
        "launch/route_planning_launch.xml",
    )

    drn_perception_reference_detection_launch = os.path.join(
        get_package_share_directory("drn_perception_reference_detection"),
        "drn_perception_reference_detection_launch.py",
    )

    drn_navigation_docking_action = os.path.join(
        get_package_share_directory("docking_services"),
        "launch",
        "docking_action.launch.py",
    )

    drn_navigation_decision_maker_launch = os.path.join(
        get_package_share_directory("drn_navigation_decision_maker"),
        "launch",
        "decision_maker_launch.py",
    )
    
    start_navigation_script = os.path.join(
        get_package_share_directory("drn_navigation_mapping"), "scripts", "start_navigation.bash"
    )

    start_localization_script = os.path.join(
        get_package_share_directory("drn_navigation_mapping"), "scripts", "start_localization.bash"
    )

    return LaunchDescription(
        [                  
            IncludeLaunchDescription(
                XMLLaunchDescriptionSource(drn_navigation_route_planning_launch)
            ),   

            TimerAction(
                period=1.0, #10.0
                actions=[
                    IncludeLaunchDescription(
                        PythonLaunchDescriptionSource(
                            drn_navigation_decision_maker_launch
                        )
                    ),
                ],
            ),

            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(drn_navigation_mapping_launch)
            ),               
            
            TimerAction(
                period=1.0,
                actions=[
                    IncludeLaunchDescription(
                        PythonLaunchDescriptionSource(pointcloud2_to_laserscan_launch)
                    ),
                ],
            ),                          

            TimerAction(
                period=1.0,
                actions=[
                    ExecuteProcess(
                        cmd=[
                            '/usr/bin/env', 'bash',
                            str(start_localization_script)],
                        output='screen',
                        shell=False,
                    )
                ]
            ),                                    
            
            TimerAction(
                period=5.0,
                actions=[
                    IncludeLaunchDescription(
                        PythonLaunchDescriptionSource(drn_integration_communication_launch)
                    ),
                ],
            ),                                                                                     
            
            TimerAction(
                period=1.0,
                actions=[
                    ExecuteProcess(
                        cmd=[
                            '/usr/bin/env', 'bash',
                            str(start_navigation_script)],
                        output='screen',
                        shell=False,
                    )
                ]
            ),                                              
            
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(drn_navigation_docking_action)
            ),                                                                                  
            
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(
                    drn_perception_reference_detection_launch
                ),
                launch_arguments={"base_frame": "map"}.items(),
            ),
        ]
    )
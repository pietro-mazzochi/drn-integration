from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='pointcloud_to_laserscan', 
            executable='pointcloud_to_laserscan_node',
            remappings=[
                ('cloud_in', '/front_lidar'),
                ('scan', '/scan')
            ],
            parameters=[{
                'target_frame': 'base_link',
                'transform_tolerance': 0.01,
                'min_height': 0.0,
                'max_height': 0.3,
                'angle_min': -1.5708,
                'angle_max': 1.5708,
                'angle_increment': 0.0016283,
                'scan_time': 0.0,
                'range_min': 0.1,
                'range_max': 100.0,
                'use_inf': False,
                'inf_epsilon': 1.0,
                'robot_length': 2.0,
                'robot_width': 1.36,
            }],
            name='pointcloud_to_laserscan'
        )
    ])

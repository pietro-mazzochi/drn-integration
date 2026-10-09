#from launch import LaunchContext, parse_launch_arguments
import launch 
import launch_ros.actions
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='drn_integration_communication',
            executable='drn_integration_communication_node',
            name='drn_integration_communication_node',
            namespace = "drn_integration/communication",
            output="screen",
            emulate_tty=True,
            parameters=[
            {"param1": "Test"},
            {"param2": 1000},
            ],
            remappings=[
                ('/input/rgb', 'rgb'),
                ('/output/communication', '/drn_integration/communication/communication')
            ]
        )
    ])
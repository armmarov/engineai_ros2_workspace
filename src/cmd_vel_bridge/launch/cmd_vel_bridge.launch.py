from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        Node(
            package='cmd_vel_bridge',
            executable='cmd_vel_bridge_node',
            name='cmd_vel_bridge',
            output='screen',
            parameters=[{
                'max_vx': 0.5,
                'max_vy': 0.35,
                'max_vyaw': 0.6,
                'publish_rate_hz': 100.0,
                'watchdog_timeout_ms': 200,
            }],
        ),
    ])

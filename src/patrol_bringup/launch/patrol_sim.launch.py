"""Launch file for Phase 0 patrol simulation: MuJoCo + RL controller + cmd_vel_bridge."""
import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node


def generate_launch_description():
    # Include existing launch files
    mujoco_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('mujoco_simulator'),
                'launch',
                'mujoco_simulator.launch.py',
            )
        )
    )

    rl_controller_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('interface_example'),
                'launch',
                'rl_basic_example.launch.py',
            )
        )
    )

    # cmd_vel_bridge node
    cmd_vel_bridge_node = Node(
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
    )

    return LaunchDescription([
        mujoco_launch,
        rl_controller_launch,
        cmd_vel_bridge_node,
    ])

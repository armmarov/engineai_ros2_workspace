cmake_minimum_required(VERSION 3.8)
project(cmd_vel_bridge)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(interface_protocol REQUIRED)

add_executable(cmd_vel_bridge_node
  src/cmd_vel_bridge_node.cc
)

ament_target_dependencies(cmd_vel_bridge_node
  rclcpp
  geometry_msgs
  interface_protocol
)

install(TARGETS
  cmd_vel_bridge_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

ament_package()

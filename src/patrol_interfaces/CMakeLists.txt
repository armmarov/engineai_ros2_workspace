cmake_minimum_required(VERSION 3.8)
project(patrol_interfaces)

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)

file(GLOB MSG_FILES "msg/*.msg")
file(GLOB SRV_FILES "srv/*.srv")
file(GLOB ACTION_FILES "action/*.action")

# Convert to relative paths
foreach(MSG_FILE ${MSG_FILES})
  file(RELATIVE_PATH REL_MSG_FILE "${CMAKE_CURRENT_SOURCE_DIR}" "${MSG_FILE}")
  list(APPEND REL_MSG_FILES "${REL_MSG_FILE}")
endforeach()

foreach(SRV_FILE ${SRV_FILES})
  file(RELATIVE_PATH REL_SRV_FILE "${CMAKE_CURRENT_SOURCE_DIR}" "${SRV_FILE}")
  list(APPEND REL_SRV_FILES "${REL_SRV_FILE}")
endforeach()

foreach(ACTION_FILE ${ACTION_FILES})
  file(RELATIVE_PATH REL_ACTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}" "${ACTION_FILE}")
  list(APPEND REL_ACTION_FILES "${REL_ACTION_FILE}")
endforeach()

rosidl_generate_interfaces(${PROJECT_NAME}
  ${REL_MSG_FILES}
  ${REL_SRV_FILES}
  ${REL_ACTION_FILES}
  DEPENDENCIES
  std_msgs
  geometry_msgs
  sensor_msgs
)

ament_package()

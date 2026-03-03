# Patrol system status

uint8 STATE_IDLE = 0
uint8 STATE_PATROLLING = 1
uint8 STATE_PAUSED = 2
uint8 STATE_RETURNING_HOME = 3
uint8 STATE_ERROR = 4

std_msgs/Header header
uint8 state
string current_route
string current_waypoint
uint32 loop_count
uint32 total_loops
float64 battery_pct
float64 localization_confidence
string error_message

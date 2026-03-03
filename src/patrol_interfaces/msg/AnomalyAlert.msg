# Anomaly detection alert

uint8 TYPE_CROWD = 0
uint8 TYPE_BLOCKED = 1
uint8 TYPE_SUSPICIOUS = 2
uint8 TYPE_OTHER = 3

std_msgs/Header header
uint8 alert_type
string description
string location_name
geometry_msgs/Point location
sensor_msgs/Image snapshot
bool acknowledged

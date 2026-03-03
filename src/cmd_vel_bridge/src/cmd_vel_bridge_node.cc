#include <chrono>
#include <cmath>
#include <mutex>

#include "geometry_msgs/msg/twist.hpp"
#include "interface_protocol/msg/gamepad_keys.hpp"
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

class CmdVelBridge : public rclcpp::Node {
 public:
  CmdVelBridge() : Node("cmd_vel_bridge") {
    // Declare parameters with defaults from plan
    declare_parameter("max_vx", 0.5);
    declare_parameter("max_vy", 0.35);
    declare_parameter("max_vyaw", 0.6);
    declare_parameter("publish_rate_hz", 100.0);
    declare_parameter("watchdog_timeout_ms", 200);

    max_vx_ = get_parameter("max_vx").as_double();
    max_vy_ = get_parameter("max_vy").as_double();
    max_vyaw_ = get_parameter("max_vyaw").as_double();
    double publish_rate = get_parameter("publish_rate_hz").as_double();
    watchdog_timeout_ = std::chrono::milliseconds(
        get_parameter("watchdog_timeout_ms").as_int());

    // Subscribe to /cmd_vel
    cmd_vel_sub_ = create_subscription<geometry_msgs::msg::Twist>(
        "/cmd_vel", 10,
        std::bind(&CmdVelBridge::CmdVelCallback, this, std::placeholders::_1));

    // Publish to /hardware/gamepad_keys
    auto qos = rclcpp::QoS(rclcpp::KeepLast(1)).best_effort().durability_volatile();
    gamepad_pub_ = create_publisher<interface_protocol::msg::GamepadKeys>(
        "/hardware/gamepad_keys", qos);

    // Timer at publish_rate_hz
    auto period = std::chrono::duration<double>(1.0 / publish_rate);
    publish_timer_ = create_wall_timer(
        std::chrono::duration_cast<std::chrono::nanoseconds>(period),
        std::bind(&CmdVelBridge::PublishCallback, this));

    last_cmd_time_ = this->now();

    RCLCPP_INFO(get_logger(),
                "cmd_vel_bridge started: max_vx=%.2f, max_vy=%.2f, max_vyaw=%.2f, rate=%.0fHz",
                max_vx_, max_vy_, max_vyaw_, publish_rate);
  }

 private:
  void CmdVelCallback(const geometry_msgs::msg::Twist::SharedPtr msg) {
    std::lock_guard<std::mutex> lock(mtx_);

    // Clamp velocities
    vx_ = std::clamp(msg->linear.x, -max_vx_, max_vx_);
    vy_ = std::clamp(msg->linear.y, -max_vy_, max_vy_);
    vyaw_ = std::clamp(msg->angular.z, -max_vyaw_, max_vyaw_);

    last_cmd_time_ = this->now();
  }

  void PublishCallback() {
    auto msg = interface_protocol::msg::GamepadKeys();
    msg.header.stamp = this->now();

    // Check watchdog: zero velocity if cmd_vel timed out
    auto elapsed = this->now() - last_cmd_time_;
    bool timed_out = elapsed > rclcpp::Duration(watchdog_timeout_);

    {
      std::lock_guard<std::mutex> lock(mtx_);
      if (timed_out) {
        vx_ = 0.0;
        vy_ = 0.0;
        vyaw_ = 0.0;
      }

      // Convert to GamepadKeys analog states [-1.0, 1.0]
      // GamepadKeys::LEFT_STICK_Y (3) = vx / max_vx  (forward/backward)
      // GamepadKeys::LEFT_STICK_X (2) = vy / max_vy  (left/right)
      // GamepadKeys::RIGHT_STICK_Y (5) = vyaw / max_vyaw (rotation)
      // This matches how rl_basic_example reads:
      //   command_.x() = analog[LEFT_STICK_X(3)] * command_scale.x()  -> vx
      //   command_.y() = analog[LEFT_STICK_Y(2)] * command_scale.y()  -> vy
      //   command_.z() = analog[RIGHT_STICK_Y(5)] * command_scale.z() -> vyaw
      msg.analog_states[3] = (max_vx_ > 0.0) ? vx_ / max_vx_ : 0.0;
      msg.analog_states[2] = (max_vy_ > 0.0) ? vy_ / max_vy_ : 0.0;
      msg.analog_states[5] = (max_vyaw_ > 0.0) ? vyaw_ / max_vyaw_ : 0.0;
    }

    gamepad_pub_->publish(msg);
  }

  // Parameters
  double max_vx_;
  double max_vy_;
  double max_vyaw_;
  std::chrono::milliseconds watchdog_timeout_;

  // Current velocity command
  std::mutex mtx_;
  double vx_ = 0.0;
  double vy_ = 0.0;
  double vyaw_ = 0.0;
  rclcpp::Time last_cmd_time_;

  // ROS interfaces
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_sub_;
  rclcpp::Publisher<interface_protocol::msg::GamepadKeys>::SharedPtr gamepad_pub_;
  rclcpp::TimerBase::SharedPtr publish_timer_;
};

int main(int argc, char* argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CmdVelBridge>());
  rclcpp::shutdown();
  return 0;
}

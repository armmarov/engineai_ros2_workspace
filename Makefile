CONTAINER  := engineai_ros2
ROS_SOURCE := source /opt/ros/humble/setup.bash && source /workspace/install/setup.bash

# ─────────────────────────────────────────────
#  Docker image / container management
# ─────────────────────────────────────────────

build-image:
	docker compose build

start: xhost
	docker compose up -d

stop:
	docker compose down

restart: stop start

shell:
	docker exec -it $(CONTAINER) bash

# ─────────────────────────────────────────────
#  Build
# ─────────────────────────────────────────────

build:
	docker exec $(CONTAINER) bash -c \
		"source /opt/ros/humble/setup.bash && cd /workspace && \
		rm -rf build/mujoco_simulator build/interface_example install/mujoco_simulator install/interface_example && \
		./scripts/build_nodes.sh sim"

# ─────────────────────────────────────────────
#  Run nodes (run each in a separate terminal)
# ─────────────────────────────────────────────

run-controller:
	docker exec -it $(CONTAINER) bash -c "$(ROS_SOURCE) && ros2 launch interface_example rl_basic_example.launch.py"

run-sim:
	docker exec -it $(CONTAINER) bash -c "$(ROS_SOURCE) && ros2 launch mujoco_simulator mujoco_simulator.launch.py"

run-patrol:
	docker exec -it $(CONTAINER) bash -c "$(ROS_SOURCE) && python3 /workspace/src/interface_example/scripts/patrol_rectangle.py"

# ─────────────────────────────────────────────
#  Utilities
# ─────────────────────────────────────────────

xhost:
	xhost +local:docker

monitor:
	docker exec $(CONTAINER) bash -c "$(ROS_SOURCE) && ros2 topic echo /hardware/joint_command --once"

topics:
	docker exec $(CONTAINER) bash -c "$(ROS_SOURCE) && ros2 topic list"

hz:
	docker exec $(CONTAINER) bash -c "$(ROS_SOURCE) && ros2 topic hz /hardware/joint_command"

logs:
	docker logs -f $(CONTAINER)

# ─────────────────────────────────────────────
#  First-time setup
# ─────────────────────────────────────────────

setup: build-image start build
	@echo "Setup complete."
	@echo "Terminal 1: make run-controller"
	@echo "Terminal 2: make run-sim"
	@echo "Terminal 3: make run-patrol (optional)"

.PHONY: build-image start stop restart shell build \
        run-controller run-sim run-patrol \
        xhost monitor topics hz logs setup

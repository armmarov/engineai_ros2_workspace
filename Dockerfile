FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Asia/Kuala_Lumpur

# Basic system deps
RUN apt update && apt install -y \
    curl gnupg2 lsb-release \
    build-essential gcc g++ cmake \
    git wget rsync sshpass openssh-client \
    libglfw3-dev libxinerama-dev libxcursor-dev \
    libgl1-mesa-glx libgl1-mesa-dri \
    python3 python3-pip \
    tmux \
    && rm -rf /var/lib/apt/lists/*

# Add ROS2 Humble apt repo
RUN curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key \
    -o /usr/share/keyrings/ros-archive-keyring.gpg && \
    echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] \
    http://packages.ros.org/ros2/ubuntu $(lsb_release -cs) main" \
    > /etc/apt/sources.list.d/ros2.list

# Install ROS2 Humble
RUN apt update && apt install -y \
    ros-dev-tools \
    ros-humble-ros-base \
    ros-humble-rmw-cyclonedds-cpp \
    && rm -rf /var/lib/apt/lists/*

# Extract third-party libs (MNN, MuJoCo, Eigen, yaml-cpp, etc.)
# engineai_robotics_third_party_libs.tar.gz is already in the repo
COPY src/third_party/engineai_robotics_third_party_libs.tar.gz /tmp/
RUN tar -xzf /tmp/engineai_robotics_third_party_libs.tar.gz -C /opt && \
    rm /tmp/engineai_robotics_third_party_libs.tar.gz

# ROS2 environment
ENV ROS_DOMAIN_ID=69
ENV ROS_LOCALHOST_ONLY=0
ENV RMW_IMPLEMENTATION=rmw_cyclonedds_cpp

RUN echo "source /opt/ros/humble/setup.bash" >> /root/.bashrc

WORKDIR /workspace

CMD ["/bin/bash"]

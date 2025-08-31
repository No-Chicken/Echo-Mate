# 使用幸狐官方提供的 Luckfox Pico 镜像作为基础
FROM luckfoxtech/luckfox_pico:1.0

ENV DEBIAN_FRONTEND=noninteractive

RUN sed -i 's/archive.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list && \
    sed -i 's/security.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list && \
    apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    ninja-build \
    pkg-config \
    libjson-c-dev \
    libjsoncpp-dev \
    libcurl4-openssl-dev \
    libwebsocketpp-dev \
    libboost-all-dev \
    libopus-dev \
    portaudio19-dev \
    libdrm-dev \
    libsdl2-dev \
    libsdl2-image-dev \
    libopenblas-dev \
    x11-apps \
    && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# 创建一个工作目录
WORKDIR /project

# 容器启动时默认执行的命令
CMD ["/bin/bash"]
# Pinned base image by digest for Ubuntu 22.04 (retrieved from Docker Hub)
FROM ubuntu@sha256:1c4cc37c10c4678fd5369d172a4e079af8a28a6e6f724647ccaa311b4801c3c9

ENV DEBIAN_FRONTEND=noninteractive

# ---- Pin package versions ----
# Add 3rd party repositories

RUN apt-get update && apt-get install --no-install-recommends -y \
    apt-utils \
    gpg \
    gpg-agent \
    ca-certificates \
    software-properties-common \
    && add-apt-repository ppa:openjdk-r/ppa

# Install necessary packages
RUN apt-get update && apt-get install --no-install-recommends -y --fix-missing \
    build-essential \
    curl \
    wget \
    git \
    python3 \
    python3-pip \
    libpcre2-dev \
    make \
    ninja-build \
    unzip \
    bzip2 \
    xz-utils \
    tar \
    libncurses5 \
    libncursesw5 \
    libtinfo5 \
    libusb-1.0-0 \
    libgtk-3-0 \
    rsync \
    && rm -rf /var/lib/apt/lists/*

# Install latest CMake from Kitware APT repository, verify via SHA256
ARG KITWARE_URL="https://apt.kitware.com/kitware-archive.sh"
ARG KITWARE_SHA256="4c16054d0a4808c9871e347dd1b10c1e4bbd3880b31235c06d6be91f86f4bf8f"
RUN curl -fsSL -o /tmp/kitware-archive.sh $KITWARE_URL \
    && echo "$KITWARE_SHA256  /tmp/kitware-archive.sh" | sha256sum -c - \
    && bash /tmp/kitware-archive.sh \
    && apt-get update \
    && apt-get install -y cmake \
    && rm -rf /var/lib/apt/lists/* \
    && rm /tmp/kitware-archive.sh

# Download & install GNU Arm Embedded Toolchain, verify via SHA256
ARG ARM_GCC_URL="https://developer.arm.com/-/media/Files/downloads/gnu/12.2.rel1/binrel/arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi.tar.xz"
ARG ARM_GCC_SHA256="84be93d0f9e96a15addd490b6e237f588c641c8afdf90e7610a628007fc96867"
WORKDIR /tmp
RUN curl -fsSL -o arm-gnu-toolchain.tar.xz $ARM_GCC_URL \
    && echo "sha256sum arm-gnu-toolchain.tar.xz" \
    && echo "$ARM_GCC_SHA256  arm-gnu-toolchain.tar.xz" | sha256sum -c - \
    && tar -xf arm-gnu-toolchain.tar.xz \
    && TOOLCHAIN_FOLDER=$(find . -maxdepth 1 -type d -name 'arm-gnu-toolchain-*' | head -n 1) \
    && mv "$TOOLCHAIN_FOLDER" /opt/gcc-arm-none-eabi \
    && rm arm-gnu-toolchain.tar.xz -rf

# ---- Environment ----
ENV ARM_GCC_DIR="/opt/gcc-arm-none-eabi"
ENV PATH="${PATH}:/opt/gcc-arm-none-eabi/bin"
ENV PATH="${PATH}:/usr/local/bin"

WORKDIR /home

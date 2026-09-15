### Dockerfile for the CI/CD pipeline

FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Define the URLs for the tools
ARG ARM_GCC_URL="https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz"

# Install necessary packages
RUN apt-get update \
    && apt-get install --no-install-recommends -y --fix-missing \
    build-essential \
    ca-certificates \
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

# Install latest CMake
ADD https://apt.kitware.com/kitware-archive.sh /tmp/kitware-archive.sh
RUN bash /tmp/kitware-archive.sh \
    && apt-get update \
    && apt-get install -y cmake \
    && rm -rf /var/lib/apt/lists/* \
    && rm /tmp/kitware-archive.sh

# Install GNU Arm Embedded Toolchain
# REGEXP: $(find . -maxdepth 1 -type d -name 'arm-gnu-toolchain-*' | head -n 1)
# This will find the first folder in the current directory that starts with 'arm-gnu-toolchain-'
# This is necessary because the downloaded archive contains a folder with a version number in the name
# and we don't know what that version number is.
WORKDIR /tmp
ADD "$ARM_GCC_URL" arm-gnu-toolchain.tar.xz

RUN tar -xf arm-gnu-toolchain.tar.xz \
    && TOOLCHAIN_FOLDER=$(find . -maxdepth 1 -type d -name 'arm-gnu-toolchain-*' | head -n 1) \
    && mv "$TOOLCHAIN_FOLDER" /opt/gcc-arm-none-eabi \
    && rm arm-gnu-toolchain.tar.xz -rf

ENV ARM_GCC_DIR="/opt/gcc-arm-none-eabi"
ENV PATH="${PATH}:/opt/gcc-arm-none-eabi/bin"

WORKDIR /home

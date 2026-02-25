# Pinned base image by digest for Ubuntu 22.04 (retrieved from Docker Hub)
FROM ubuntu@sha256:1c4cc37c10c4678fd5369d172a4e079af8a28a6e6f724647ccaa311b4801c3c9

ENV DEBIAN_FRONTEND=noninteractive

# ---- Pin package versions ----
# Add 3rd party repositories
RUN apt-get update && apt-get install --no-install-recommends -y \
    apt-utils=2.4.14 \
    gpg \
    gpg-agent \
    ca-certificates=20240203~22.04.1 \
    software-properties-common=0.99.22.9 \
    && add-apt-repository ppa:openjdk-r/ppa

# Install necessary packages
RUN apt-get update && apt-get install --no-install-recommends -y --fix-missing \
    build-essential=12.9ubuntu3 \
    curl=7.81.0-1ubuntu1.21 \
    wget=1.21.2-2ubuntu1.1 \
    git=1:2.34.1-1ubuntu1.15 \
    python3=3.10.6-1~22.04.1 \
    python3-pip=22.0.2+dfsg-1ubuntu0.7 \
    libpcre2-dev=10.39-3ubuntu0.1 \
    make=4.3-4.1build1 \
    ninja-build=1.10.1-1 \
    unzip=6.0-26ubuntu3.2 \
    bzip2=1.0.8-5build1 \
    xz-utils=5.2.5-2ubuntu1 \
    tar=1.34+dfsg-1ubuntu0.1.22.04.2 \
    libncurses5=6.3-2ubuntu0.1 \
    libncursesw5=6.3-2ubuntu0.1 \
    libtinfo5=6.3-2ubuntu0.1 \
    libusb-1.0-0=2:1.0.25-1ubuntu2 \
    libgtk-3-0=3.24.33-1ubuntu2.2 \
    rsync=3.2.7-0ubuntu0.22.04.4 \
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

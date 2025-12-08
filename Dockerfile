### This is a template Dockerfile for the CI/CD pipeline

FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Define the URLs for the tools
##### TODO #####
# Check and update version numbers if necessary
ARG ARM_GCC_URL="https://developer.arm.com/-/media/Files/downloads/gnu/12.2.rel1/binrel/arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi.tar.xz"
ARG SONAR_SCANNER_URL="https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-6.1.0.4477-linux-x64.zip"
ARG SONAR_BUILD_WRAPPER="https://sonarqube.silabs.net/static/cpp/build-wrapper-linux-x86.zip"


#add 3rd party repositories
RUN apt-get update  \
    && apt-get install --no-install-recommends -y \
    apt-utils \
    gpg \
    gpg-agent \
    ca-certificates \
    software-properties-common \
    && add-apt-repository ppa:openjdk-r/ppa

#Install necessary packages
RUN apt-get update \
    && apt-get install --no-install-recommends -y --fix-missing \
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

# Download and install SonarQube scanner
#REGEX: $(find /opt -maxdepth 1 -type d -name 'sonar-scanner-*' | head -n 1)
#This will find the first folder in /opt that starts with 'sonar-scanner-'
#This is necessary because the downloaded archive contains a folder with a version number in the name
#and we don't know what that version number is.

ADD  "$SONAR_SCANNER_URL" /tmp/sonar-scanner-cli.zip

RUN unzip /tmp/sonar-scanner-cli.zip -d /opt \
    && SCANNER_FOLDER=$(find /opt -maxdepth 1 -type d -name 'sonar-scanner-*' | head -n 1) \
    && ln -s ${SCANNER_FOLDER}/bin/sonar-scanner /usr/local/bin/sonar-scanner \
    && rm /tmp/sonar-scanner-cli.zip

# Download and install build-wrapper
ADD "$SONAR_BUILD_WRAPPER" /tmp/build-wrapper-linux-x86.zip
RUN unzip /tmp/build-wrapper-linux-x86.zip -d /opt \
    && ln -s /opt/build-wrapper-linux-x86/build-wrapper-linux-x86 /usr/local/bin/build-wrapper \
    && rm /tmp/build-wrapper-linux-x86.zip

ENV ARM_GCC_DIR="/opt/gcc-arm-none-eabi"
ENV PATH="${PATH}:/opt/gcc-arm-none-eabi/bin"
ENV PATH="${PATH}:/usr/local/bin"
ENV PATH="${PATH}:/opt/build-wrapper-linux-x86/"

WORKDIR /home

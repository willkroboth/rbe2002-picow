# Based on Latest Ubuntu https://hub.docker.com/_/ubuntu/
FROM ubuntu:latest

WORKDIR /workspaces

# Install dependencies for working with C code
RUN apt-get update && apt-get install -y \
    cmake \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    build-essential \
    libstdc++-arm-none-eabi-newlib \
    git \
    python3 \
    micro

# Install PICO SDK
RUN git clone --recursive https://github.com/raspberrypi/pico-sdk.git
ENV PICO_SDK_PATH=/workspaces/pico-sdk

# Install picotool
RUN git clone https://github.com/raspberrypi/picotool.git && \
    cd picotool && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make install

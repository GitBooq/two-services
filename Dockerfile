FROM ubuntu:24.04 AS builder-base

ENV DEBIAN_FRONTEND=noninteractive

# toolchain
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    gcc-13 \
    g++-13 \
    git \
    cmake \
    ninja-build \
    pkg-config \
    curl \
    zip \
    unzip \
    tar \
    python3 \
    python3-pip \
    bison \
    flex \
    autoconf \
    automake \
    libtool \
    ca-certificates \
    && update-ca-certificates \
    && rm -rf /var/lib/apt/lists/*

RUN update-alternatives \
    --install /usr/bin/gcc gcc /usr/bin/gcc-13 100 && \
    update-alternatives \
    --install /usr/bin/g++ g++ /usr/bin/g++-13 100


# vcpkg
WORKDIR /opt

RUN git clone https://github.com/microsoft/vcpkg.git

WORKDIR /opt/vcpkg

RUN git checkout 2026.06.01 && \
    ./bootstrap-vcpkg.sh -disableMetrics

ENV VCPKG_ROOT=/opt/vcpkg
ENV VCPKG_FORCE_SYSTEM_BINARIES=1
ENV VCPKG_MAX_CONCURRENCY=4
ENV CMAKE_BUILD_PARALLEL_LEVEL=4

COPY vcpkg.json /tmp/vcpkg.json

WORKDIR /tmp

RUN /opt/vcpkg/vcpkg install \
    --triplet=x64-linux

# cmake configure
FROM builder-base AS configure

WORKDIR /workspace

COPY . .

RUN cmake \
    -S . \
    -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake

# Client builder
FROM configure AS client-builder
RUN cmake --build build --target ipv4filter_client

# Server builder
FROM configure AS server-builder
RUN cmake --build build --target history_server

# Client bin
FROM ubuntu:24.04 AS ipv4filter_client

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        ca-certificates && \
    rm -rf /var/lib/apt/lists/*

COPY --from=client-builder \
    /workspace/build/services/ipv4_filter/ipv4filter_client \
    /usr/local/bin/

ENTRYPOINT ["/usr/local/bin/ipv4filter_client"]

# Server bin
FROM ubuntu:24.04 AS history_server

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        ca-certificates && \
    rm -rf /var/lib/apt/lists/*

COPY --from=server-builder \
    /workspace/build/services/history/history_server \
    /usr/local/bin/

ENTRYPOINT ["/usr/local/bin/history_server"]
cmake_minimum_required(VERSION 3.15...4.1)

project(
    TwoServices
    VERSION 0.1.0
    DESCRIPTION "nice description"
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

option(ENABLE_SANITIZERS "Enable sanitizers" OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

include(CTest) # flask
include(FetchContent)

if(ENABLE_SANITIZERS)
    add_compile_options(
        -fsanitize=address,undefined
        -g
        -fno-omit-frame-pointer
    )

    add_link_options(
        -fsanitize=address,undefined
    )
endif()

# ----------------------------------------------------------
# Dependencies
# ----------------------------------------------------------
set(NETLOGFILTER_BUILD_TESTS OFF)
set(NETLOGFILTER_BUILD_EXAMPLES OFF)

include(FetchContent)
FetchContent_Declare(
    netLogFilter
    GIT_REPOSITORY https://github.com/GitBooq/net-log-filter.git
    GIT_TAG v1.1.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(netLogFilter)

find_package(libpqxx CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED) # Protobuf first!(https://stackoverflow.com/questions/62245040/grpc-cant-find-protobuf-library)
message(STATUS "Protobuf_VERSION = ${Protobuf_VERSION}")
find_package(gRPC CONFIG REQUIRED)

#protobuf
add_subdirectory(proto)

#shared
add_subdirectory(shared)

#client service
add_subdirectory(services/ipv4_filter)

#server service
add_subdirectory(services/history)

# libpqxx::pqxx     // postgresql
# netFilterLogger   // ipv4filter
# proto_generated   // protobuf
# gRPC::grpc++

option(BUILD_TESTS "BUILD_TESTS" ON)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
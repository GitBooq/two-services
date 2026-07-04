# USE CASES
add_library(client_use_cases STATIC
    core/use_cases/save_event_use_case.cc
    core/use_cases/get_events_use_case.cc
    core/use_cases/get_stats_use_case.cc
)

target_link_libraries(client_use_cases
    PUBLIC
        shared_entities
        shared_interfaces
        shared_utils
)

target_include_directories(client_use_cases
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/core/use_cases
)

target_compile_features(client_use_cases PUBLIC cxx_std_20)

# ADAPTERS
add_subdirectory(infrastructure/adapters)

# gRPC
add_subdirectory(infrastructure/network)

# CONFIG
add_library(client_config INTERFACE
)

target_link_libraries(client_config
    INTERFACE
        shared_entities
)

target_include_directories(client_config
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/infrastructure/config
)

# CLIENT EXE
add_executable(client main.cc)

target_link_libraries(client
    PRIVATE
        client_use_cases
        client_adapters
        client_network
        client_config
        #client_logging
        shared_entities
        shared_interfaces
        shared_utils
        proto_generated
        netFilterLogger
)

target_compile_features(client PUBLIC cxx_std_20)
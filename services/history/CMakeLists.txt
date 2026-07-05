# Core Interfaces
add_library(core_interfaces INTERFACE)

target_link_libraries(core_interfaces INTERFACE shared_entities)

target_include_directories(core_interfaces 
    INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}/core/use_cases
)

target_compile_features(core_interfaces INTERFACE cxx_std_20)

# gRPC
add_subdirectory(network)

# Application logic
add_subdirectory(application/use_cases)

# Repository interfaces
add_subdirectory(adapters/repositories)

# SERVER EXE
add_executable(server main.cc)

target_link_libraries(server
    PRIVATE
        server_network
        shared_entities
        shared_interfaces
        shared_utils
        proto_generated
        netFilterLogger
)

target_compile_features(server PUBLIC cxx_std_20)
# gRPC
add_subdirectory(network)

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
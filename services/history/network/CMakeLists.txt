# gRPC SERVER NETWORK
add_library(server_network STATIC
    grpc_event_service_impl.cc)

target_link_libraries(server_network 
    PUBLIC
        proto_generated
        gRPC::grpc++
        shared_interfaces
        shared_utils)

target_include_directories(server_network 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/)

target_compile_features(server_network PUBLIC cxx_std_20)
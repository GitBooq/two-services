# gRPC NETWORK
add_library(client_network STATIC
    grpc_service.cc)

target_link_libraries(client_network 
    PUBLIC
        proto_generated
        gRPC::grpc++
        shared_interfaces)

target_include_directories(client_network 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/)

target_compile_features(client_network PUBLIC cxx_std_20)
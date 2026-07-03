###########################################################################
# We have file: proto/{SOME_NAME}.proto
# From this file protoc should generate:
#   build/proto/generated/
#   {SOME_NAME}.pb.h
#   {SOME_NAME}.pb.cc
#   {SOME_NAME}.grpc.pb.h
#   {SOME_NAME}.grpc.pb.cc
# then we need to build lib proto_generated to use from services
###########################################################################

set(PROTO_FILE
    ${CMAKE_CURRENT_SOURCE_DIR}/event_service.proto
)

set(GENERATED_DIR
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

file(MAKE_DIRECTORY ${GENERATED_DIR})

# Find the protoc and gRPC executable installed on the host system
find_program(PROTOBUF_PROTOC protoc REQUIRED)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin REQUIRED)
message(STATUS "PROTOC=${PROTOBUF_PROTOC}")
message(STATUS "PLUGIN=${GRPC_CPP_PLUGIN}")

add_custom_command(
    OUTPUT
        ${GENERATED_DIR}/event_service.pb.cc
        ${GENERATED_DIR}/event_service.pb.h
        ${GENERATED_DIR}/event_service.grpc.pb.cc
        ${GENERATED_DIR}/event_service.grpc.pb.h

    COMMAND
        ${PROTOBUF_PROTOC}

    ARGS
        --grpc_out=${GENERATED_DIR}
        --cpp_out=${GENERATED_DIR}
        --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
        -I ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROTO_FILE}

    DEPENDS
        ${PROTO_FILE}
)

add_library(proto_generated
    ${GENERATED_DIR}/event_service.pb.cc
    ${GENERATED_DIR}/event_service.grpc.pb.cc
)

target_include_directories(proto_generated
    PUBLIC
    ${GENERATED_DIR}
)

target_link_libraries(proto_generated
    PUBLIC
    protobuf::libprotobuf
    gRPC::grpc++
)
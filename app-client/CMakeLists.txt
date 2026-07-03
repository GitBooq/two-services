# DOMAIN (DTO)
add_library(commonObjects INTERFACE)
target_include_directories(commonObjects INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/domain
)

# EVENT PIPELINE
add_library(eventPipeline STATIC pipeline/event_pipeline.cc)
target_link_libraries(eventPipeline
    PUBLIC
        eventBuilder
)
target_include_directories(eventPipeline 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/pipeline
        ${CMAKE_CURRENT_SOURCE_DIR}/interfaces
        ${CMAKE_CURRENT_SOURCE_DIR}/config)
target_compile_features(eventPipeline PUBLIC cxx_std_20)

# NETWORK (gRPC)
add_library(grpcService STATIC network/grpc_service.cc)
target_link_libraries(grpcService
    PUBLIC
        commonObjects
        proto_generated
)
target_include_directories(grpcService 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/network
        ${CMAKE_CURRENT_SOURCE_DIR}/interfaces)
target_compile_features(grpcService PUBLIC cxx_std_20)

# FILTER RUNNER
add_library(filterRunner STATIC filter_runner/filter_runner.cc)
target_link_libraries(filterRunner
    PUBLIC
        eventPipeline
        netFilterLogger
)
target_include_directories(filterRunner 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/filter_runner
        ${CMAKE_CURRENT_SOURCE_DIR}/config)
target_compile_features(filterRunner PUBLIC cxx_std_20)

# EVENT BUILDER
add_library(eventBuilder STATIC event_builder/event_builder.cc)
target_link_libraries(eventBuilder
    PUBLIC
        netFilterLogger
        commonObjects
)
target_include_directories(eventBuilder PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/event_builder)
target_compile_features(eventBuilder PUBLIC cxx_std_20)

# CLIENT (main)
add_executable(client main.cc)
target_link_libraries(client
    PUBLIC
        commonObjects
        filterRunner
        eventPipeline
        grpcService
)
target_include_directories(client 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/config)
target_compile_features(client PUBLIC cxx_std_20)
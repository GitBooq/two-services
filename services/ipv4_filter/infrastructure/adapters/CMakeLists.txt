# Client Adapters
add_library(client_adapters STATIC
    builder/event_builder.cc)

target_link_libraries(client_adapters 
    PUBLIC
        shared_interfaces
        shared_utils)

target_include_directories(client_adapters 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/builder)

target_compile_features(client_adapters PUBLIC cxx_std_20)
add_library(client_helpers client_helpers.cc)

target_compile_features(client_helpers PRIVATE cxx_std_20)

target_include_directories(client_helpers
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include/core
        ${CMAKE_SOURCE_DIR}/include/client)

add_executable(app-client app-client.cc)

target_link_libraries(app-client 
    PUBLIC 
        netFilterLogger 
        client_helpers)

target_compile_features(app-client PRIVATE cxx_std_20)

target_include_directories(app-client 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include/core
        ${CMAKE_SOURCE_DIR}/include/client)
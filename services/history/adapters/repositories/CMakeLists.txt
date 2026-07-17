add_library(repository_interfaces INTERFACE)

target_include_directories(repository_interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(repository_interfaces 
    INTERFACE 
        core_interfaces
        shared_entities
)

target_compile_features(repository_interfaces INTERFACE cxx_std_20)
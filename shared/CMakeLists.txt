# ENTITIES (DTO)
add_library(shared_entities INTERFACE)

target_include_directories(shared_entities
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/core/entities
)

target_compile_features(shared_entities INTERFACE cxx_std_20)

# INTERFACES
add_library(shared_interfaces INTERFACE)

target_include_directories(shared_interfaces
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/core/interfaces
)

target_link_libraries(shared_interfaces
    INTERFACE
        shared_entities
        netFilterLogger
)

# UTILS
add_library(shared_utils STATIC
    utils/utils.cc
)

target_include_directories(shared_utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

target_link_libraries(shared_utils
    PUBLIC
        shared_entities
        netFilterLogger
)

target_compile_features(shared_utils PUBLIC cxx_std_20)


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

# Postgres repo
add_library(postgres_repo STATIC infrastructure/db/postgres_events_repo.cc)

target_link_libraries(postgres_repo 
    PRIVATE 
        shared_entities
        repository_interfaces
        libpqxx::pqxx
)

target_include_directories(postgres_repo 
    INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}/infrastructure/db
)

target_compile_features(postgres_repo PRIVATE cxx_std_20)

# SERVER EXE
add_executable(history_server main.cc)

target_link_libraries(history_server
    PRIVATE
        server_network
        shared_entities
        shared_interfaces
        shared_utils
        application_logic
        proto_generated
        postgres_repo
        libpqxx::pqxx
)

target_compile_features(history_server PUBLIC cxx_std_20)
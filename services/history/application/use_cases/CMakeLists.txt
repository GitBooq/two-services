add_library(application_logic STATIC
    get_events_use_case.cc
    get_stats_use_case.cc
    save_event_use_case.cc
)

target_link_libraries(application_logic
    PUBLIC 
        core_interfaces
        repository_interfaces
        shared_entities
    
)

target_include_directories(application_logic
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(application_logic PRIVATE cxx_std_20)
# GoogleTest library
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

# SHARED
add_executable(shared_tests
    shared/utils/utils_test.cc
)

target_link_libraries(shared_tests
    PRIVATE
        shared_entities
        shared_utils
        GTest::gtest_main
)

target_include_directories(shared_tests PRIVATE ${CMAKE_SOURCE_DIR}/tests)

target_compile_options(shared_tests PRIVATE
    -O0
    -g
)

target_compile_features(shared_tests PUBLIC cxx_std_20)

gtest_discover_tests(shared_tests)

set(CLIENT_TESTS_SRC_DIR "services/ipv4_filter")
set(SERVER_TESTS_SRC_DIR "services/history")

# EventsUseCases
add_executable(event_use_cases_test
    ${CLIENT_TESTS_SRC_DIR}/core/use_cases/save_event_use_case_test.cc
    ${CLIENT_TESTS_SRC_DIR}/core/use_cases/get_events_use_case_test.cc
    ${CLIENT_TESTS_SRC_DIR}/core/use_cases/get_stats_use_case_test.cc
)

target_link_libraries(event_use_cases_test
    GTest::gtest_main
    GTest::gmock_main
    client_use_cases
)

target_include_directories(event_use_cases_test 
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/tests)

target_compile_options(event_use_cases_test PRIVATE
    -O0
    -g
)

target_compile_features(event_use_cases_test PUBLIC cxx_std_20)

gtest_discover_tests(event_use_cases_test)

# Server grpc event service
add_executable(grpc_event_service_test
    ${SERVER_TESTS_SRC_DIR}/grpc_event_service_impl_test.cc
)

target_link_libraries(grpc_event_service_test
    GTest::gtest_main
    GTest::gmock_main
    gRPC::grpc++
    core_interfaces
    server_network
)

target_include_directories(grpc_event_service_test 
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/tests)

target_compile_options(grpc_event_service_test PRIVATE
    -O0
    -g
)

target_compile_features(grpc_event_service_test PUBLIC cxx_std_20)

gtest_discover_tests(grpc_event_service_test)
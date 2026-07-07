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

set(TESTS_SRC_DIR "services/ipv4_filter/core/use_cases")

# SaveEventUseCase
add_executable(save_event_use_case_test
    ${TESTS_SRC_DIR}/save_event_use_case_test.cc
    ${TESTS_SRC_DIR}/get_events_use_case_test.cc
    ${TESTS_SRC_DIR}/get_stats_use_case_test.cc
)

target_link_libraries(save_event_use_case_test
    GTest::gtest_main
    GTest::gmock_main
    client_use_cases
)

target_include_directories(save_event_use_case_test 
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/tests)

target_compile_options(save_event_use_case_test PRIVATE
    -O0
    -g
)

target_compile_features(save_event_use_case_test PUBLIC cxx_std_20)

gtest_discover_tests(save_event_use_case_test)
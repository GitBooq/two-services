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

include(GoogleTest)
gtest_discover_tests(shared_tests)
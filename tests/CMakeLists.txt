include(FetchContent)
FetchContent_Declare(        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git        GIT_TAG release-1.12.1
        )
FetchContent_MakeAvailable(googletest)
enable_testing()
add_executable(        tests
        DataBase_tests.cpp
        )
target_link_libraries(        tests
        GTest::gtest_main
        )
target_include_directories(tests PUBLIC ${PROJECT_SOURCE_DIR})
include(GoogleTest)
gtest_discover_tests(tests)
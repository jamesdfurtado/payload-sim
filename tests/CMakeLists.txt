# Google Test Setup
cmake_minimum_required(VERSION 3.16)

# Find Google Test
find_package(GTest REQUIRED)
include(GoogleTest)

# Include directories from main project
include_directories(../src)

# Collect all test source files
file(GLOB_RECURSE TEST_SOURCES 
    "*.cpp"
    "*.h"
)

# Remove main.cpp if it exists (we'll have our own test main)
list(FILTER TEST_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

# Collect all source files from main project (except main.cpp)
file(GLOB_RECURSE PROJECT_SOURCES 
    "../src/*.cpp"
    "../src/*.h"
)
list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

# Create test executable
add_executable(payload_sim_tests 
    ${TEST_SOURCES}
    ${PROJECT_SOURCES}
)

# Link against Google Test and required libraries
target_link_libraries(payload_sim_tests 
    GTest::gtest 
    GTest::gtest_main
    raylib
)

# Set C++ standard
set_property(TARGET payload_sim_tests PROPERTY CXX_STANDARD 17)

# Enable testing
enable_testing()

# Discover and register all tests
gtest_discover_tests(payload_sim_tests)

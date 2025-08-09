cmake_minimum_required(VERSION 3.10)
project(payload_sim VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Fetch raylib
set(RAYLIB_VERSION 5.5)
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG        ${RAYLIB_VERSION}
)

# Suppress examples/tests/tools of raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING  OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

add_executable(payload-sim
  src/main.cpp
  src/simulation/SimulationEngine.cpp
  src/systems/PowerSystem.cpp
  src/systems/DepthControl.cpp
  src/systems/EnvironmentSystem.cpp
)

target_include_directories(payload-sim PRIVATE src)
target_link_libraries(payload-sim PRIVATE raylib)



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
  src/systems/SonarSystem.cpp
  src/systems/TargetingSystem.cpp
  src/systems/SafetySystem.cpp
)

target_include_directories(payload-sim PRIVATE src)
target_link_libraries(payload-sim PRIVATE raylib)

# Platform specifics
option(BUILD_WEB "Build for WebAssembly (Emscripten)" OFF)
if(EMSCRIPTEN OR BUILD_WEB)
  message(STATUS "Configuring for WebAssembly")
  set(CMAKE_EXECUTABLE_SUFFIX ".html")
  target_compile_definitions(payload-sim PRIVATE PLATFORM_WEB)
  target_link_options(payload-sim PRIVATE
    -sUSE_GLFW=3
    -sWASM=1
    -sALLOW_MEMORY_GROWTH=1
    -sASYNCIFY
    --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html
  )
else()
  if(MSVC)
    target_compile_options(payload-sim PRIVATE /W4 /permissive-)
  else()
    target_compile_options(payload-sim PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endif()

# On Windows with MinGW
if(WIN32 AND MINGW)
  target_link_libraries(payload-sim PRIVATE winmm)
endif()



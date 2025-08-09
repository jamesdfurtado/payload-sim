cmake_minimum_required(VERSION 3.10)
project(payload_sim VERSION 0.1.0 LANGUAGES CXX)

cmake_policy(VERSION 3.20)

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

# Fetch Google Test
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)

# Suppress examples/tests/tools of raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

# Suppress Google Test's own tests
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

# Force Google Test to use the same runtime library as our project
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib googletest)

## Patch raylib's CMakeLists to relax the minimum CMake version requirement
file(READ  ${raylib_SOURCE_DIR}/CMakeLists.txt CONTENT)
string(REGEX REPLACE
  "cmake_minimum_required\\(VERSION +[0-9]+\\.[0-9]+\\)"
  "cmake_minimum_required(VERSION 3.10)"
  CONTENT
  "${CONTENT}"
)
file(WRITE ${raylib_SOURCE_DIR}/CMakeLists.txt "${CONTENT}")

## Sources
file(GLOB_RECURSE SIM_SRC CONFIGURE_DEPENDS
  src/*.cpp
)

add_executable(payload-sim ${SIM_SRC})

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

# Enable testing
enable_testing()

# Test sources
file(GLOB_RECURSE TEST_SRC CONFIGURE_DEPENDS
  tests/*.cpp
)

# Create test executable if test sources exist
if(TEST_SRC)
  add_executable(payload-sim-tests ${TEST_SRC})
  target_include_directories(payload-sim-tests PRIVATE src)
  target_link_libraries(payload-sim-tests PRIVATE gtest gtest_main raylib)
  
  # Add individual test files as separate systems
  file(GLOB_RECURSE SIM_TEST_SOURCES CONFIGURE_DEPENDS
    src/simulation/*.cpp
    src/systems/*.cpp
  )
  
  # Link simulation systems to tests (but not main.cpp)
  target_sources(payload-sim-tests PRIVATE ${SIM_TEST_SOURCES})
  
  # Apply same platform-specific settings as main executable
  if(WIN32 AND MINGW)
    target_link_libraries(payload-sim-tests PRIVATE winmm)
  endif()
  
  # Discover tests for CTest
  include(GoogleTest)
  gtest_discover_tests(payload-sim-tests)
endif()

# Install step (optional)
install(TARGETS payload-sim RUNTIME DESTINATION bin)



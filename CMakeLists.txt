cmake_minimum_required(VERSION 3.16)
project(payload-sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Allow users to provide Raylib via package managers (vcpkg, Conan, system)
find_package(raylib QUIET CONFIG)
if(NOT raylib_FOUND)
  find_package(raylib QUIET)
endif()

if(NOT raylib_FOUND)
  message(STATUS "raylib not found; fetching with FetchContent")
  include(FetchContent)
  set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
  )
  FetchContent_MakeAvailable(raylib)
endif()

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  "src/*.cpp"
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

if(WIN32)
  target_compile_definitions(${PROJECT_NAME} PRIVATE _USE_MATH_DEFINES NOMINMAX)
endif()



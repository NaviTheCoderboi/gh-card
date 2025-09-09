cmake_minimum_required(VERSION 4.0)

set(CMAKE_C_COMPILER "/usr/bin/clang" CACHE STRING "C compiler" FORCE)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++" CACHE STRING "C++ compiler" FORCE)

set(PROJECT_NAME "gh-card")
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR src)
set(INCLUDE_DIR includes)

add_executable(${PROJECT_NAME} ${SRC_DIR}/main.cpp)

file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE HEADER_FILES ${INCLUDE_DIR}/*.h ${INCLUDE_DIR}/*.hpp)

target_sources(${PROJECT_NAME} PRIVATE ${SRC_FILES} ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})

find_package( CURL REQUIRED )
target_link_libraries(${PROJECT_NAME} CURL::libcurl)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)

# find_package(ImageMagick COMPONENTS Magick++ REQUIRED)
# target_link_libraries(${PROJECT_NAME} ImageMagick::Magick++)
# target_include_directories(${PROJECT_NAME} PRIVATE ${ImageMagick_INCLUDE_DIRS})
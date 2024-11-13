# Minimum CMake version requirement
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(EchoBrian)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure pkg-config is available
find_package(PkgConfig REQUIRED)

# Use pkg-config to find opus and websocketpp
pkg_check_modules(OPUS REQUIRED opus)
#find_package(OPUS REQUIRED)
find_package(WEBSOCKETPP REQUIRED)

# Include directories for opus and websocketpp
include_directories(
    ${OPUS_INCLUDE_DIRS}
    ${WEBSOCKETPP_INCLUDE_DIRS}
)

# Link directories for opus and websocketpp (if needed)
link_directories(
    ${OPUS_LIBRARY_DIRS}
    ${WEBSOCKETPP_LIBRARY_DIRS}
)

# Add the source files
add_executable(EchoBrian
    main.cc
    AudioProcess.cc
    WebsocketClient.cc
)

# Link libraries for opus and websocketpp
target_link_libraries(EchoBrian
    ${OPUS_LIBRARIES}
    ${WEBSOCKETPP_LIBRARIES}
)

# Additional compile options if needed
target_compile_options(EchoBrian PRIVATE ${OPUS_CFLAGS_OTHER} ${WEBSOCKETPP_CFLAGS_OTHER})

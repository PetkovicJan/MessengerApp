cmake_minimum_required(VERSION 3.25)

# Set the project name.
project(NetworkingApp)

# Set C++ standard.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Testing.
enable_testing()

# JSON settings.
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")

add_subdirectory(${PROJECT_SOURCE_DIR}/ext)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
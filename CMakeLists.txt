cmake_minimum_required(VERSION 3.23)

# Set the project name.
project(NetworkingApp)

# Set C++ standard.
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# JSON settings
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")

add_subdirectory(${PROJECT_SOURCE_DIR}/ext/json)
add_subdirectory(${PROJECT_SOURCE_DIR}/ext/sqlite)

add_subdirectory(${PROJECT_SOURCE_DIR}/sources/NetworkingCore)
add_subdirectory(${PROJECT_SOURCE_DIR}/sources/ServerApp)
add_subdirectory(${PROJECT_SOURCE_DIR}/sources/ClientApp)


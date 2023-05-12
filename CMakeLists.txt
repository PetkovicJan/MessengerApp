cmake_minimum_required(VERSION 3.23)

# Set the project name.
project(NetworkingApp)

# Set C++ standard.
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(${PROJECT_SOURCE_DIR}/sources/NetworkingCore)
add_subdirectory(${PROJECT_SOURCE_DIR}/sources/ServerApp)
add_subdirectory(${PROJECT_SOURCE_DIR}/sources/ClientApp)


# Set the project name.
project(sqlite)

# Add the executable.
add_library(${PROJECT_NAME} sqlite3.c sqlite3.h)

# Include directory.
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
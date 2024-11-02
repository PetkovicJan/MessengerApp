# Set the project name.
project(ServerApp)

# Set source files.
set(CPP_DIR ${PROJECT_SOURCE_DIR}/src)
set(CPP_FILES ${CPP_DIR}/main.cpp)

# Add the executable.
add_executable(${PROJECT_NAME} ${CPP_FILES})

# Link libraries.
set(LIBRARIES NetworkingCore)
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBRARIES})

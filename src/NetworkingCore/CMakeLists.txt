# Set the project name.
project(NetworkingCore)

# Set header files.
set(HPP_DIR ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME})
set(HPP_FILES 
  ${HPP_DIR}/Client.h
  ${HPP_DIR}/Connection.h
  ${HPP_DIR}/Message.h
  ${HPP_DIR}/MessengerDefines.h
  ${HPP_DIR}/MessengerServer.h
  ${HPP_DIR}/MessengerClient.h
  ${HPP_DIR}/Server.h
  ${HPP_DIR}/Socket.h
  ${HPP_DIR}/ThreadSafeQueue.h
  ${HPP_DIR}/UsersDB.h)

# Set source files.
set(CPP_DIR ${PROJECT_SOURCE_DIR}/src)
set(CPP_FILES 
  ${CPP_DIR}/Client.cpp
  ${CPP_DIR}/Connection.cpp
  ${CPP_DIR}/MessengerServer.cpp
  ${CPP_DIR}/MessengerClient.cpp
  ${CPP_DIR}/Server.cpp
  ${CPP_DIR}/Socket.cpp
  ${CPP_DIR}/UsersDB.cpp)

# Add the executable.
add_library(${PROJECT_NAME} STATIC ${CPP_FILES} ${HPP_FILES})

# Add include directory.
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Link libraries.
set(LIBRARIES Ws2_32.lib nlohmann_json::nlohmann_json sqlite picosha2)
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBRARIES})

# Add tests.
add_executable(tests tests/tests.cpp)
target_link_libraries(tests PRIVATE ${PROJECT_NAME} GTest::gtest_main)

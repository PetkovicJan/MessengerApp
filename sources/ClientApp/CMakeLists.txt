# Set the project name.
project(ClientApp)

find_package(Qt5 COMPONENTS REQUIRED Core Widgets)

# Set header files.
set(HPP_DIR ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME})
set(HPP_FILES 
  ${HPP_DIR}/MessengerAppQt.h
  ${HPP_DIR}/MessengerClientQt.h
  ${HPP_DIR}/UsersModelQt.h)

# Set source files.
set(CPP_DIR ${PROJECT_SOURCE_DIR}/src)
set(CPP_FILES 
  ${CPP_DIR}/main.cpp
  ${CPP_DIR}/MessengerAppQt.cpp
  ${CPP_DIR}/MessengerClientQt.cpp
  ${CPP_DIR}/UsersModelQt.cpp)

# Add the executable.
add_executable(${PROJECT_NAME} ${CPP_FILES} ${HPP_FILES})

# Add include directory.
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Link libraries.
set(LIBRARIES NetworkingCore Qt5::Widgets Qt5::Core)
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBRARIES})

# Configure Qt moc.
set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC TRUE AUTORCC TRUE)

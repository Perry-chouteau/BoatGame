cmake_minimum_required(VERSION 3.24)
project(boat)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILES main.cpp timing.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC .)

add_subdirectory(cyclone)
target_link_libraries(${PROJECT_NAME} PUBLIC cyclone)

if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework GLUT")
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework OpenGL")
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa")
endif()

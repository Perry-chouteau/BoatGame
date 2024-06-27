cmake_minimum_required(VERSION 3.24)
project(cyclone)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include(FetchContent)

#set(GLFW_VERSION "3.4")
#FetchContent_Declare(glfw
#    GIT_REPOSITORY https://github.com/glfw/glfw
#    GIT_TAG        "${GLFW_VERSION}"
#    )
#FetchContent_MakeAvailable(glfw)

set(SOURCE_FILES body.cpp
                collide_fine.cpp
                core.cpp
                joints.cpp
                pcontacts.cpp
                plinks.cpp
                random.cpp
                collide_coarse.cpp
                contacts.cpp
                fgen.cpp
                particle.cpp
                pfgen.cpp
                pworld.cpp
                world.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

set(INCLUDE_DIR include)
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/gl
)

set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(${PROJECT_NAME} PUBLIC GLUT::GLUT OpenGL::GL)

if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework GLUT")
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework OpenGL")
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa")
endif()

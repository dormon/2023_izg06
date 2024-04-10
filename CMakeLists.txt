cmake_minimum_required(VERSION 3.15)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1 ) 


project(IZG06)

add_subdirectory(libs/SDL-release-2.30.1)
add_subdirectory(libs/geGL)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE libs/glm-0.9.9.8)

add_executable(${PROJECT_NAME} src/main.cpp src/bunny.cpp src/bunny.hpp)

target_link_libraries(${PROJECT_NAME} SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(${PROJECT_NAME} geGL::geGL dl glm)

target_include_directories(${PROJECT_NAME} PUBLIC src/)

add_custom_target(run ./${PROJECT_NAME})

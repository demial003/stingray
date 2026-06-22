cmake_minimum_required(VERSION 3.10)
project(stingray)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(stingray src/main.cpp utils/sphere.cpp utils/glad.c
                        utils/cylinder.cpp utils/mesh.cpp)

target_link_libraries(stingray OpenGL::GL glfw dl)

add_custom_target(
  copy-compile-commands ALL
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/compile_commands.json
          ${CMAKE_SOURCE_DIR}/compile_commands.json)

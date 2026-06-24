cmake_minimum_required(VERSION 3.10)
project(stingray)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      Debug
      CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNEDEBUG")

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

include_directories(include)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(
  stingray
  src/main.cpp
  utils/sphere.cpp
  utils/glad.c
  utils/cylinder.cpp
  utils/mesh.cpp
  src/particle.cpp
  src/pcontacts.cpp
  src/pfgen.cpp
  src/plinks.cpp)

target_link_libraries(stingray OpenGL::GL glfw dl)

add_custom_target(
  copy-compile-commands ALL
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/compile_commands.json
          ${CMAKE_SOURCE_DIR}/compile_commands.json)

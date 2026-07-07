cmake_minimum_required(VERSION 3.10)
project(stingray)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SOURCE_FILES
    src/main.cpp
    utils/sphere.cpp
    utils/glad.c
    utils/cylinder.cpp
    utils/mesh.cpp
    src/particle.cpp
    src/pcontacts.cpp
    src/pfgen.cpp
    src/pworld.cpp
    src/plinks.cpp)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      Debug
      CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_FLAGS_DEBUG)
set(CMAKE_CXX_FLAGS_RELEASE)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

include_directories(include external/imgui external/imgui/backends)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)

add_library(
  imgui STATIC
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)

target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_link_libraries(imgui PUBLIC glfw)

add_compile_options(-Wall -Wextra)
add_executable(stingray ${SOURCE_FILES})

target_link_libraries(stingray OpenGL::GL glfw dl glad imgui)

add_custom_target(
  copy-compile-commands ALL
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/compile_commands.json
          ${CMAKE_SOURCE_DIR}/compile_commands.json)

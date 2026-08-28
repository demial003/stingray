cmake_minimum_required(VERSION 3.13)
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

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

include_directories(include PRIVATE external/glm external/imgui
                    external/imgui/backends)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(BUILD_SHARED_LIBS
    OFF
    CACHE BOOL "" FORCE)
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

set(ASSIMP_BUILD_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS
    OFF
    CACHE BOOL "" FORCE)
add_subdirectory(external/assimp)

target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends)
target_link_libraries(imgui PUBLIC glfw)

add_executable(stingray ${SOURCE_FILES})
target_compile_options(
  stingray PRIVATE $<$<CONFIG:Debug>:-fsanitize=address,undefined
                   -fno-omit-frame-pointer -g>)
target_link_options(stingray PRIVATE
                    $<$<CONFIG:Debug>:-fsanitize=address,undefined>)

target_link_libraries(stingray OpenGL::GL glfw imgui assimp)

add_custom_target(
  copy-compile-commands ALL
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/compile_commands.json
          ${CMAKE_SOURCE_DIR}/compile_commands.json)

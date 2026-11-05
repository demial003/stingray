cmake_minimum_required(VERSION 3.14)
project(stingray)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      Debug
      CACHE STRING "" FORCE)
endif()

include(FetchContent)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 6f14f4792a0cde5d0cf2c910506724d61cb95834)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
  GIT_SHALLOW TRUE)
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG 9e6c0416fe1bc159b4fe029415a047d910cfb918)
FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG 66c592c3f4445f638a14f07f81c2cb476c09bd93)

set(BUILD_SHARED_LIBS
    OFF
    CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES
    OFF
    CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS
    OFF
    CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB
    ON
    CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS
    OFF
    CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS
    OFF
    CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL
    OFF
    CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS
    OFF
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glm glfw imgui assimp)

find_package(OpenGL REQUIRED)

file(GLOB IMGUI_SRC ${imgui_SOURCE_DIR}/imgui*.cpp)
add_library(
  imgui STATIC ${IMGUI_SRC} ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
               ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR}
                                        ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui PUBLIC glfw)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS src/*.cpp src/*.c)
add_executable(stingray ${SRC})
target_include_directories(stingray PRIVATE include external)
target_link_libraries(stingray PRIVATE glm::glm imgui assimp OpenGL::GL glfw)

option(ENABLE_SANITIZERS "Build with ASan/UBSan" ON)
target_compile_options(stingray PRIVATE $<$<CONFIG:Debug>:-Wall -Wshadow -g>)
if(ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(stingray PRIVATE -fsanitize=address,undefined
                                          -fno-omit-frame-pointer)
  target_link_options(stingray PRIVATE -fsanitize=address,undefined)
endif()

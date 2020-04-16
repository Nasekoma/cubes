cmake_minimum_required(VERSION 2.8)
project(cubes)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)

include_directories(
        ./cubes-libs
        ./cubes-shaders
        ./cubes-textures
        ./cubes-classes
)

include_directories(${OPENGL_INCLUDE_DIRS})

add_executable(cubes main.cpp)

target_link_libraries(cubes ${OPENGL_LIBRARIES} glfw ${CMAKE_SOURCE_DIR}/cubes-libs/SOIL.lib)

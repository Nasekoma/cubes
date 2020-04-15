cmake_minimum_required(VERSION 3.15)
project(ALCHEMYgraph)

set(CMAKE_CXX_STANDARD 14)

set(OpenGlLinkers -lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor)

include_directories(
        ./cubes-libs
        ./cubes-shaders
        ./cubes-textures
        ./cubes-classes
)

add_executable(cubes main.cpp)

target_link_libraries(cubes ${OpenGlLinkers} ${CMAKE_SOURCE_DIR}/cubes-libs/SOIL.lib)
cmake_minimum_required(VERSION 4.2)
project(learnopengl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(includes)
link_directories(lib)

add_executable(learnopengl src/main.cpp
        src/glad.c
        src/Shader.h
        src/Shader.cpp
        src/stb_image.cpp
        src/Camera.h
)

target_link_libraries(learnopengl
        glfw3
        GL
        X11
        pthread
        Xrandr
        Xi
        dl
)


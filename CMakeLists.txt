cmake_minimum_required(VERSION 3.9)
project(GPUBasedRendering_A2)

FIND_PACKAGE(OpenGL REQUIRED)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp main.h global.h system.cpp system.h utility.cpp utility.h draw.cpp draw.h Shader.cpp Shader.h
        drawable.cpp drawable.h glutils.cpp glutils.h tgaio.cpp tgaio.h vbomesh.cpp vbomesh.h)

include_directories(include)

link_directories(lib)

add_executable(GPUBasedRendering_A2 ${SOURCE_FILES})

target_link_libraries(GPUBasedRendering_A2 ${OPENGL_LIBRARIES} glew32.lib glew32s.lib freeglut.lib)
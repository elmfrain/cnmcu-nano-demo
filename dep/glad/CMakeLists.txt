cmake_minimum_required(VERSION 3.2)

project("glad")

cmake_policy(SET CMP0072 NEW)
find_package(OpenGL REQUIRED)

add_library(glad STATIC
    src/glad.c
)

target_link_libraries(glad PUBLIC
    ${CMAKE_DL_LIBS}
)

target_include_directories(glad PUBLIC
    include
)
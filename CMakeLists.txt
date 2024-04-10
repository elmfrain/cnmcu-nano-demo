cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)

project(cnmcu-nano-demo)

add_subdirectory("dep/glm")
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL " " FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON  CACHE BOOL " " FORCE) #Only enable obj importer
set(ASSIMP_NO_EXPORT          ON  CACHE BOOL " " FORCE)
set(ASSIMP_BUILD_TESTS        OFF CACHE BOOL " " FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL " " FORCE)
set(ASSIMP_INSTALL            OFF CACHE BOOL " " FORCE)
set(BUILD_SHARED_LIBS         OFF CACHE BOOL " " FORCE)
add_subdirectory("dep/assimp")
add_subdirectory("dep/lua")
add_subdirectory("dep")

if(NOT EMSCRIPTEN)
  add_subdirectory("dep/glfw")
  add_subdirectory("dep/glad")
endif()

set(PROJECT_SOURCES
  src/main.cpp
  src/Visualizer.cpp
  src/Logger.cpp
  src/Input.cpp
  src/SceneObject.cpp
  src/Camera.cpp
  src/MeshObject.cpp
  src/MeshBuilder.cpp
  src/VisualizerScene.cpp
  src/Mesh.cpp
  src/stb_image.cpp
  src/LightObject.cpp
  src/Framebuffer.cpp
  src/LuaIndexable.cpp
  src/CodeNodeNano.cpp
  src/mos6502.cpp

  src/shaders/Shader.cpp
  src/shaders/PhongShader.cpp
  src/shaders/Compositor.cpp

  src/animation/Smoother.cpp
  src/animation/Easing.cpp
  src/animation/Track.cpp
  src/animation/Timeline.cpp
)

set(PROJECT_INCLUDES
  include
  dep/glfw/include
  dep/glad/include
  dep/glm
  dep/imgui
  dep/imgui/backends
)

set(PROJECT_LIBRARIES
  glfw
  glad
  glm
  assimp
  lua
  imgui
)

if(EMSCRIPTEN)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 --preload-file res")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_WEBGL2=1 -s ALLOW_MEMORY_GROWTH=1")

  list(REMOVE_ITEM PROJECT_INCLUDES dep/glad/include)
  list(REMOVE_ITEM PROJECT_LIBRARIES glad)
endif()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

if(EMSCRIPTEN)
  set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
      OUTPUT_NAME ${PROJECT_NAME}
      SUFFIX ".html"
  )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDES})

target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBRARIES})

if(NOT (${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR}))
  add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/res
    ${CMAKE_CURRENT_BINARY_DIR}/res
    COMMENT "Copying resources to build directory"
  )

  add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/toolchain
    ${CMAKE_CURRENT_BINARY_DIR}/toolchain
    COMMENT "Copying toolchain to build directory"
  )
endif()
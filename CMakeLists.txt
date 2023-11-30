cmake_minimum_required(VERSION 3.1)

project(sare-init-visulizer)

add_subdirectory("dep/glfw")

add_subdirectory("dep/glad")

add_subdirectory("dep/glm")

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/Visualizer.cpp
  src/Logger.cpp
  src/Input.cpp
  src/SceneObject.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
  include
  dep/glfw/include
  dep/glad/include
  dep/glm
)

target_link_libraries(${PROJECT_NAME}
  glfw
  glad
  glm
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/res
  ${CMAKE_CURRENT_BINARY_DIR}/res
  COMMENT "Copying resources to build directory"
)
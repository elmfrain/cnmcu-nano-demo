cmake_minimum_required(VERSION 3.22)

# --- ImGui --- #

project(imgui)

add_library(${PROJECT_NAME}
    ./imgui/imgui.cpp
    ./imgui/imgui_demo.cpp
    ./imgui/imgui_draw.cpp
    ./imgui/imgui_tables.cpp
    ./imgui/imgui_widgets.cpp
    
    ./imgui/backends/imgui_impl_glfw.cpp
    ./imgui/backends/imgui_impl_opengl3.cpp

    ./imgui/misc/cpp/imgui_stdlib.cpp

    ./ImGuiColorTextEdit/TextEditor.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ./imgui
    ./imguibackends
    ./imgui/misc/cpp
    ./ImGuiColorTextEdit
    ./glfw/include/
)
cmake_minimum_required(VERSION 3.20)
project(imguiwindow CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(imguiwindow
    main.cpp
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imguiwindow PRIVATE imgui imgui/backends stb)
target_link_libraries(imguiwindow PRIVATE glfw OpenGL::GL)

configure_file(bigrat.jpg ${CMAKE_BINARY_DIR}/bigrat.jpg COPYONLY)

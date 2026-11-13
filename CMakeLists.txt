cmake_minimum_required(VERSION 3.16)
project(imgui-test)
set(CMAKE_CXX_STANDARD 17)
find_package(SDL2 REQUIRED)

add_executable(app
    main.cpp
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_sdl2.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(app PRIVATE imgui imgui/backends)
target_link_libraries(app PRIVATE SDL2::SDL2 GLESv2 EGL)
target_compile_definitions(app PRIVATE IMGUI_IMPL_OPENGL_ES3)

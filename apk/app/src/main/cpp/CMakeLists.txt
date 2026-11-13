cmake_minimum_required(VERSION 3.10)
project(imguitest)
add_library(imguitest SHARED main.cpp
  ../../../../../imgui/imgui.cpp
  ../../../../../imgui/imgui_draw.cpp
  ../../../../../imgui/imgui_widgets.cpp
  ../../../../../imgui/imgui_tables.cpp
  ../../../../../imgui/imgui_demo.cpp
  ../../../../../imgui/backends/imgui_impl_android.cpp
  ../../../../../imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imguitest PRIVATE ../../../../../imgui ../../../../../imgui/backends .)
target_link_libraries(imguitest log android EGL GLESv3 -static-libstdc++)
target_compile_definitions(imguitest PRIVATE IMGUI_IMPL_OPENGL_ES3)
target_compile_options(imguitest PRIVATE -static-libstdc++)

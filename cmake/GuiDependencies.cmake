find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG QUIET)
if(NOT TARGET glfw)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(
        GLFW3
        REQUIRED
        IMPORTED_TARGET
        glfw3)
    add_library(glfw ALIAS PkgConfig::GLFW3)
endif()

include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.9b
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(imgui)

add_library(
    imgui
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp")
target_include_directories(imgui SYSTEM PUBLIC "${imgui_SOURCE_DIR}"
                                               "${imgui_SOURCE_DIR}/backends")
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

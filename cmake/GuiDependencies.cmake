include(FetchContent)
application_pin_fetchcontent_dependency(
    imgui https://github.com/ocornut/imgui.git
    f5befd2d29e66809cd1110a152e375a7f1981f06)
FetchContent_MakeAvailable(imgui)
FetchContent_GetProperties(imgui SOURCE_DIR fetched_imgui_source_dir)
set(IMGUI_SOURCE_DIR "${fetched_imgui_source_dir}")
set(IMGUI_SOURCE_DIR
    "${fetched_imgui_source_dir}"
    PARENT_SCOPE)

add_library(
    imgui
    "${IMGUI_SOURCE_DIR}/imgui.cpp"
    "${IMGUI_SOURCE_DIR}/imgui_draw.cpp"
    "${IMGUI_SOURCE_DIR}/imgui_tables.cpp"
    "${IMGUI_SOURCE_DIR}/imgui_widgets.cpp"
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp")
target_include_directories(
    imgui SYSTEM
    PUBLIC
        $<BUILD_INTERFACE:${IMGUI_SOURCE_DIR}>
        $<BUILD_INTERFACE:${IMGUI_SOURCE_DIR}/backends>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PACKAGE_NAME}/imgui>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PACKAGE_NAME}/imgui/backends>
)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

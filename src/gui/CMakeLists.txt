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
include(GuiDependencies)

add_library(gui application.cpp preferences.cpp)
set_target_properties(gui PROPERTIES EXPORT_NAME gui)
set_target_properties(imgui PROPERTIES EXPORT_NAME imgui)
target_include_directories(
    gui
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PACKAGE_NAME}>
        $<BUILD_INTERFACE:${IMGUI_SOURCE_DIR}>
        $<BUILD_INTERFACE:${IMGUI_SOURCE_DIR}/backends>)
target_link_libraries(
    gui
    PUBLIC imgui
           glfw
           OpenGL::GL
           core
           application_options)
if(ENABLE_COVERAGE)
    target_link_options(
        gui
        INTERFACE
        -fprofile-arcs
        -ftest-coverage)
    target_link_libraries(gui INTERFACE gcov)
endif()

if(BUILD_GUI_RUNTIME)
    add_executable(${GUI_EXECUTABLE_NAME} ${CMAKE_SOURCE_DIR}/src/gui_main.cpp)
    target_link_libraries(
        ${GUI_EXECUTABLE_NAME}
        PRIVATE app
                application_options
                gui
                CLI11::CLI11)
    target_compile_definitions(${GUI_EXECUTABLE_NAME} PRIVATE HAS_GUI=1)
endif()

cmake_minimum_required(VERSION 3.25)

project(im_cpp
    VERSION 0.1.0
    DESCRIPTION "C++23 port of the im application"
    LANGUAGES CXX
)

option(IM_CPP_ENABLE_CLANG_TIDY "Run clang-tidy during compilation when available" OFF)
option(IM_CPP_BUILD_TESTS "Build and register unit tests" ON)
option(IM_CPP_BUILD_GUI "Build the ImGui GLFW/OpenGL desktop GUI" ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerSettings)

add_library(im_cpp_options INTERFACE)
target_compile_features(im_cpp_options INTERFACE cxx_std_23)
set_target_properties(im_cpp_options PROPERTIES
    CXX_EXTENSIONS OFF
)

im_cpp_configure_compiler_settings(im_cpp_options)

find_package(spdlog CONFIG QUIET)
if(NOT spdlog_FOUND)
    include(FetchContent)
    set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "Disable spdlog tests" FORCE)
    set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "Disable spdlog examples" FORCE)
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.17.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(spdlog)
endif()

find_package(CLI11 CONFIG QUIET)
if(NOT CLI11_FOUND)
    include(FetchContent)
    set(CLI11_BUILD_TESTS OFF CACHE BOOL "Disable CLI11 tests" FORCE)
    FetchContent_Declare(
        CLI11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
        GIT_TAG v2.4.2
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(CLI11)
endif()

add_library(im_core
    src/core/gui_log_sink.cpp
    src/core/log_level.cpp
    src/core/logger.cpp
)
target_include_directories(im_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(im_core PUBLIC im_cpp_options spdlog::spdlog)

add_library(im_app src/app/cli.cpp)
target_include_directories(im_app PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(im_app PUBLIC im_core im_cpp_options CLI11::CLI11)

add_executable(im-cpp src/main.cpp)
target_link_libraries(im-cpp PRIVATE im_app im_cpp_options)

if(IM_CPP_BUILD_GUI)
    include(GuiDependencies)
    add_library(im_gui
        src/gui/application.cpp
        src/gui/preferences.cpp
    )
    target_include_directories(im_gui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(im_gui PUBLIC imgui im_core im_cpp_options)
    target_link_libraries(im-cpp PRIVATE im_gui)
    target_link_libraries(im_app PRIVATE im_gui)
    target_compile_definitions(im-cpp PRIVATE IM_CPP_HAS_GUI=1)
    target_compile_definitions(im_app PRIVATE IM_CPP_HAS_GUI=1)
else()
    target_compile_definitions(im-cpp PRIVATE IM_CPP_HAS_GUI=0)
    target_compile_definitions(im_app PRIVATE IM_CPP_HAS_GUI=0)
endif()

if(IM_CPP_BUILD_TESTS)
    include(CTest)
    include(FetchContent)

    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.8.1
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(im_cpp_tests
        tests/application_info_tests.cpp
        tests/cli_tests.cpp
        tests/gui_log_sink_tests.cpp
        tests/logger_tests.cpp
        tests/log_level_tests.cpp
    )
    target_include_directories(im_cpp_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(im_cpp_tests PRIVATE Catch2::Catch2WithMain im_app im_core im_cpp_options)
    if(IM_CPP_BUILD_GUI)
        target_link_libraries(im_cpp_tests PRIVATE im_gui)
        target_compile_definitions(im_cpp_tests PRIVATE IM_CPP_HAS_GUI=1)
        target_sources(im_cpp_tests PRIVATE tests/preferences_tests.cpp)
    else()
        target_compile_definitions(im_cpp_tests PRIVATE IM_CPP_HAS_GUI=0)
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
        target_compile_options(im_cpp_tests PRIVATE -Wno-c2y-extensions)
    endif()
    add_test(NAME im_cpp_tests COMMAND im_cpp_tests)
endif()

if(IM_CPP_ENABLE_CLANG_TIDY)
    find_program(IM_CPP_CLANG_TIDY_EXECUTABLE NAMES clang-tidy REQUIRED)
    if(IM_CPP_BUILD_TESTS AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
        message(STATUS "clang-tidy is disabled on Catch2 tests for Clang toolchains")
    endif()
    set(clang_tidy_targets im_core im_app im-cpp)
    if(IM_CPP_BUILD_GUI)
        list(APPEND clang_tidy_targets im_gui)
    endif()
    foreach(target IN LISTS clang_tidy_targets)
        set_target_properties(${target} PROPERTIES
            CXX_CLANG_TIDY "${IM_CPP_CLANG_TIDY_EXECUTABLE}"
        )
    endforeach()
endif()

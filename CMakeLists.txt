cmake_minimum_required(VERSION 3.25)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/ProjectMetadata.cmake")

project(
    "${CMAKE_PACKAGE_NAME}"
    VERSION "${PROJECT_VERSION}"
    DESCRIPTION "${PROJECT_DESCRIPTION}"
    HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}"
    LANGUAGES CXX)

set(MINIMUM_GCC_VERSION 14)
if(NOT
   CMAKE_CXX_COMPILER_ID
   STREQUAL
   "GNU")
    message(
        FATAL_ERROR
            "im-cpp requires GCC. Configure with -DCMAKE_CXX_COMPILER=g++.")
endif()
if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS MINIMUM_GCC_VERSION)
    message(
        FATAL_ERROR
            "im-cpp requires GCC ${MINIMUM_GCC_VERSION} or newer; found ${CMAKE_CXX_COMPILER_VERSION}."
    )
endif()

option(ENABLE_CLANG_TIDY "Run clang-tidy during compilation when available" OFF)
option(BUILD_TESTS "Build and register unit tests" ON)
option(ENABLE_COVERAGE "Instrument project code for Debug gcovr reports" OFF)
option(BUILD_GUI "Build the ImGui GLFW/OpenGL desktop GUI" ON)
option(BUILD_GUI_RUNTIME "Build the separately packaged GUI runtime executable"
       OFF)
if(BUILD_GUI_RUNTIME AND NOT BUILD_GUI)

    message(FATAL_ERROR "BUILD_GUI_RUNTIME requires BUILD_GUI=ON.")
endif()
set(PACKAGE_NAME
    "${DISTRIBUTION_NAME}"
    CACHE STRING "CPack package name")
set_property(CACHE PACKAGE_NAME PROPERTY STRINGS "${DISTRIBUTION_NAME}"
                                         "${DISTRIBUTION_NAME}-gui")
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GNUInstallDirs)
include(CompilerSettings)
include(DependencyPins)
set(INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PACKAGE_NAME}")
set(GENERATED_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(GENERATED_METADATA_HEADER
    "${GENERATED_INCLUDE_DIR}/core/application_metadata.h")
file(MAKE_DIRECTORY "${GENERATED_INCLUDE_DIR}/core")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/application_metadata.h.in"
               "${GENERATED_METADATA_HEADER}" @ONLY)

add_library(im_cpp_options INTERFACE)
set_target_properties(im_cpp_options PROPERTIES EXPORT_NAME options)
target_compile_features(im_cpp_options INTERFACE cxx_std_23)
set_target_properties(im_cpp_options PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(
    im_cpp_options
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${GENERATED_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PACKAGE_NAME}>)

find_package(
    spdlog
    1.17.0
    EXACT
    CONFIG
    QUIET)
if(NOT spdlog_FOUND)
    include(FetchContent)
    set(SPDLOG_BUILD_TESTS
        OFF
        CACHE BOOL "Disable spdlog tests" FORCE)
    set(SPDLOG_BUILD_EXAMPLE
        OFF
        CACHE BOOL "Disable spdlog examples" FORCE)
    im_cpp_pin_fetchcontent_dependency(
        spdlog https://github.com/gabime/spdlog.git
        79524ddd08a4ec981b7fea76afd08ee05f83755d)
    FetchContent_MakeAvailable(spdlog)
endif()

find_package(
    CLI11
    2.4.2
    EXACT
    CONFIG
    QUIET)
if(NOT CLI11_FOUND)
    include(FetchContent)
    set(CLI11_BUILD_TESTS
        OFF
        CACHE BOOL "Disable CLI11 tests" FORCE)
    im_cpp_pin_fetchcontent_dependency(
        CLI11 https://github.com/CLIUtils/CLI11.git
        6c7b07a878ad834957b98d0f9ce1dbe0cb204fc9)
    set(CMAKE_POLICY_VERSION_MINIMUM 3.10)
    FetchContent_MakeAvailable(CLI11)
    unset(CMAKE_POLICY_VERSION_MINIMUM)
endif()

add_library(im_core src/core/gui_log_sink.cpp src/core/log_level.cpp
                    src/core/logger.cpp)
set_target_properties(im_core PROPERTIES EXPORT_NAME core)
if(ENABLE_COVERAGE)
    target_link_options(
        im_core
        INTERFACE
        -fprofile-arcs
        -ftest-coverage)
    target_link_libraries(im_core INTERFACE gcov)
endif()
target_link_libraries(im_core PUBLIC im_cpp_options spdlog::spdlog)

add_library(im_app src/app/cli.cpp)
set_target_properties(im_app PROPERTIES EXPORT_NAME app)
target_include_directories(
    im_app
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${GENERATED_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PACKAGE_NAME}>)
target_link_libraries(im_app PUBLIC im_core im_cpp_options)
if(ENABLE_COVERAGE)
    target_link_options(
        im_app
        INTERFACE
        -fprofile-arcs
        -ftest-coverage)
    target_link_libraries(im_app INTERFACE gcov)
endif()

add_library(im_cli src/app/main_entry.cpp)
set_target_properties(im_cli PROPERTIES EXPORT_NAME cli)
target_link_libraries(im_cli PUBLIC im_app im_cpp_options CLI11::CLI11)
if(ENABLE_COVERAGE)
    target_link_options(
        im_cli
        INTERFACE
        -fprofile-arcs
        -ftest-coverage)
    target_link_libraries(im_cli INTERFACE gcov)
endif()

add_executable(${EXECUTABLE_NAME} src/main.cpp)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE im_cli)

target_include_directories(
    im_app SYSTEM
    PRIVATE "$<TARGET_PROPERTY:CLI11::CLI11,INTERFACE_INCLUDE_DIRECTORIES>")

if(BUILD_GUI)
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
    add_library(im_gui src/gui/application.cpp)
    set_target_properties(im_gui PROPERTIES EXPORT_NAME gui)
    if(ENABLE_COVERAGE)
        target_link_options(
            im_gui
            INTERFACE
            -fprofile-arcs
            -ftest-coverage)
        target_link_libraries(im_gui INTERFACE gcov)
    endif()
    set_target_properties(imgui PROPERTIES EXPORT_NAME imgui)
    target_include_directories(
        im_gui
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PACKAGE_NAME}>
            $<BUILD_INTERFACE:${imgui_SOURCE_DIR}>
            $<BUILD_INTERFACE:${imgui_SOURCE_DIR}/backends>)
    target_link_libraries(
        im_gui
        PUBLIC imgui
               glfw
               OpenGL::GL
               im_core
               im_cpp_options)
    target_sources(im_gui PRIVATE src/gui/preferences.cpp)
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE im_gui)
    if(BUILD_GUI_RUNTIME)
        add_executable(${GUI_EXECUTABLE_NAME} src/gui_main.cpp)
        target_link_libraries(
            ${GUI_EXECUTABLE_NAME}
            PRIVATE im_app
                    im_cpp_options
                    im_gui
                    CLI11::CLI11)
        target_compile_definitions(${GUI_EXECUTABLE_NAME} PRIVATE HAS_GUI=1)
    endif()
    target_compile_definitions(${EXECUTABLE_NAME} PRIVATE HAS_GUI=1)
else()
    target_compile_definitions(${EXECUTABLE_NAME} PRIVATE HAS_GUI=0)
endif()

set(im_cpp_compile_targets
    im_core
    im_app
    im_cli
    ${EXECUTABLE_NAME})
if(BUILD_GUI)
    list(APPEND im_cpp_compile_targets im_gui)
    if(BUILD_GUI_RUNTIME)
        list(APPEND im_cpp_compile_targets ${GUI_EXECUTABLE_NAME})
    endif()
endif()
foreach(target IN LISTS im_cpp_compile_targets)
    apply_common_compile_options(${target})
endforeach()

if(BUILD_TESTS)
    include(CTest)
    include(FetchContent)

    find_package(
        Catch2
        3.8.1
        CONFIG
        QUIET
        COMPONENTS Catch2WithMain)
    if(NOT Catch2_FOUND)
        im_cpp_pin_fetchcontent_dependency(
            Catch2 https://github.com/catchorg/Catch2.git
            2b60af89e23d28eefc081bc930831ee9d45ea58b)
        FetchContent_MakeAvailable(Catch2)
        set(Catch2_SOURCE_DIR "${catch2_SOURCE_DIR}")
    else()
        set(Catch2_SOURCE_DIR "${Catch2_DIR}/../../../..")
    endif()
    list(APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/extras")
    include(Catch)

    file(
        GLOB_RECURSE
        im_cpp_test_sources
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/core/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/app/*.cpp")
    if(BUILD_GUI)
        file(
            GLOB_RECURSE
            im_cpp_gui_test_sources
            CONFIGURE_DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/gui/*.cpp")
        list(APPEND im_cpp_test_sources ${im_cpp_gui_test_sources})
    endif()
    add_executable(im_cpp_tests ${im_cpp_test_sources})
    target_link_libraries(
        im_cpp_tests
        PRIVATE Catch2::Catch2WithMain
                im_app
                im_core
                im_cpp_options)
    target_link_libraries(im_cpp_tests PRIVATE CLI11::CLI11)
    apply_common_compile_options(im_cpp_tests)
    if(BUILD_GUI)
        target_link_libraries(im_cpp_tests PRIVATE im_gui)
        if(INSTALL_GUI)
            target_link_libraries(im_cpp_tests PRIVATE ${GUI_EXECUTABLE_NAME})
        endif()
    endif()
    apply_unit_test_compile_options(im_cpp_tests)
    if(ENABLE_COVERAGE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
        apply_coverage_compile_options(im_cpp_tests)
        apply_unit_test_link_options(im_cpp_tests)
        foreach(target IN ITEMS im_core im_app)
            apply_coverage_compile_options(${target})
        endforeach()
        target_link_options(
            im_app
            INTERFACE
            -fprofile-arcs
            -ftest-coverage)
        target_link_libraries(im_app INTERFACE gcov)
        if(BUILD_GUI)
            apply_coverage_compile_options(im_gui)
        endif()
    endif()
    catch_discover_tests(im_cpp_tests)
    add_test(NAME im_cpp_cli_check COMMAND ${EXECUTABLE_NAME} check)
    add_test(NAME im_cpp_cli_version COMMAND ${EXECUTABLE_NAME} --version)
    if(BUILD_GUI_RUNTIME)
        add_test(NAME im_cpp_gui_cli_version COMMAND ${GUI_EXECUTABLE_NAME}
                                                     --version)
    endif()
endif()

if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy REQUIRED)
    set(clang_tidy_targets im_core im_app ${EXECUTABLE_NAME})
    if(BUILD_GUI)
        list(APPEND clang_tidy_targets im_gui)
    endif()
    foreach(target IN LISTS clang_tidy_targets)
        set_target_properties(
            ${target}
            PROPERTIES CXX_CLANG_TIDY
                       "${CLANG_TIDY_EXECUTABLE};--warnings-as-errors=-*")
    endforeach()
endif()

include(InstallRules)

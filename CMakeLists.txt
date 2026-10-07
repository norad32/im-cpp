cmake_minimum_required(VERSION 3.25)

project(im_cpp
    VERSION 0.1.0
    DESCRIPTION "C++23 port of the im application"
    LANGUAGES CXX
)

option(IM_CPP_ENABLE_CLANG_TIDY "Run clang-tidy during compilation when available" OFF)
option(IM_CPP_BUILD_TESTS "Build and register unit tests" ON)

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

add_executable(im-cpp src/main.cpp)
set_target_properties(im-cpp PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
target_include_directories(im-cpp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(im-cpp PRIVATE im_cpp_options)

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

    add_executable(im_cpp_tests tests/application_info_tests.cpp)
    target_include_directories(im_cpp_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    target_link_libraries(im_cpp_tests PRIVATE Catch2::Catch2WithMain im_cpp_options)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
        target_compile_options(im_cpp_tests PRIVATE -Wno-c2y-extensions)
    endif()
    add_test(NAME im_cpp_tests COMMAND im_cpp_tests)
endif()

if(IM_CPP_ENABLE_CLANG_TIDY)
    find_program(IM_CPP_CLANG_TIDY_EXECUTABLE NAMES clang-tidy REQUIRED)
    set_target_properties(im-cpp PROPERTIES
        CXX_CLANG_TIDY "${IM_CPP_CLANG_TIDY_EXECUTABLE}"
    )
endif()

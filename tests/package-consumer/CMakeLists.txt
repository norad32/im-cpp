cmake_minimum_required(VERSION 3.25)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/ProjectMetadata.cmake")
project(im_cpp_package_consumer LANGUAGES CXX)

find_package(
    ${IM_CPP_CMAKE_PACKAGE_NAME}
    ${IM_CPP_PROJECT_VERSION}
    CONFIG
    REQUIRED)

add_executable(im_cpp_package_consumer main.cpp)
target_link_libraries(im_cpp_package_consumer PRIVATE im_cpp::app)

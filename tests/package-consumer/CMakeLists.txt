cmake_minimum_required(VERSION 3.25)
project(im_cpp_package_consumer LANGUAGES CXX)

find_package(
    im_cpp
    0.1
    CONFIG
    REQUIRED)

add_executable(im_cpp_package_consumer main.cpp)
target_link_libraries(im_cpp_package_consumer PRIVATE im_cpp::app)

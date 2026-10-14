include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/ProjectMetadata.cmake")

project(package_consumer LANGUAGES CXX)

find_package(
    ${CMAKE_PACKAGE_NAME}
    ${PROJECT_VERSION}
    CONFIG
    REQUIRED
    COMPONENTS app)

add_executable(package_consumer main.cpp)
target_link_libraries(package_consumer PRIVATE ${PACKAGE_NAMESPACE}app)

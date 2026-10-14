add_library(app cli.cpp)
set_target_properties(app PROPERTIES EXPORT_NAME app)
target_include_directories(
    app
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${GENERATED_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PACKAGE_NAME}>)
target_link_libraries(app PUBLIC core application_options)
if(ENABLE_COVERAGE)
    target_link_options(
        app
        INTERFACE
        -fprofile-arcs
        -ftest-coverage)
    target_link_libraries(app INTERFACE gcov)
endif()
target_include_directories(
    app SYSTEM
    PRIVATE "$<TARGET_PROPERTY:CLI11::CLI11,INTERFACE_INCLUDE_DIRECTORIES>")

add_library(cli main_entry.cpp)
set_target_properties(cli PROPERTIES EXPORT_NAME cli)
target_link_libraries(cli PUBLIC app application_options CLI11::CLI11)
if(ENABLE_COVERAGE)
    target_link_options(
        cli
        INTERFACE
        -fprofile-arcs
        -ftest-coverage)
    target_link_libraries(cli INTERFACE gcov)
endif()

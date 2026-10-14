include(CTest)
include(FetchContent)

find_package(
    Catch2
    ${CATCH2_PACKAGE_VERSION}
    CONFIG
    QUIET
    COMPONENTS Catch2WithMain)
if(NOT Catch2_FOUND)
    application_pin_fetchcontent_dependency(
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
    test_sources
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp")
if(BUILD_GUI)
    file(
        GLOB_RECURSE
        gui_test_sources
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/gui/*.cpp")
    list(APPEND test_sources ${gui_test_sources})
endif()
add_executable(tests ${test_sources})
target_link_libraries(
    tests
    PRIVATE Catch2::Catch2WithMain
            app
            core
            application_options
            CLI11::CLI11)
apply_common_compile_options(tests)
if(BUILD_GUI)
    target_link_libraries(tests PRIVATE gui)
    if(INSTALL_GUI)
        target_link_libraries(tests PRIVATE ${GUI_EXECUTABLE_NAME})
    endif()
endif()
apply_unit_test_compile_options(tests)
if(ENABLE_COVERAGE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    apply_coverage_compile_options(tests)
    apply_unit_test_link_options(tests)
    foreach(target IN ITEMS core app)
        apply_coverage_compile_options(${target})
    endforeach()
    target_link_options(
        app
        INTERFACE
        -fprofile-arcs
        -ftest-coverage)
    target_link_libraries(app INTERFACE gcov)
    if(BUILD_GUI)
        apply_coverage_compile_options(gui)
    endif()
endif()
catch_discover_tests(tests)
add_test(NAME cli_check COMMAND ${EXECUTABLE_NAME} check)
add_test(NAME cli_version COMMAND ${EXECUTABLE_NAME} --version)
if(BUILD_GUI_RUNTIME)
    add_test(NAME gui_cli_version COMMAND ${GUI_EXECUTABLE_NAME} --version)
endif()

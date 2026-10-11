include_guard(GLOBAL)

set(WARNING_OPTIONS
    -Werror
    -Wpedantic
    -Wall
    -Wextra
    -Wabi
    -Wmissing-declarations
    -Wconversion
    -Wshadow
    -Wlogical-op
    -Wfloat-equal
)

set(DEBUGGING_OPTIONS -g3)
set(COMMON_OPT_OPTIONS -ffunction-sections -fdata-sections)
set(DEBUG_OPT_OPTIONS ${COMMON_OPT_OPTIONS} -Og)
set(UNIT_TEST_COMPILE_OPTIONS -fprofile-arcs -ftest-coverage -fno-elide-constructors)
set(RELEASE_OPT_OPTIONS ${COMMON_OPT_OPTIONS} -O2)
set(DIAGMSG_OPTIONS -fmessage-length=0)
set(CXX_OPTIONS -std=gnu++23)

function(apply_common_compile_options target_name)
    if(NOT TARGET ${target_name})
        message(FATAL_ERROR "apply_common_compile_options: target does not exist: ${target_name}")
    endif()

    get_target_property(target_type ${target_name} TYPE)
    if(target_type STREQUAL "INTERFACE_LIBRARY")
        set(scope INTERFACE)
    else()
        set(scope PRIVATE)
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${target_name} ${scope}
            ${WARNING_OPTIONS}
            ${DIAGMSG_OPTIONS}
            ${CXX_OPTIONS}
            $<$<CONFIG:Debug>:${DEBUG_OPT_OPTIONS}>
            $<$<CONFIG:Debug>:${DEBUGGING_OPTIONS}>
            $<$<CONFIG:Release>:${RELEASE_OPT_OPTIONS}>
        )
        target_compile_definitions(${target_name} ${scope}
            $<$<CONFIG:Release>:NDEBUG=1>
            $<$<CONFIG:Release>:RELEASE=1>
        )
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
        target_compile_options(${target_name} ${scope}
            -Werror
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wshadow
            -Wformat=2
            -Wfloat-equal
            -fmessage-length=0
            $<$<CONFIG:Debug>:-ffunction-sections>
            $<$<CONFIG:Debug>:-fdata-sections>
            $<$<CONFIG:Debug>:-Og>
            $<$<CONFIG:Debug>:-g3>
            $<$<CONFIG:Release>:-ffunction-sections>
            $<$<CONFIG:Release>:-fdata-sections>
            $<$<CONFIG:Release>:-Os>
        )
        target_compile_definitions(${target_name} ${scope}
            $<$<CONFIG:Release>:NDEBUG=1>
            $<$<CONFIG:Release>:RELEASE=1>
        )
    elseif(MSVC)
        target_compile_options(${target_name} ${scope} /W4 /WX)
    endif()
endfunction()

function(apply_unit_test_compile_options target_name)
    if(NOT TARGET ${target_name})
        message(FATAL_ERROR "apply_unit_test_compile_options: target does not exist: ${target_name}")
    endif()

    get_target_property(target_type ${target_name} TYPE)
    if(target_type STREQUAL "INTERFACE_LIBRARY")
        set(scope INTERFACE)
    else()
        set(scope PRIVATE)
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${target_name} ${scope} ${UNIT_TEST_COMPILE_OPTIONS})
    endif()
endfunction()

function(apply_unit_test_link_options target_name)
    if(NOT TARGET ${target_name})
        message(FATAL_ERROR "apply_unit_test_link_options: target does not exist: ${target_name}")
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_options(${target_name} PRIVATE -fprofile-arcs -ftest-coverage)
        target_link_libraries(${target_name} PRIVATE gcov)
    endif()
endfunction()

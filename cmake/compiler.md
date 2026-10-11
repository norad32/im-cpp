#[[
Warning options
See https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html for a complete list
and documentation of all options.

-fsyntax-only             Check the code for syntax errors, but don't do anything beyond that.
-fmax-errors=n            Limits the maximum number of error messages to n
-w                        Inhibit all warning messages.
-Werror                   Make all warnings into errors.
-Werror=                  Make the specified warning into an error.
-Wfatal-errors            Abort compilation on the first error occurred.
-Wpedantic                Issue all the warnings demanded by strict ISO C and ISO C+.
-Wall                     Turns most warning flags on.
-Wextra                   Enables some extra warning flags that are not enabled by -Wall.
-Wabi                     Warn about code affected by ABI changes.
-Wmissing-declarations    Warn if a global function is defined without a previous declaration.
-Wconversion              Warn for implicit conversions that may alter a value.
-Wshadow                  Warn whenever a local variable or type declaration shadows another variable, parameter, type, class member (in C++), or whenever a built-in function is shadowed.
-Wlogical-op              Warn about suspicious uses of logical operators in expressions
-Wfloat-equal             Warn if floating-point values are used in equality comparisons.
]]
set(WARNING_OPTIONS
    -Werror
    -Wpedantic
    -Wall
    -Wextra
    -Wmissing-declarations
    -Wconversion
    -Wshadow
    -Wlogical-op
    -Wfloat-equal)

#[[
Debugging options
See https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html for a complete list and documentation of all options.

-glevel                   Request debugging information and also use level to specify how much information. The default level is 2.
-g0                       Level 0 produces no debug information at all. Thus, -g0 negates -g.
-g1                       Level 1 produces minimal information, enough for making backtraces in parts of the program that you don't plan to debug. This includes descriptions of functions and external variables, and line number tables, but no information about local variables.
-g3                       Level 3 includes extra information, such as all the macro definitions present in the program. Some debuggers support macro expansion when you use -g3.
]]
set(DEBUGGING_OPTIONS -g3)

#[[
Optimization options
See https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html for a complete list and documentation of all options.

-O1                       Optimize. Optimizing compilation takes somewhat more time, and a lot more memory for a large function.
-O2                       Optimize even more. Turns on all optimization flags specified by -O1.
-O3                       Optimize yet more. -O3 turns on all optimizations specified by -O2.
-O0                       Reduce compilation time and make debugging produce the expected results. This is the default.
-Os                       Optimize for size. -Os enables all -O2 optimizations except those that often increase code size.
-Og                       Optimize debugging experience. -Og should be the optimization level of choice for the standard edit-compile-debug cycle, offering a reasonable level of optimization while maintaining fast compilation and a good debugging experience.
-Oz                       Optimize aggressively for size rather than speed. This may increase the number of instructions executed if those instructions require fewer bytes to encode. -Oz behaves similarly to -Os including enabling most -O2 optimizations.
-ffunction-sections       Place each function or data item into its own section in the output file if the target supports arbitrary sections.
-fdata-sections           Place each function or data item into its own section in the output file if the target supports arbitrary sections.
-fmove-loop-invariants    Enables the loop invariant motion pass in the RTL loop optimizer. Enabled at level -O1 and higher, except for -Og.
-flto[=n]                 This option runs the standard link-time optimizer.
-fprofile-arcs            Instruments the program to generate extra code to write profile information suitable for gcov.
-ftest-coverage           Instruments the program to generate extra code to write test coverage information suitable for gcov.
-fno-elide-constructors   Disable the optimization that allows the compiler to omit the copy and move constructors and their corresponding assignment operators. This optimization is called copy elision and is enabled by default in C++17 and later standards. By using -fno-elide-constructors, you can force the compiler to generate calls to the copy and move constructors, which can be useful for debugging or testing purposes.
]]
set(COMMON_OPT_OPTIONS -ffunction-sections -fdata-sections)

set(DEBUG_OPT_OPTIONS ${COMMON_OPT_OPTIONS} -Og)

set(UNIT_TEST_COMPILE_OPTIONS -fprofile-arcs -ftest-coverage
                              -fno-elide-constructors)

set(RELEASE_OPT_OPTIONS ${COMMON_OPT_OPTIONS} -Os)

#[[
Diagnostic Message Formatting options
See https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Message-Formatting-Options.html#Diagnostic-Message-Formatting-Options for a complete list and documentation of all options.

-fmessage-length=n    Try to format error messages so that they fit on lines of about n characters. If n is zero, then no line-wrapping is done; each error message appears on a single line.
]]
set(DIAGMSG_OPTIONS -fmessage-length=0)

#[[
C Language options
See https://gcc.gnu.org/onlinedocs/gcc/C-Dialect-Options.html for a complete list and documentation of all options.

-std=                 Determine the language standard.
-fsigned-char         Let the type char be signed, like signed char.
-fno-builtin          Don't recognize built-in functions that do not begin with '__builtin_' as prefix.
-ffreestanding        Assert that compilation targets a freestanding environment. This implies -fno-builtin. A freestanding environment is one in which the standard library may not exist, and program startup may not necessarily be at main.
]]
set(C_OPTIONS -std=gnu17 -fsigned-char -ffreestanding)

#[[
C++ Language options
See https://gcc.gnu.org/onlinedocs/gcc/C_002b_002b-Dialect-Options.html for a complete list and documentation of all options.

-std=                 Determine the language standard.
]]
set(CXX_OPTIONS -std=gnu++23)

set(ASM_OPTIONS -x assembler-with-cpp)

#[[
Applies the common compile options and compile definitions to a target.

The function validates that <target_name> exists, then applies:
  - warning options
  - diagnostic message formatting options
  - C-specific options for C sources
  - C++-specific options for C++ sources
  - ASM-specific options for assembly sources
  - Debug optimization and debug information options for Debug builds
  - Release optimization options for Release builds

Arguments:
  target_name
    Name of the target that should receive the common compile options.
]]
function(apply_common_compile_options target_name)
    if(NOT TARGET ${target_name})
        message(
            FATAL_ERROR
                "apply_common_compile_options: target does not exist: ${target_name}"
        )
    endif()

    target_compile_options(
        ${target_name}
        PRIVATE ${WARNING_OPTIONS}
                ${DIAGMSG_OPTIONS}
                $<$<COMPILE_LANGUAGE:C>:${C_OPTIONS}>
                $<$<COMPILE_LANGUAGE:CXX>:${CXX_OPTIONS}>
                $<$<COMPILE_LANGUAGE:ASM>:${ASM_OPTIONS}>
                $<$<CONFIG:Debug>:${DEBUG_OPT_OPTIONS}>
                $<$<CONFIG:Debug>:${DEBUGGING_OPTIONS}>
                $<$<CONFIG:Release>:${RELEASE_OPT_OPTIONS}>)

    target_compile_definitions(
        ${target_name} PRIVATE $<$<CONFIG:Release>:NDEBUG=1>
                               $<$<CONFIG:Release>:RELEASE=1>)
endfunction()

#[[
Applies unit-test-specific compile options to a target.

This function adds the unit-test debug optimization and coverage-related compile options.

Arguments:
  target_name
    Name of the target that should receive the unit-test compile options.
]]
function(apply_unit_test_compile_options target_name)
    if(NOT TARGET ${target_name})
        message(FATAL_ERROR "Target does not exist: ${target_name}")
    endif()

    message(
        STATUS
            "Applying unit test compile options to ${target_name}: ${UNIT_TEST_COMPILE_OPTIONS}"
    )

    get_target_property(target_type "${target_name}" TYPE)

    set(scope PRIVATE)

    if(target_type STREQUAL "INTERFACE_LIBRARY")
        set(scope INTERFACE)
    endif()

    target_compile_options(${target_name} ${scope} ${UNIT_TEST_COMPILE_OPTIONS})
endfunction()

#[[
Applies unit-test-specific link options to a target.

This function adds coverage-related linker options and links the target against
gcov.

Arguments:
  target_name
    Name of the target that should receive the unit-test link options.
]]
function(apply_unit_test_link_options target_name)
    target_link_options(
        ${target_name}
        PRIVATE
        -fprofile-arcs
        -ftest-coverage)

    target_link_libraries(${target_name} PRIVATE gcov)
endfunction()

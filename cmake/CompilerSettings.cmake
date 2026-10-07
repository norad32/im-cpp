include_guard(GLOBAL)

# Apply the project's baseline warnings to project-owned targets only.
# Keeping these settings on an interface target avoids imposing them on dependencies.
function(im_cpp_configure_compiler_settings target)
    if(MSVC)
        target_compile_options(${target} INTERFACE /W4 /WX)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        target_compile_options(${target} INTERFACE
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wformat=2
            -Werror
        )
    endif()
endfunction()

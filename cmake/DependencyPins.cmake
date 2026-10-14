function(
    application_pin_fetchcontent_dependency
    dependency_name
    repository
    commit)
    string(TOLOWER "${dependency_name}" dependency_prefix)
    set(${dependency_prefix}_SOURCE_DIR
        "${PROJECT_SOURCE_DIR}/third_party/${dependency_prefix}"
        CACHE PATH "Path to ${dependency_name} sources")
    if(EXISTS "${${dependency_prefix}_SOURCE_DIR}/CMakeLists.txt")
        set(FETCHCONTENT_SOURCE_DIR_${dependency_prefix}
            "${${dependency_prefix}_SOURCE_DIR}"
            CACHE PATH "Use vendored ${dependency_name} sources" FORCE)
    else()
        FetchContent_Declare(
            ${dependency_name}
            GIT_REPOSITORY ${repository}
            GIT_TAG ${commit}
            GIT_SHALLOW FALSE)
    endif()
endfunction()

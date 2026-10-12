include(CMakePackageConfigHelpers)

install(
    TARGETS im_cpp_options im_core im_app
    EXPORT im_cppTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    INCLUDES
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/im_cpp"
    COMPONENT Development)

if(IM_CPP_BUILD_GUI)
    install(
        TARGETS im_gui imgui
        EXPORT im_cppTargets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        INCLUDES
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/im_cpp"
        COMPONENT Development)

    install(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/im_cpp/gui"
        COMPONENT Development
        FILES_MATCHING
        PATTERN "*.h")

    install(
        FILES "${imgui_SOURCE_DIR}/imgui.h"
              "${imgui_SOURCE_DIR}/imconfig.h"
              "${imgui_SOURCE_DIR}/imgui_internal.h"
              "${imgui_SOURCE_DIR}/imstb_rectpack.h"
              "${imgui_SOURCE_DIR}/imstb_textedit.h"
              "${imgui_SOURCE_DIR}/imstb_truetype.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/im_cpp/imgui"
        COMPONENT Development)

    install(
        FILES "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h"
              "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/im_cpp/imgui/backends"
        COMPONENT Development)

    install(
        FILES "${imgui_SOURCE_DIR}/LICENSE.txt"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/licenses/im_cpp/imgui"
        COMPONENT Development)
endif()
install(TARGETS im-cpp RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
                               COMPONENT Runtime)
if(IM_CPP_BUILD_GUI_RUNTIME)
    install(TARGETS im-cpp-gui RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
                                       COMPONENT Gui)
endif()

install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/im_cpp"
    COMPONENT Development
    FILES_MATCHING
    PATTERN "*.h")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/im_cppConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/im_cppConfig.cmake"
    INSTALL_DESTINATION "${IM_CPP_INSTALL_CMAKEDIR}")
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/im_cppConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion)

install(
    EXPORT im_cppTargets
    FILE im_cppTargets.cmake
    NAMESPACE im_cpp::
    DESTINATION "${IM_CPP_INSTALL_CMAKEDIR}"
    COMPONENT Development)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/im_cppConfig.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/im_cppConfigVersion.cmake"
    DESTINATION "${IM_CPP_INSTALL_CMAKEDIR}"
    COMPONENT Development)

set(CPACK_PACKAGE_NAME "${IM_CPP_PACKAGE_NAME}")
set(CPACK_PACKAGE_VENDOR "norad32")
set(CPACK_PACKAGE_CONTACT "norad32")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++23 port of the im application")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_FILE_NAME
    "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS ON)
set(CPACK_COMPONENTS_ALL Runtime Development)
if(IM_CPP_BUILD_GUI_RUNTIME)
    list(APPEND CPACK_COMPONENTS_ALL Gui)
endif()
set(CPACK_DEBIAN_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "norad32")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_RUNTIME_PACKAGE_SHLIBDEPS ON)
if(IM_CPP_INSTALL_GUI AND NOT IM_CPP_BUILD_GUI_RUNTIME)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libglfw3, libgl1")
endif()
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
if(IM_CPP_BUILD_GUI_RUNTIME)
    set(CPACK_DEB_COMPONENT_INSTALL ON)
    set(CPACK_DEBIAN_COMPONENT_INSTALL ON)
    set(CPACK_DEBIAN_GUI_FILE_NAME DEB-DEFAULT)
    set(CPACK_DEBIAN_PACKAGE_GUI_NAME "im-cpp-gui")
    set(CPACK_DEBIAN_PACKAGE_GUI_DEPENDS
        "${CPACK_PACKAGE_NAME}, libglfw3, libgl1")
    set(CPACK_DEBIAN_GUI_PACKAGE_SHLIBDEPS ON)
endif()
include(CPack)

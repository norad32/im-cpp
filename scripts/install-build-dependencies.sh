#!/usr/bin/env bash
"""Install missing C++ build, GUI, and formatting dependencies on apt/pacman Linux.

Installs compiler, CMake, Ninja, clang-tidy, clang-format, GLFW/OpenGL development
packages, and cmake-format via pipx when needed. Requires sudo/root for system packages.
"""
set -euo pipefail

if (( EUID == 0 )); then
    SUDO=()
elif command -v sudo >/dev/null 2>&1; then
    SUDO=(sudo)
else
    printf '%s\n' "This installer needs root privileges. Install sudo or run it as root." >&2
    exit 1
fi

install_apt() {
    local -a packages=()
    local package
    local -a required_packages=(
        build-essential
        cmake
        ninja-build
        clang-tidy
        clang-format
        valgrind
        libglfw3-dev
        libgl1-mesa-dev
        pkg-config
        python3-pip
        pipx
    )

    for package in "${required_packages[@]}"; do
        if [[ "$(dpkg-query -W -f='${db:Status-Status}' "$package" 2>/dev/null || true)" != "installed" ]]; then
            packages+=("$package")
        fi
    done

    if ((${#packages[@]})); then
        "${SUDO[@]}" apt-get update
        "${SUDO[@]}" apt-get install -y "${packages[@]}"
    fi

    if ! command -v cmake-format >/dev/null 2>&1 && apt-cache show cmake-format >/dev/null 2>&1; then
        "${SUDO[@]}" apt-get install -y cmake-format
    fi
}

install_pacman() {
    local -a packages=()
    local package
    local -a required_packages=(
        base-devel
        cmake
        ninja
        clang
        clang-tools-extra
        valgrind
        glfw-x11
        mesa
        pkgconf
        python-pipx
    )

    for package in "${required_packages[@]}"; do
        if ! pacman -Q "$package" >/dev/null 2>&1; then
            packages+=("$package")
        fi
    done

    if ((${#packages[@]})); then
        "${SUDO[@]}" pacman -S --needed --noconfirm "${packages[@]}"
    fi
}

if command -v apt-get >/dev/null 2>&1; then
    install_apt
elif command -v pacman >/dev/null 2>&1; then
    install_pacman
else
    printf '%s\n' "Unsupported package manager. This script supports apt and pacman." >&2
    exit 1
fi

CMAKE_FORMAT_BIN=""
PIPX_BIN_DIR=""
if ! command -v cmake-format >/dev/null 2>&1; then
    if ! command -v pipx >/dev/null 2>&1; then
        printf '%s\n' "pipx is required to install cmake-format." >&2
        exit 1
    fi
    if ! pipx list --short 2>/dev/null | awk '$1 == "cmakelang" { found = 1 } END { exit !found }'; then
        pipx install cmakelang
    fi

    PIPX_BIN_DIR="$(pipx environment --value PIPX_BIN_DIR)"
    if [[ -x "${PIPX_BIN_DIR}/cmake-format" ]]; then
        CMAKE_FORMAT_BIN="${PIPX_BIN_DIR}/cmake-format"
        if [[ ":${PATH}:" != *":${PIPX_BIN_DIR}:"* ]]; then
            pipx ensurepath
            export PATH="${PIPX_BIN_DIR}:${PATH}"
        fi
    fi
fi

for tool in g++ cmake ninja clang-tidy clang-format; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        printf 'Dependency installation completed, but %s is still not on PATH.\n' "$tool" >&2
        exit 1
    fi
done

for tool in g++ cmake ninja clang-tidy clang-format cmake-format valgrind; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        printf 'Dependency installation completed, but %s is still not on PATH.\n' "$tool" >&2
        exit 1
    fi
done

printf '%s\n' "Build and formatting tools are installed."

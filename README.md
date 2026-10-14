# im-cpp

[![Build and test](https://github.com/norad32/im-cpp/actions/workflows/build.yml/badge.svg)](https://github.com/norad32/im-cpp/actions/workflows/build.yml)

A C++23 starter template. It provides a command-line interface and optional Dear ImGui desktop GUI. The project targets Linux and requires GCC 14 or newer.

## Build and run

Requirements include CMake 3.25 or newer, Ninja, and GCC 14 or newer. GUI builds also need GLFW 3 and OpenGL development libraries. Install supported tools on apt- or pacman-based Linux distributions with `./scripts/install-build-dependencies.sh`.

Build and test the headless configuration:

```sh
cmake --preset release-headless
cmake --build --preset release-headless
ctest --preset release-headless
./build/release-headless/im-cpp check
```

The presets use `g++`. If your GCC 14+ compiler has a versioned name, pass it when configuring, for example `-DCMAKE_CXX_COMPILER=g++-14`. CMake caches the compiler choice, so use a fresh build directory when switching compilers.

Available presets are `debug`, `debug-gui`, `debug-headless`, `release-gui`, and `release-headless`. GUI support is enabled by default. Use `-DBUILD_GUI=OFF` for a headless build. `BUILD_TESTS` controls the Catch2 test suite, and `ENABLE_CLANG_TIDY` enables clang-tidy during compilation. Compiler warnings are treated as errors.

Use `im-cpp --version` to print the version and `im-cpp --log-level DEBUG check` to set the logging level. In GUI builds, run `im-cpp gui` or start without a command to open the interface. GUI preferences are saved as `preferences.conf` under `$XDG_CONFIG_HOME/im-cpp/`, or `$HOME/.config/im-cpp/` when `XDG_CONFIG_HOME` is unset or relative. If neither location is available, preferences use defaults and are not saved.

CMake finds spdlog, CLI11, and Catch2 in installed packages or fetches pinned revisions for spdlog and CLI11. It fetches Dear ImGui for GUI builds.

## Develop and test

Run the suite with `ctest --preset release-headless` or `ctest --preset release-gui`. Pushes build and test the headless Release preset. Pull requests do the same and additionally generate a source coverage summary; same-repository pull requests receive it as a comment, and all pull requests upload it as an artifact.

## Install and package

Prebuilt Linux archives and Debian and Arch packages are attached to each [GitHub release](https://github.com/norad32/im-cpp/releases). The archives install under `/usr` and contain the headless or GUI runtime executable; the GUI executable is `im-cpp-gui` and requires GLFW and OpenGL runtime libraries.

To install the Debian headless packages, download the `headless-*.deb` assets and run:

```sh
sudo apt install ./headless-*.deb
```

To install the GUI packages, download the `gui-*.deb` assets and run:

```sh
sudo apt install ./gui-*.deb
```

The GUI set includes the `im-cpp-gui` runtime package and its runtime dependencies. Install all matching assets together so apt can resolve package dependencies. The headless set contains Runtime and Development packages; the GUI set additionally contains the GUI executable package.

On Arch Linux, download the `.pkg.tar.zst` assets and install the split runtime, GUI, and development packages with:

```sh
sudo pacman -U ./im-cpp-*.pkg.tar.zst
```

To install a prebuilt archive instead, extract it into the filesystem root:

```sh
sudo tar -C / -xzf im-cpp-headless-<version>-linux-x86_64.tar.gz
```

Use `im-cpp-gui-<version>-linux-x86_64.tar.gz` for the GUI archive. Both archives install executables under `/usr/bin` and the license under `/usr/share/licenses/im-cpp`. The GUI archive includes both `im-cpp` and `im-cpp-gui`.

To build packages locally, configure and build the desired release preset. CPack generates Debian packages; the generated Arch `PKGBUILD` can be built on Arch Linux with `makepkg --syncdeps --cleanbuild` from its directory.

```sh
cmake --preset release-headless
cmake --build --preset release-headless
cpack --config build/release-headless/CPackConfig.cmake -G DEB -B build/packages/headless

cmake --preset release-gui
cmake --build --preset release-gui
cpack --config build/release-gui/CPackConfig.cmake -G DEB -B build/packages/gui
```

The GUI preset builds a separate `im-cpp-gui` runtime. Its Debian packages include Development and Runtime components, plus Gui for the separate GUI executable.

Install a build locally with `cmake --install build/release-headless --prefix <install-prefix>`. The Development component includes libraries, headers, and CMake package files. Runtime installs the executable.

CMake consumers can request the application component and link the exported target:

```cmake
find_package(application CONFIG REQUIRED COMPONENTS app)
target_link_libraries(my_target PRIVATE application::app)
```

## Project layout

```text
.github/workflows/   CI checks
cmake/               Build, dependency, and packaging configuration
scripts/             Build dependency installer
src/                 Application, CLI, core, and GUI implementation
tests/               Unit tests and package consumer
```

## Credits

| Library | Purpose | License |
| --- | --- | --- |
| [Dear ImGui](https://github.com/ocornut/imgui) | Immediate-mode GUI | MIT |
| [GLFW](https://github.com/glfw/glfw) | Window and input handling | zlib/libpng |
| [CLI11](https://github.com/CLIUtils/CLI11) | Command-line parsing | BSD-3-Clause |
| [spdlog](https://github.com/gabime/spdlog) | Logging | MIT |
| [Catch2](https://github.com/catchorg/Catch2) | Testing | Boost Software License 1.0 |
| [CMake](https://cmake.org/) | Build configuration and packaging | BSD-3-Clause |

## License

This project is licensed under the [MIT License](LICENSE). Third-party components retain their respective licenses.

## Author

[norad32](https://github.com/norad32)

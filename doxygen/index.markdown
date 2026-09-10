Getting Started
===============

@tableofcontents

This project implements a set of foundational C++ libraries to power Sourcemeta
projects.

Live at head
------------

Like projects such as [`GoogleTest`](https://github.com/google/googletest),
this project follows the [Abseil Live at
Head](https://abseil.io/about/philosophy#upgrade-support) philosophy. We
recommend always following the latest commit in the `main` branch.

Installation
------------

### As a git submodule

```sh
# Add the project as a submodule, in this case to deps/core
$ git submodule add https://github.com/sourcemeta/core.git deps/core

# Add the project as a CMake subdirectory
add_subdirectory("${PROJECT_SOURCE_DIR}/deps/core")

# Link your targets accordingly
target_link_libraries(my_executable_or_library PUBLIC sourcemeta::core::json)
target_link_libraries(my_executable_or_library PUBLIC sourcemeta::core::jsonschema)
target_link_libraries(my_executable_or_library PUBLIC sourcemeta::core::jsonpointer)
target_link_libraries(my_executable_or_library PUBLIC sourcemeta::core::jsonl)
```

### Using FetchContent

```cmake
cmake_minimum_required(VERSION 3.14)
project(my_project)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
  core
  URL https://github.com/sourcemeta/core/archive/<commit-sha>.zip
  DOWNLOAD_EXTRACT_TIMESTAMP NO)
FetchContent_MakeAvailable(core)

add_executable(my_example ...)
target_link_libraries(my_example PUBLIC sourcemeta::core::json)
target_link_libraries(my_example PUBLIC sourcemeta::core::jsonschema)
target_link_libraries(my_example PUBLIC sourcemeta::core::jsonpointer)
target_link_libraries(my_example PUBLIC sourcemeta::core::jsonl)
```

### Using find_package

Assuming you have installed Sourcemeta Core in a place where CMake can find the
package config file:

```sh
find_package(Core REQUIRED)

# This package also supports component-based inclusion
find_package(Core REQUIRED COMPONENTS json jsonschema jsonpointer jsonl)

# Link your targets accordingly
target_link_libraries(my_executable_or_library PUBLIC sourcemeta::core::json)
target_link_libraries(my_executable_or_library PUBLIC sourcemeta::core::jsonschema)
target_link_libraries(my_executable_or_library PUBLIC sourcemeta::core::jsonpointer)
target_link_libraries(my_executable_or_library PUBLIC sourcemeta::core::jsonl)
```

CMake
-----

### Options

Check the top-level
[`CMakeLists.txt`](https://github.com/sourcemeta/core/blob/main/CMakeLists.txt)
for the available CMake options.

### Components

If you install Sourcemeta Core from source, keep in mind that the project is split
into a set of CMake components:

| Component             | Description                                       |
|-----------------------|---------------------------------------------------|
| `sourcemeta_core`     | The Sourcemeta Core libraries                     |
| `sourcemeta_core_dev` | The Sourcemeta Core headers and development files |

Contributing
------------

You can configure, build and test the project by simply runnning:

```sh
# On UNIX-based systems
make
# On Windows
nmake
```

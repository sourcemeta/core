cmake_minimum_required(VERSION 3.24)
project(core_http_usockets LANGUAGES CXX)

# Experimental Linux loopback fixture, not a public HTTP server API. Build a
# uSockets checkout at 166a04810c9fa970fc22096a7b35b792378364fb with WITH_LTO=0,
# then configure this directory with -DUSOCKETS_ROOT=/path/to/uSockets.
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(FATAL_ERROR "The HTTP over uSockets experiment currently targets Linux")
endif()

find_package(Python3 3.8 REQUIRED COMPONENTS Interpreter)
set(USOCKETS_ROOT "" CACHE PATH "Path to the pinned, built uSockets checkout")
find_path(USOCKETS_INCLUDE_DIR libusockets.h
          PATHS "${USOCKETS_ROOT}/src" NO_DEFAULT_PATH REQUIRED)
find_file(USOCKETS_LIBRARY uSockets.a
          PATHS "${USOCKETS_ROOT}" NO_DEFAULT_PATH REQUIRED)

# Build the actual parser in this checkout. Nothing adds this experiment or
# its external dependency to Core's normal build or installation.
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../.." core EXCLUDE_FROM_ALL)
add_executable(core_http_usockets server.cc)
target_compile_features(core_http_usockets PRIVATE cxx_std_23)
set_target_properties(core_http_usockets PROPERTIES CXX_EXTENSIONS OFF)
target_compile_options(core_http_usockets PRIVATE -Wall -Wextra -Wpedantic)
target_include_directories(core_http_usockets SYSTEM PRIVATE "${USOCKETS_INCLUDE_DIR}")
target_link_libraries(core_http_usockets PRIVATE
                      sourcemeta::core::http "${USOCKETS_LIBRARY}")

enable_testing()
add_test(NAME core.http.usockets
         COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/test.py"
                 $<TARGET_FILE:core_http_usockets>)
set_tests_properties(core.http.usockets PROPERTIES TIMEOUT 30)

# Opt-in example using an installed Core. From the Core checkout:
# cmake -S examples/mcp-deployment-security -B build/mcp-deployment-security \
#   -DCMAKE_PREFIX_PATH=/path/to/core/prefix
# cmake --build build/mcp-deployment-security
# ctest --test-dir build/mcp-deployment-security --output-on-failure
cmake_minimum_required(VERSION 3.24)
project(mcp_deployment_security LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(Core REQUIRED COMPONENTS mcp crypto test)
find_package(Threads REQUIRED)
enable_testing()
add_executable(mcp_deployment_security main.cc)
target_link_libraries(mcp_deployment_security PRIVATE
  sourcemeta::core::mcp sourcemeta::core::crypto
  sourcemeta::core::test_main Threads::Threads)
add_test(NAME mcp.deployment_security COMMAND mcp_deployment_security)

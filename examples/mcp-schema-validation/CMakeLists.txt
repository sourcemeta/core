# Standalone, opt-in integration example. Install Core and Blaze into a common
# prefix, then run from the Core checkout:
# cmake -S examples/mcp-schema-validation -B build/mcp-schema-validation \
#   -DCMAKE_PREFIX_PATH=/path/to/prefix
# cmake --build build/mcp-schema-validation
# ctest --test-dir build/mcp-schema-validation --output-on-failure
# Nothing in Core's default build discovers or links Blaze.
cmake_minimum_required(VERSION 3.24)
project(mcp_schema_validation LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT TARGET sourcemeta::core::mcp)
  find_package(Core REQUIRED COMPONENTS mcp jsonschema test)
endif()
if(NOT TARGET sourcemeta::blaze::compiler)
  find_package(Blaze REQUIRED COMPONENTS compiler evaluator)
endif()
enable_testing()
add_executable(mcp_schema_validation main.cc)
target_link_libraries(mcp_schema_validation PRIVATE
  sourcemeta::core::mcp sourcemeta::core::jsonschema
  sourcemeta::core::test_main sourcemeta::blaze::compiler
  sourcemeta::blaze::evaluator)
add_test(NAME mcp.application_schema_validation COMMAND mcp_schema_validation)

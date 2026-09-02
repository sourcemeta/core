# TODO: Get install path from a variable?
add_test(NAME ${PROJECT_NAME}.find_package_configure COMMAND
  "${CMAKE_COMMAND}"
  -S "${CMAKE_CURRENT_SOURCE_DIR}/find_package"
  -B "${CMAKE_CURRENT_BINARY_DIR}/find_package"
  "-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}"
  "-DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH};${PROJECT_SOURCE_DIR}/build/dist"
  "-DCMAKE_TOOLCHAIN_FILE:PATH=${CMAKE_TOOLCHAIN_FILE}")
add_test(NAME ${PROJECT_NAME}.find_package_build COMMAND
  "${CMAKE_COMMAND}"
  --build "${CMAKE_CURRENT_BINARY_DIR}/find_package"
  --config "${CMAKE_BUILD_TYPE}")
set_tests_properties(${PROJECT_NAME}.find_package_build
  PROPERTIES DEPENDS ${PROJECT_NAME}.find_package_configure)

# This consumption path speaks in `-l` flags and relies on pkg-config being
# around, neither of which is how a library is found on Windows
find_program(MESON_EXECUTABLE NAMES meson)
if(MESON_EXECUTABLE AND NOT WIN32)
  # Unlike a plain setup, this works both on a fresh build directory and on one
  # left behind by a previous run, which is what makes the test repeatable
  add_test(NAME ${PROJECT_NAME}.meson_pkgconfig_setup COMMAND
    "${MESON_EXECUTABLE}" setup --reconfigure
    "${CMAKE_CURRENT_BINARY_DIR}/meson_pkgconfig"
    "${CMAKE_CURRENT_SOURCE_DIR}/meson_pkgconfig"
    "-Dpkg_config_path=${PROJECT_SOURCE_DIR}/build/dist/${CMAKE_INSTALL_LIBDIR}/pkgconfig")
  add_test(NAME ${PROJECT_NAME}.meson_pkgconfig_compile COMMAND
    "${MESON_EXECUTABLE}" compile -C "${CMAKE_CURRENT_BINARY_DIR}/meson_pkgconfig")
  set_tests_properties(${PROJECT_NAME}.meson_pkgconfig_compile
    PROPERTIES DEPENDS ${PROJECT_NAME}.meson_pkgconfig_setup)
endif()

# The allocator is the one dependency whose absence a consumer cannot notice on
# its own, as a program links and runs perfectly well without it
find_program(PKG_CONFIG_EXECUTABLE NAMES pkg-config)
if(PKG_CONFIG_EXECUTABLE AND NOT WIN32 AND TARGET Mimalloc::Mimalloc)
  add_test(NAME ${PROJECT_NAME}.pkg_config_allocator COMMAND
    "${PKG_CONFIG_EXECUTABLE}" --libs sourcemeta_core)
  set_tests_properties(${PROJECT_NAME}.pkg_config_allocator
    PROPERTIES
      PASS_REGULAR_EXPRESSION "mimalloc"
      ENVIRONMENT
        "PKG_CONFIG_PATH=${PROJECT_SOURCE_DIR}/build/dist/${CMAKE_INSTALL_LIBDIR}/pkgconfig")
endif()

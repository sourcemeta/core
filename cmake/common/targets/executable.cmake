function(sourcemeta_executable)
  cmake_parse_arguments(SOURCEMETA_EXECUTABLE ""
    "NAMESPACE;PROJECT;NAME;VARIANT;OUTPUT" "SOURCES" ${ARGN})

  if(NOT SOURCEMETA_EXECUTABLE_PROJECT)
    message(FATAL_ERROR "You must pass the project name using the PROJECT option")
  endif()
  if(NOT SOURCEMETA_EXECUTABLE_NAME)
    message(FATAL_ERROR "You must pass the executable name using the NAME option")
  endif()
  if(NOT SOURCEMETA_EXECUTABLE_SOURCES)
    message(FATAL_ERROR "You must pass the sources list using the SOURCES option")
  endif()

  if(SOURCEMETA_EXECUTABLE_NAMESPACE)
    set(TARGET_NAME "${SOURCEMETA_EXECUTABLE_NAMESPACE}_${SOURCEMETA_EXECUTABLE_PROJECT}_${SOURCEMETA_EXECUTABLE_NAME}")
    set(FOLDER_NAME "${SOURCEMETA_EXECUTABLE_NAMESPACE}/${SOURCEMETA_EXECUTABLE_PROJECT}/${SOURCEMETA_EXECUTABLE_NAME}")
  else()
    set(TARGET_NAME "${SOURCEMETA_EXECUTABLE_PROJECT}_${SOURCEMETA_EXECUTABLE_NAME}")
    set(FOLDER_NAME "${SOURCEMETA_EXECUTABLE_PROJECT}/${SOURCEMETA_EXECUTABLE_NAME}")
  endif()

  if(SOURCEMETA_EXECUTABLE_VARIANT)
    set(TARGET_NAME "${TARGET_NAME}_${SOURCEMETA_EXECUTABLE_VARIANT}")
  endif()

  if(SOURCEMETA_EXECUTABLE_OUTPUT)
    set("${SOURCEMETA_EXECUTABLE_OUTPUT}" "${TARGET_NAME}" PARENT_SCOPE)
  endif()

  add_executable("${TARGET_NAME}" ${SOURCEMETA_EXECUTABLE_SOURCES})
  sourcemeta_add_default_options(PRIVATE ${TARGET_NAME})

  # See https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Options-Hardening-Guide-for-C-and-C++.html
  # Position Independent Executable (PIE) for ASLR support
  if(SOURCEMETA_COMPILER_LLVM OR SOURCEMETA_COMPILER_GCC)
    target_compile_options(${TARGET_NAME} PRIVATE
      $<$<CONFIG:Release>:-fPIE>
      $<$<CONFIG:RelWithDebInfo>:-fPIE>)
    target_link_options(${TARGET_NAME} PRIVATE
      $<$<CONFIG:Release>:-pie>
      $<$<CONFIG:RelWithDebInfo>:-pie>)
  endif()

  # Linux-specific ELF linker hardening options
  if(LINUX AND (SOURCEMETA_COMPILER_LLVM OR SOURCEMETA_COMPILER_GCC))
    target_link_options(${TARGET_NAME} PRIVATE
      "LINKER:-z,nodlopen"
      "LINKER:-z,noexecstack"
      "LINKER:-z,relro"
      "LINKER:-z,now"
      "LINKER:--as-needed"
      "LINKER:--no-copy-dt-needed-entries")
  endif()

  set_target_properties("${TARGET_NAME}" PROPERTIES FOLDER "${FOLDER_NAME}")
endfunction()

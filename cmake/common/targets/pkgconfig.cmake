# Nothing refers to the entry points that replace the standard allocator by
# name, so a linker that only pulls in the archive members it needs would leave
# the program running on the allocator it was trying to replace
function(sourcemeta_pkgconfig_allocator OUTPUT_VARIABLE)
  if(BUILD_SHARED_LIBS)
    set(FLAGS "-lmimalloc")
  elseif(APPLE)
    set(FLAGS "-Wl,-force_load,\${libdir}/libmimalloc.a")
  elseif(SOURCEMETA_COMPILER_MSVC)
    set(FLAGS "/WHOLEARCHIVE:mimalloc.lib")
  else()
    set(FLAGS
      "-Wl,--whole-archive \${libdir}/libmimalloc.a -Wl,--no-whole-archive")
  endif()

  set("${OUTPUT_VARIABLE}" "${FLAGS}" PARENT_SCOPE)
endfunction()

function(sourcemeta_pkgconfig_dependency LIBRARY MODULES_VARIABLE LIBS_VARIABLE)
  set(MODULES ${${MODULES_VARIABLE}})
  set(LIBS ${${LIBS_VARIABLE}})

  # CMake keeps the private dependencies of a static library in its exported
  # link interface behind this expression, as they take part in linking but not
  # in compiling against the library
  string(REGEX REPLACE "^\\$<LINK_ONLY:(.+)>$" "\\1" LIBRARY "${LIBRARY}")

  if(LIBRARY MATCHES "^sourcemeta::")
    string(REPLACE "::" "_" MODULE "${LIBRARY}")
    list(APPEND MODULES "${MODULE}")
  elseif(LIBRARY STREQUAL "Mimalloc::Mimalloc")
    # The allocator is a property of the program rather than of any one module,
    # and it is pulled in whole. Naming it here would make a consumer that asks
    # for two modules at once hand the same archive to the linker twice, which
    # reports every symbol in it as duplicated. It ships as a module of its own
    # instead, which the aggregate carries
  elseif(LIBRARY STREQUAL "PCRE2::pcre2")
    list(APPEND LIBS "-lpcre2" "-lsljit")
  elseif(LIBRARY STREQUAL "LibDeflate::LibDeflate")
    list(APPEND LIBS "-ldeflate")
  elseif(LIBRARY STREQUAL "CMarkGFM::cmark_gfm")
    list(APPEND LIBS "-lcmark_gfm")
  elseif(LIBRARY STREQUAL "OpenSSL::Crypto")
    list(APPEND LIBS "-lcrypto")
  elseif(LIBRARY STREQUAL "CURL::libcurl")
    list(APPEND LIBS "-lcurl")
  elseif(LIBRARY STREQUAL "Threads::Threads")
    if(CMAKE_USE_PTHREADS_INIT)
      list(APPEND LIBS "-pthread")
    endif()
  elseif(LIBRARY MATCHES "\\.framework$")
    get_filename_component(FRAMEWORK "${LIBRARY}" NAME_WE)
    list(APPEND LIBS "-framework ${FRAMEWORK}")
  elseif(LIBRARY MATCHES "^-")
    list(APPEND LIBS "${LIBRARY}")
  elseif(NOT TARGET "${LIBRARY}" AND LIBRARY MATCHES "^[a-zA-Z0-9_]+$")
    # A bare name that names no target is what CMake reads as a library the
    # system already provides, which is linked the same way here
    list(APPEND LIBS "-l${LIBRARY}")
  else()
    message(FATAL_ERROR "Cannot express the dependency on ${LIBRARY} as a "
      "pkg-config flag. Teach ${CMAKE_CURRENT_FUNCTION} how to spell it, so "
      "that consumers that do not read CMake keep linking against it")
  endif()

  set("${MODULES_VARIABLE}" ${MODULES} PARENT_SCOPE)
  set("${LIBS_VARIABLE}" ${LIBS} PARENT_SCOPE)
endfunction()

function(sourcemeta_pkgconfig_direct
    TARGET_NAME MODULES_VARIABLE OPTIONS_VARIABLE LIBS_VARIABLE)
  set(MODULES ${${MODULES_VARIABLE}})
  set(OPTIONS ${${OPTIONS_VARIABLE}})
  set(LIBS ${${LIBS_VARIABLE}})

  # A module that reaches for something the toolchain does not find on its own,
  # such as a runtime that sits outside the default search paths, says so here
  # rather than through a library it links against
  get_target_property(LINK_OPTIONS ${TARGET_NAME} INTERFACE_LINK_OPTIONS)
  if(LINK_OPTIONS)
    foreach(OPTION IN LISTS LINK_OPTIONS)
      if(OPTION MATCHES "^(SHELL|LINKER):")
        message(FATAL_ERROR "The link option ${OPTION} of ${TARGET_NAME} is "
          "spelled in a way that only CMake knows how to expand. Write it as "
          "a single token instead, so that it survives into the packaging "
          "that consumers which do not read CMake rely on")
      endif()
      list(APPEND OPTIONS "${OPTION}")
    endforeach()
  endif()

  set(ENTRIES)
  get_target_property(TARGET_TYPE ${TARGET_NAME} TYPE)
  get_target_property(PUBLIC_LIBRARIES
    ${TARGET_NAME} INTERFACE_LINK_LIBRARIES)
  if(PUBLIC_LIBRARIES)
    list(APPEND ENTRIES ${PUBLIC_LIBRARIES})
  endif()

  # An interface library carries its entire dependency set in its link
  # interface, and has no private one to read
  if(NOT TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
    get_target_property(PRIVATE_LIBRARIES ${TARGET_NAME} LINK_LIBRARIES)
    if(PRIVATE_LIBRARIES)
      list(APPEND ENTRIES ${PRIVATE_LIBRARIES})
    endif()
  endif()

  foreach(LIBRARY IN LISTS ENTRIES)
    sourcemeta_pkgconfig_dependency("${LIBRARY}" MODULES LIBS)
  endforeach()

  set("${MODULES_VARIABLE}" ${MODULES} PARENT_SCOPE)
  set("${OPTIONS_VARIABLE}" ${OPTIONS} PARENT_SCOPE)
  set("${LIBS_VARIABLE}" ${LIBS} PARENT_SCOPE)
endfunction()

# Walk the whole graph below a module rather than pointing at the neighbours it
# happens to touch. Naming the neighbours reads better, but pkg-config expands
# such a chain once per path that reaches a module, and a linker handed the
# resulting hundreds of repeated archives either takes minutes or gives up
function(sourcemeta_pkgconfig_closure
    ROOTS MODULES_VARIABLE OPTIONS_VARIABLE LIBS_VARIABLE)
  set(PENDING ${ROOTS})
  set(VISITED)
  set(OPTIONS)
  set(LIBS)

  while(PENDING)
    list(POP_FRONT PENDING CURRENT)
    if(CURRENT IN_LIST VISITED)
      continue()
    endif()
    list(APPEND VISITED "${CURRENT}")
    set(DIRECT)
    sourcemeta_pkgconfig_direct("${CURRENT}" DIRECT OPTIONS LIBS)
    list(APPEND PENDING ${DIRECT})
  endwhile()

  # A module is declared after everything it depends on, as CMake could not
  # resolve the dependency otherwise, so walking the declarations backwards
  # gives a linker the order it needs
  get_property(DECLARED GLOBAL PROPERTY SOURCEMETA_PKGCONFIG_TARGETS)
  list(REVERSE DECLARED)
  set(MODULES)
  foreach(MODULE IN LISTS DECLARED)
    if(MODULE IN_LIST VISITED)
      list(APPEND MODULES "${MODULE}")
    endif()
  endforeach()

  list(LENGTH VISITED EXPECTED)
  list(LENGTH MODULES ACTUAL)
  if(NOT EXPECTED EQUAL ACTUAL)
    message(FATAL_ERROR "Some of the modules that ${ROOTS} depends on are not "
      "packaged, and consumers that do not read CMake would fail to link "
      "against them: ${VISITED}")
  endif()

  if(OPTIONS)
    list(REMOVE_DUPLICATES OPTIONS)
  endif()
  if(LIBS)
    list(REMOVE_DUPLICATES LIBS)
  endif()

  set("${MODULES_VARIABLE}" ${MODULES} PARENT_SCOPE)
  set("${OPTIONS_VARIABLE}" ${OPTIONS} PARENT_SCOPE)
  set("${LIBS_VARIABLE}" ${LIBS} PARENT_SCOPE)
endfunction()

function(sourcemeta_pkgconfig_flags TARGET_NAME OUTPUT_VARIABLE)
  get_target_property(TARGET_TYPE ${TARGET_NAME} TYPE)
  if(TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
    set("${OUTPUT_VARIABLE}" "" PARENT_SCOPE)
  else()
    get_target_property(OUTPUT_NAME ${TARGET_NAME} OUTPUT_NAME)
    if(NOT OUTPUT_NAME)
      set(OUTPUT_NAME "${TARGET_NAME}")
    endif()
    set("${OUTPUT_VARIABLE}" "-l${OUTPUT_NAME}" PARENT_SCOPE)
  endif()
endfunction()

function(sourcemeta_pkgconfig_file OUTPUT NAME DESCRIPTION LIBS PRIVATE_LIBS)
  # Anchoring at the location of the file itself, rather than at the prefix
  # that was configured, keeps the result correct for an installation that is
  # staged, relocated, or packaged somewhere other than where it was built
  if(IS_ABSOLUTE "${CMAKE_INSTALL_LIBDIR}")
    set(PREFIX "${CMAKE_INSTALL_PREFIX}")
  else()
    string(REGEX REPLACE "[^/]+" ".." PREFIX
      "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
    set(PREFIX "\${pcfiledir}/${PREFIX}")
  endif()

  string(CONCAT CONTENT
    "prefix=${PREFIX}\n"
    "exec_prefix=\${prefix}\n"
    "libdir=\${prefix}/${CMAKE_INSTALL_LIBDIR}\n"
    "includedir=\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}\n"
    "\n"
    "Name: ${NAME}\n"
    "Description: ${DESCRIPTION}\n"
    "Version: ${PROJECT_VERSION}\n"
    "Cflags: -I\${includedir}\n"
    "Libs: ${LIBS}\n"
    "Libs.private: ${PRIVATE_LIBS}\n")

  # Generated rather than written, as a link option is allowed to be a
  # generator expression and only CMake itself knows what it stands for
  file(GENERATE OUTPUT "${OUTPUT}" CONTENT "${CONTENT}")
endfunction()

function(sourcemeta_pkgconfig_write TARGET_NAME OUTPUT DESCRIPTION)
  sourcemeta_pkgconfig_closure("${TARGET_NAME}" MODULES OPTIONS LIBS)
  list(REMOVE_ITEM MODULES "${TARGET_NAME}")

  sourcemeta_pkgconfig_flags("${TARGET_NAME}" LIBRARY_FLAGS)
  if(LIBRARY_FLAGS)
    set(LIBRARY_FLAGS "-L\${libdir} ${LIBRARY_FLAGS}")
  endif()

  # Ahead of the libraries, as some of them are the search paths that the
  # libraries are then looked up in
  set(PRIVATE_FLAGS ${OPTIONS})
  foreach(MODULE IN LISTS MODULES)
    sourcemeta_pkgconfig_flags("${MODULE}" MODULE_FLAGS)
    if(MODULE_FLAGS)
      list(APPEND PRIVATE_FLAGS "${MODULE_FLAGS}")
    endif()
  endforeach()
  list(APPEND PRIVATE_FLAGS ${LIBS})
  list(JOIN PRIVATE_FLAGS " " PRIVATE_FLAGS)

  sourcemeta_pkgconfig_file("${OUTPUT}" "${TARGET_NAME}" "${DESCRIPTION}"
    "${LIBRARY_FLAGS}" "${PRIVATE_FLAGS}")
endfunction()

function(sourcemeta_pkgconfig_write_aggregate NAME OUTPUT DESCRIPTION)
  get_property(TARGETS GLOBAL PROPERTY SOURCEMETA_PKGCONFIG_TARGETS)
  sourcemeta_pkgconfig_closure("${TARGETS}" MODULES OPTIONS LIBS)

  set(FLAGS ${OPTIONS})
  list(APPEND FLAGS "-L\${libdir}")
  foreach(MODULE IN LISTS MODULES)
    sourcemeta_pkgconfig_flags("${MODULE}" MODULE_FLAGS)
    if(MODULE_FLAGS)
      list(APPEND FLAGS "${MODULE_FLAGS}")
    endif()
  endforeach()
  list(APPEND FLAGS ${LIBS})
  if(TARGET Mimalloc::Mimalloc)
    sourcemeta_pkgconfig_allocator(ALLOCATOR_FLAGS)
    list(APPEND FLAGS "${ALLOCATOR_FLAGS}")
  endif()
  list(JOIN FLAGS " " FLAGS)

  sourcemeta_pkgconfig_file("${OUTPUT}" "${NAME}" "${DESCRIPTION}" "${FLAGS}" "")
endfunction()

function(sourcemeta_pkgconfig_install)
  cmake_parse_arguments(SOURCEMETA_PKGCONFIG ""
    "TARGET;DESCRIPTION;COMPONENT" "" ${ARGN})

  set_property(GLOBAL APPEND PROPERTY SOURCEMETA_PKGCONFIG_TARGETS
    "${SOURCEMETA_PKGCONFIG_TARGET}")
  set(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${SOURCEMETA_PKGCONFIG_TARGET}.pc")

  # A module states some of its dependencies after asking to be installed, so
  # the link interface is only complete once the project has been walked. The
  # arguments of a deferred call are evaluated when the call finally runs, by
  # which point these variables are long gone, so bake them in here
  cmake_language(EVAL CODE
    "cmake_language(DEFER DIRECTORY \"${PROJECT_SOURCE_DIR}\"
       CALL sourcemeta_pkgconfig_write
       \"${SOURCEMETA_PKGCONFIG_TARGET}\" \"${OUTPUT}\"
       \"${SOURCEMETA_PKGCONFIG_DESCRIPTION}\")")

  include(GNUInstallDirs)
  install(FILES "${OUTPUT}"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
    COMPONENT ${SOURCEMETA_PKGCONFIG_COMPONENT})
endfunction()

# The whole library behind a single name, which is how a consumer that does not
# read CMake gets the allocator along with everything else
function(sourcemeta_pkgconfig_install_aggregate)
  cmake_parse_arguments(SOURCEMETA_PKGCONFIG ""
    "NAME;DESCRIPTION;COMPONENT" "" ${ARGN})

  set(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${SOURCEMETA_PKGCONFIG_NAME}.pc")
  cmake_language(EVAL CODE
    "cmake_language(DEFER DIRECTORY \"${PROJECT_SOURCE_DIR}\"
       CALL sourcemeta_pkgconfig_write_aggregate
       \"${SOURCEMETA_PKGCONFIG_NAME}\" \"${OUTPUT}\"
       \"${SOURCEMETA_PKGCONFIG_DESCRIPTION}\")")

  include(GNUInstallDirs)
  install(FILES "${OUTPUT}"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
    COMPONENT ${SOURCEMETA_PKGCONFIG_COMPONENT})

  if(TARGET Mimalloc::Mimalloc)
    sourcemeta_pkgconfig_allocator(ALLOCATOR_FLAGS)
    set(ALLOCATOR_OUTPUT
      "${CMAKE_CURRENT_BINARY_DIR}/${SOURCEMETA_PKGCONFIG_NAME}_mimalloc.pc")
    sourcemeta_pkgconfig_file("${ALLOCATOR_OUTPUT}"
      "${SOURCEMETA_PKGCONFIG_NAME}_mimalloc"
      "The bundled mimalloc allocator of ${SOURCEMETA_PKGCONFIG_DESCRIPTION}"
      "-L\${libdir} ${ALLOCATOR_FLAGS}" "")
    install(FILES "${ALLOCATOR_OUTPUT}"
      DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
      COMPONENT ${SOURCEMETA_PKGCONFIG_COMPONENT})
  endif()
endfunction()

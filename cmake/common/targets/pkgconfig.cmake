# Teach the packaging how to link against something that is not one of our own
# modules. A project states this next to wherever it brings the dependency in,
# so that adding one never means editing this file
function(sourcemeta_pkgconfig_declare)
  cmake_parse_arguments(SOURCEMETA_PKGCONFIG "" "TARGET" "LIBS" ${ARGN})
  set_property(GLOBAL PROPERTY
    "SOURCEMETA_PKGCONFIG_LIBS_${SOURCEMETA_PKGCONFIG_TARGET}"
    "${SOURCEMETA_PKGCONFIG_LIBS}")
endfunction()

# The order matters, as a module is declared after everything it depends on
function(sourcemeta_pkgconfig_register TARGET_NAME)
  set_property(GLOBAL APPEND PROPERTY
    SOURCEMETA_PKGCONFIG_TARGETS "${TARGET_NAME}")
endfunction()

function(sourcemeta_pkgconfig_dependency LIBRARY MODULES_VARIABLE LIBS_VARIABLE)
  set(MODULES ${${MODULES_VARIABLE}})
  set(LIBS ${${LIBS_VARIABLE}})

  # CMake keeps the private dependencies of a static library in its exported
  # link interface behind this expression, as they take part in linking but not
  # in compiling against the library
  string(REGEX REPLACE "^\\$<LINK_ONLY:(.+)>$" "\\1" LIBRARY "${LIBRARY}")

  get_property(DECLARED GLOBAL PROPERTY
    "SOURCEMETA_PKGCONFIG_LIBS_${LIBRARY}" SET)

  if(LIBRARY MATCHES "^sourcemeta::")
    string(REPLACE "::" "_" MODULE "${LIBRARY}")
    list(APPEND MODULES "${MODULE}")
  elseif(DECLARED)
    get_property(DECLARED_LIBS GLOBAL PROPERTY
      "SOURCEMETA_PKGCONFIG_LIBS_${LIBRARY}")
    list(APPEND LIBS ${DECLARED_LIBS})
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
      "pkg-config flag. Declare it with sourcemeta_pkgconfig_declare, so that "
      "consumers that do not read CMake keep linking against it")
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

function(sourcemeta_pkgconfig_write NAME OUTPUT DESCRIPTION)
  get_property(TARGETS GLOBAL PROPERTY SOURCEMETA_PKGCONFIG_TARGETS)
  sourcemeta_pkgconfig_closure("${TARGETS}" MODULES OPTIONS LIBS)

  # Ahead of the libraries, as some of them are the search paths that the
  # libraries are then looked up in
  set(FLAGS ${OPTIONS})
  list(APPEND FLAGS "-L\${libdir}")
  foreach(MODULE IN LISTS MODULES)
    get_target_property(TARGET_TYPE ${MODULE} TYPE)
    if(NOT TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
      get_target_property(OUTPUT_NAME ${MODULE} OUTPUT_NAME)
      if(NOT OUTPUT_NAME)
        set(OUTPUT_NAME "${MODULE}")
      endif()
      list(APPEND FLAGS "-l${OUTPUT_NAME}")
    endif()
  endforeach()
  list(APPEND FLAGS ${LIBS})
  list(JOIN FLAGS " " FLAGS)

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
    "Libs: ${FLAGS}\n")

  # Generated rather than written, as a link option is allowed to be a
  # generator expression and only CMake itself knows what it stands for
  file(GENERATE OUTPUT "${OUTPUT}" CONTENT "${CONTENT}")
endfunction()

# The whole library behind a single name, which is all a consumer that does not
# read CMake needs to know about how we are put together
function(sourcemeta_pkgconfig_install)
  cmake_parse_arguments(SOURCEMETA_PKGCONFIG ""
    "NAME;DESCRIPTION;COMPONENT" "" ${ARGN})

  set(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${SOURCEMETA_PKGCONFIG_NAME}.pc")

  # A module states some of its dependencies after asking to be installed, so
  # the link interface is only complete once the project has been walked. The
  # arguments of a deferred call are evaluated when the call finally runs, by
  # which point these variables are long gone, so bake them in here
  cmake_language(EVAL CODE
    "cmake_language(DEFER DIRECTORY \"${PROJECT_SOURCE_DIR}\"
       CALL sourcemeta_pkgconfig_write
       \"${SOURCEMETA_PKGCONFIG_NAME}\" \"${OUTPUT}\"
       \"${SOURCEMETA_PKGCONFIG_DESCRIPTION}\")")

  include(GNUInstallDirs)
  install(FILES "${OUTPUT}"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
    COMPONENT ${SOURCEMETA_PKGCONFIG_COMPONENT})
endfunction()

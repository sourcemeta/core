if(NOT Mimalloc_FOUND)
  set(MIMALLOC_DIR "${PROJECT_SOURCE_DIR}/vendor/mimalloc")

  set(MI_INSTALL_TOPLEVEL ON CACHE BOOL "" FORCE)
  set(MI_BUILD_SHARED ${BUILD_SHARED_LIBS} CACHE BOOL "" FORCE)
  if(BUILD_SHARED_LIBS)
    set(MI_BUILD_STATIC OFF CACHE BOOL "" FORCE)
    set(MIMALLOC_TARGET mimalloc)
  else()
    set(MI_BUILD_STATIC ON CACHE BOOL "" FORCE)
    set(MIMALLOC_TARGET mimalloc-static)
  endif()
  set(MI_BUILD_OBJECT OFF CACHE BOOL "" FORCE)
  set(MI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(MI_OVERRIDE ON CACHE BOOL "" FORCE)

  set(SOURCEMETA_CORE_MIMALLOC_SKIP_INSTALL "${MI_SKIP_INSTALL}")
  set(MI_SKIP_INSTALL ON)
  add_subdirectory(
    "${MIMALLOC_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}/mimalloc" EXCLUDE_FROM_ALL)
  set(MI_SKIP_INSTALL "${SOURCEMETA_CORE_MIMALLOC_SKIP_INSTALL}")
  unset(SOURCEMETA_CORE_MIMALLOC_SKIP_INSTALL)

  if(TARGET ${MIMALLOC_TARGET})
    set_target_properties(${MIMALLOC_TARGET}
      PROPERTIES COMPILE_WARNING_AS_ERROR OFF)
    set(Mimalloc_FOUND ON)

    if(SOURCEMETA_CORE_INSTALL)
      include(GNUInstallDirs)
      if(BUILD_SHARED_LIBS)
        install(TARGETS ${MIMALLOC_TARGET}
          EXPORT mimalloc
          RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
            COMPONENT sourcemeta_core
          LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT sourcemeta_core
            NAMELINK_COMPONENT sourcemeta_core_dev
          ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT sourcemeta_core_dev)
      else()
        install(TARGETS ${MIMALLOC_TARGET}
          EXPORT mimalloc
          ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            COMPONENT sourcemeta_core_dev)
      endif()
      install(EXPORT mimalloc
        FILE mimalloc.cmake
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/mimalloc"
        COMPONENT sourcemeta_core_dev)
      install(FILES
        "${MIMALLOC_DIR}/include/mimalloc.h"
        "${MIMALLOC_DIR}/include/mimalloc-new-delete.h"
        "${MIMALLOC_DIR}/include/mimalloc-override.h"
        "${MIMALLOC_DIR}/include/mimalloc-stats.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        COMPONENT sourcemeta_core_dev)
      install(FILES
        "${MIMALLOC_DIR}/cmake/mimalloc-config.cmake"
        "${MIMALLOC_DIR}/cmake/mimalloc-config-version.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/mimalloc"
        COMPONENT sourcemeta_core_dev)
    endif()
  endif()
endif()

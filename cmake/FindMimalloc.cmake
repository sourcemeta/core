if (NOT Mimalloc_FOUND)
set(MIMALLOC_DIR "${PROJECT_SOURCE_DIR}/vendor/mimalloc")

    set(MI_BUILD_SHARED OFF CACHE BOOL
        "" FORCE) set(MI_BUILD_OBJECT OFF CACHE BOOL
                      "" FORCE) set(MI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(MI_OVERRIDE ON CACHE BOOL "" FORCE)

            add_subdirectory(
                "${MIMALLOC_DIR}"
                "${CMAKE_CURRENT_BINARY_DIR}/mimalloc" EXCLUDE_FROM_ALL)

                if (TARGET mimalloc - static) set_target_properties(
                    mimalloc - static PROPERTIES COMPILE_WARNING_AS_ERROR OFF)
                    add_library(Mimalloc::Mimalloc ALIAS mimalloc - static)
                        endif()

                            set(Mimalloc_FOUND ON) endif()
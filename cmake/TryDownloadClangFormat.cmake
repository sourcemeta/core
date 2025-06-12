# TODO: Refactor this

# User-configurable version
set(CLANG_FORMAT_VERSION "20.1.6" CACHE STRING "clang-format/LLVM version to download")

# Compute download/extract dir and expected binary path
set(_cf_dir "${CMAKE_BINARY_DIR}")
if(WIN32)
  set(_cf_bin "${_cf_dir}/bin/clang-format.exe")
else()
  set(_cf_bin "${_cf_dir}/bin/clang-format")
endif()

if(NOT EXISTS "${_cf_bin}")
  # Determine asset name for this OS/ARCH
  set(_asset "")
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "amd64")
      set(_asset "clang+llvm-${CLANG_FORMAT_VERSION}-x86_64-linux-gnu-ubuntu-20.04.tar.xz")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
      set(_asset "clang+llvm-${CLANG_FORMAT_VERSION}-aarch64-linux-gnu-ubuntu-20.04.tar.xz")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7a")
      set(_asset "clang+llvm-${CLANG_FORMAT_VERSION}-armv7a-linux-gnueabihf.tar.gz")
    endif()
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
      set(_asset "clang+llvm-${CLANG_FORMAT_VERSION}-x86_64-apple-darwin.tar.xz")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
      set(_asset "clang+llvm-${CLANG_FORMAT_VERSION}-arm64-apple-darwin.tar.xz")
    endif()
  elseif(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(_asset "clang+llvm-${CLANG_FORMAT_VERSION}-x86_64-pc-windows-msvc.tar.xz")
    endif()
  endif()

  if(_asset)
    set(_url "https://github.com/llvm/llvm-project/releases/download/llvmorg-${CLANG_FORMAT_VERSION}/${_asset}")
    set(_archive "${CMAKE_BINARY_DIR}/${_asset}")
    message(STATUS "Downloading clang-format ${CLANG_FORMAT_VERSION} for ${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR} ...")
    file(DOWNLOAD
      "${_url}"
      "${_archive}"
      SHOW_PROGRESS
      STATUS _dl_status
      LOG _dl_log
      TLS_VERIFY ON
    )
    list(GET _dl_status 0 _dl_code)
    if(_dl_code EQUAL 0)
      # extract
      file(MAKE_DIRECTORY "${_cf_dir}")
      file(ARCHIVE_EXTRACT INPUT "${_archive}" DESTINATION "${_cf_dir}")
      # if single top-level folder, move contents up
      file(GLOB _children RELATIVE "${_cf_dir}" "${_cf_dir}/*")
      list(LENGTH _children _count)
      if(_count EQUAL 1)
        list(GET _children 0 _child)
        set(_inner "${_cf_dir}/${_child}")
        if(IS_DIRECTORY "${_inner}")
          file(GLOB_RECURSE _items RELATIVE "${_inner}" "${_inner}/*")
          foreach(_it IN LISTS _items)
            file(RENAME "${_inner}/${_it}" "${_cf_dir}/${_it}")
          endforeach()
          file(REMOVE_RECURSE "${_inner}")
        endif()
      endif()
      file(REMOVE "${_archive}")
      if(EXISTS "${_cf_bin}")
        message(STATUS "clang-format downloaded to ${_cf_bin}")
      else()
        message(STATUS "clang-format binary not found after extraction; skipping.")
      endif()
    else()
      message(STATUS "Failed to download clang-format from ${_url} (status=${_dl_code}); skipping download.")
    endif()
  else()
    message(STATUS "No prebuilt clang-format for ${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR}; skipping download.")
  endif()
else()
  message(STATUS "clang-format already present at ${_cf_bin}; skipping download.")
endif()

# You can later refer to _cf_bin (e.g. pass as HINT to find_program) or ignore if not downloaded.

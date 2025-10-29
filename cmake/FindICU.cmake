if(NOT ICU_FOUND)
  set(ICU_ROOT "/opt/homebrew/opt/icu4c")

  find_path(ICU_INCLUDE_DIR
    NAMES unicode/unistr.h
    PATHS ${ICU_ROOT}/include
    NO_DEFAULT_PATH)

  find_library(ICU_UC_LIBRARY
    NAMES icuuc
    PATHS ${ICU_ROOT}/lib
    NO_DEFAULT_PATH)

  find_library(ICU_I18N_LIBRARY
    NAMES icui18n
    PATHS ${ICU_ROOT}/lib
    NO_DEFAULT_PATH)

  find_library(ICU_DATA_LIBRARY
    NAMES icudata
    PATHS ${ICU_ROOT}/lib
    NO_DEFAULT_PATH)

  if(ICU_INCLUDE_DIR AND ICU_UC_LIBRARY AND ICU_I18N_LIBRARY AND ICU_DATA_LIBRARY)
    set(ICU_FOUND ON)
    set(ICU_INCLUDE_DIRS ${ICU_INCLUDE_DIR})
    set(ICU_LIBRARIES ${ICU_UC_LIBRARY} ${ICU_I18N_LIBRARY} ${ICU_DATA_LIBRARY})

    if(NOT TARGET ICU::uc)
      add_library(ICU::uc UNKNOWN IMPORTED)
      set_target_properties(ICU::uc PROPERTIES
        IMPORTED_LOCATION "${ICU_UC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ICU_INCLUDE_DIR}")
    endif()

    if(NOT TARGET ICU::i18n)
      add_library(ICU::i18n UNKNOWN IMPORTED)
      set_target_properties(ICU::i18n PROPERTIES
        IMPORTED_LOCATION "${ICU_I18N_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ICU_INCLUDE_DIR}")
    endif()

    if(NOT TARGET ICU::data)
      add_library(ICU::data UNKNOWN IMPORTED)
      set_target_properties(ICU::data PROPERTIES
        IMPORTED_LOCATION "${ICU_DATA_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ICU_INCLUDE_DIR}")
    endif()

    message(STATUS "ICU found")
  else()
    message(FATAL_ERROR "ICU not found. Please install ICU or set ICU_ROOT.")
  endif()
endif()

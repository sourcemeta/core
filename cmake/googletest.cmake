include(GoogleTest)
find_package(GTest)

if(NOT GTest_FOUND)
  # See https://packages.ubuntu.com/kinetic/all/googletest/filelist
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND EXISTS /usr/src/googletest/CMakeLists.txt)
    add_subdirectory(/usr/src/googletest "${CMAKE_CURRENT_BINARY_DIR}/vendor/googletest")
  else()
    message(FATAL_ERROR "Could NOT find GTest")
  endif()
endif()

if(NOT TARGET GTest::gtest)
  set(GTest::gtest gtest)
endif()
if(NOT TARGET GTest::gtest_main)
  set(GTest::gtest gtest_main)
endif()

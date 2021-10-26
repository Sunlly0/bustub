#----------------------------------------------------------------
# Generated CMake target import file for configuration "DEBUG..".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "GTest::gtest" for configuration "DEBUG.."
set_property(TARGET GTest::gtest APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG..)
set_target_properties(GTest::gtest PROPERTIES
  IMPORTED_IMPLIB_DEBUG.. "${_IMPORT_PREFIX}/lib/libgtest.dll.a"
  IMPORTED_LOCATION_DEBUG.. "${_IMPORT_PREFIX}/bin/libgtest.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS GTest::gtest )
list(APPEND _IMPORT_CHECK_FILES_FOR_GTest::gtest "${_IMPORT_PREFIX}/lib/libgtest.dll.a" "${_IMPORT_PREFIX}/bin/libgtest.dll" )

# Import target "GTest::gtest_main" for configuration "DEBUG.."
set_property(TARGET GTest::gtest_main APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG..)
set_target_properties(GTest::gtest_main PROPERTIES
  IMPORTED_IMPLIB_DEBUG.. "${_IMPORT_PREFIX}/lib/libgtest_main.dll.a"
  IMPORTED_LOCATION_DEBUG.. "${_IMPORT_PREFIX}/bin/libgtest_main.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS GTest::gtest_main )
list(APPEND _IMPORT_CHECK_FILES_FOR_GTest::gtest_main "${_IMPORT_PREFIX}/lib/libgtest_main.dll.a" "${_IMPORT_PREFIX}/bin/libgtest_main.dll" )

# Import target "GTest::gmock" for configuration "DEBUG.."
set_property(TARGET GTest::gmock APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG..)
set_target_properties(GTest::gmock PROPERTIES
  IMPORTED_IMPLIB_DEBUG.. "${_IMPORT_PREFIX}/lib/libgmock.dll.a"
  IMPORTED_LOCATION_DEBUG.. "${_IMPORT_PREFIX}/bin/libgmock.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS GTest::gmock )
list(APPEND _IMPORT_CHECK_FILES_FOR_GTest::gmock "${_IMPORT_PREFIX}/lib/libgmock.dll.a" "${_IMPORT_PREFIX}/bin/libgmock.dll" )

# Import target "GTest::gmock_main" for configuration "DEBUG.."
set_property(TARGET GTest::gmock_main APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG..)
set_target_properties(GTest::gmock_main PROPERTIES
  IMPORTED_IMPLIB_DEBUG.. "${_IMPORT_PREFIX}/lib/libgmock_main.dll.a"
  IMPORTED_LOCATION_DEBUG.. "${_IMPORT_PREFIX}/bin/libgmock_main.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS GTest::gmock_main )
list(APPEND _IMPORT_CHECK_FILES_FOR_GTest::gmock_main "${_IMPORT_PREFIX}/lib/libgmock_main.dll.a" "${_IMPORT_PREFIX}/bin/libgmock_main.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

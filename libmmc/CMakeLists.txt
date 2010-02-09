include_directories ("${PROJECT_SOURCE_DIR}/libmmc")

add_library (
   libmmc STATIC
   ${PROJECT_SOURCE_DIR}/libmmc/mmc_all.c
   ${PROJECT_SOURCE_DIR}/libmmc/mmc_linux.c
   ${PROJECT_SOURCE_DIR}/libmmc/mmc_macosx.c
   ${PROJECT_SOURCE_DIR}/libmmc/mmc_win32.c
)

set_target_properties(
   libmmc
   PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/build/bin
      LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/build/lib
      ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/build/lib
)

if (MSVC_IDE)
    set_target_properties (libmmc PROPERTIES PREFIX "../")
endif()
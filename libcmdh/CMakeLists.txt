include_directories ("${PROJECT_SOURCE_DIR}/libcmdh")

add_library (
   libcmdh STATIC
   ${PROJECT_SOURCE_DIR}/libcmdh/libcmdh.c
)

set_target_properties( 
   libcmdh
   PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/build/bin
      LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/build/lib
      ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/build/lib
)


if (MSVC_IDE)
    set_target_properties (libcmdh PROPERTIES PREFIX "../")
endif()
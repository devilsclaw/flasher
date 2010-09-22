add_definitions(-DCUI)

include_directories ("${PROJECT_BINARY_DIR}/flasher")
include_directories ("${PROJECT_SOURCE_DIR}/libmmc")
include_directories ("${PROJECT_SOURCE_DIR}/libcmdh")

IF(APPLE)
   #SET(GUI_TYPE MACOSX_BUNDLE)
   INCLUDE_DIRECTORIES ( /Developer/SDKs/MacOSX10.4u.sdk )
   FIND_LIBRARY(CORESERVICES_LIBRARY CoreServices)
   FIND_LIBRARY(IOKIT_LIBRARY IOKit )
   MARK_AS_ADVANCED (CORESERVICES_LIBRARY
                     IOKIT_LIBRARY)
   SET(EXTRA_LIBS ${CORESERVICES_LIBRARY} ${IOKIT_LIBRARY})
   add_definitions(-DMACOSX)
ENDIF (APPLE)


add_executable (
   flasher
   ${PROJECT_SOURCE_DIR}/flasher/cmds_all.c 
   ${PROJECT_SOURCE_DIR}/flasher/cui_all.c
   ${PROJECT_SOURCE_DIR}/flasher/gui_gtk_all.c
   ${PROJECT_SOURCE_DIR}/flasher/gui_gtk_win32.c 
   ${PROJECT_SOURCE_DIR}/flasher/lg_renesas.c 
   ${PROJECT_SOURCE_DIR}/flasher/misc.c
   ${PROJECT_SOURCE_DIR}/flasher/cmds_all.h
   ${PROJECT_SOURCE_DIR}/flasher/cui_all.h
   ${PROJECT_SOURCE_DIR}/flasher/gui_gtk_all.h
   ${PROJECT_SOURCE_DIR}/flasher/lg_renesas.h
   ${PROJECT_SOURCE_DIR}/flasher/misc.h

)

set_target_properties( 
   flasher
   PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/build/bin
      LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/build/lib
      ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/build/lib
)


if (MSVC_IDE)
    set_target_properties (flasher PROPERTIES PREFIX "../")
endif()

add_dependencies(flasher dcdate libmmc libcmdh)
target_link_libraries (flasher ${EXTRA_LIBS} libmmc libcmdh)

FOREACH(DIR ${INCDIRS})
  include_directories ("${PROJECT_SOURCE_DIR}/${DIR}")
ENDFOREACH()

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

set_target_properties(
   ${PRJNAME}
   PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../build/bin/${PRJNAME}
      LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../build/lib
      ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../build/lib
)

if(NOT LFLAGS)
 set(LFLAGS " ")
endif()

if(NOT CFLAGS)
 set(LFLAGS " ")
endif()

set_target_properties( ${PRJNAME} PROPERTIES PREFIX "../")

if(MSVC_IDE)
set(FLAGS)
FOREACH(flag ${CFLAGS})
 set(FLAGS "${FLAGS} ${flag}")
ENDFOREACH()
set_target_properties( ${PRJNAME} PROPERTIES COMPILE_FLAGS ${FLAGS})

set(FLAGS)
FOREACH(flag ${LFLAGS})
 set(FLAGS "${FLAGS} ${flag}")
ENDFOREACH()
set_target_properties( ${PRJNAME} PROPERTIES LINK_FLAGS ${FLAGS})

endif(MSVC_IDE)

IF(DEPS)
  add_dependencies(${PRJNAME} ${DEPS})
ENDIF()

if(LIBS OR DEPS)
target_link_libraries (${PRJNAME} ${EXTRA_LIBS} ${DEPS} ${LIBS})
endif()
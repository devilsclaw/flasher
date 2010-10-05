set(PRJNAME dcdate)
set(PRJDIR ${PROJECT_SOURCE_DIR}/${PRJNAME})

set(INCDIRS ${PRJNAME})
#set(DEPS)
set(SRCFILES dcdate.c)
#set(INCFILES " ")
#set(LIBS " ")
set(LFLAGS " ")
set(CFLAGS " ")

INCLUDE(prebuild)
#add_library(${PRJNAME} STATIC ${SRCS} ${INCS})
add_executable(${PRJNAME} ${SRCS} ${INCS})
#add_executable(${PRJNAME} WIN32 ${SRCS} ${INCS})
INCLUDE(build)

ADD_CUSTOM_COMMAND(
   TARGET ${PRJNAME}
   POST_BUILD
   COMMAND ${PROJECT_BINARY_DIR}/../build/bin/${PRJNAME}/${PRJNAME} ${PROJECT_BINARY_DIR}/flasher/version.h
)

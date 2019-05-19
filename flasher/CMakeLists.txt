add_definitions(-DCUI)

set(PRJNAME flasher)
set(PRJDIR ${PROJECT_SOURCE_DIR}/${PRJNAME})
include_directories(${PROJECT_BINARY_DIR}/${PRJNAME})

set(INCDIRS ${PRJNAME} libmmc libcmdh)
set(DEPS libmmc libcmdh)
set(SRCFILES cmds_all.c cui_all.c gui_gtk_all.c gui_gtk_win32.c lg_renesas.c misc.c)
set(INCFILES cmds_all.h cui_all.h gui_gtk_all.h lg_renesas.h /misc.h)
#set(LIBS "")
set(LFLAGS /MANIFESTUAC:NO /INCREMENTAL:NO)
set(CFLAGS "-Wall -Werror")

INCLUDE(prebuild)
#add_library(${PRJNAME} STATIC ${SRCS} ${INCS})
add_executable(${PRJNAME} ${SRCS} ${INCS})
#add_executable(${PRJNAME} WIN32 ${SRCS} ${INCS})
INCLUDE(build)

add_dependencies(${PRJNAME} dcdate)

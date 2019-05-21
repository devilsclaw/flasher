add_definitions(-DCUI)

set(PRJNAME flasher)
set(PRJDIR ${PROJECT_SOURCE_DIR}/${PRJNAME})
include_directories(${PROJECT_BINARY_DIR}/${PRJNAME})

set(SRCFILES libcmdh.c cmds_all.c cui_all.c lg_renesas.c misc.c mmc_all.c mmc_debug.c mmc_linux.c)
set(INCFILES libcmdh.h cmds_all.h cui_all.h lg_renesas.h misc.h libmmc.h)
#set(LIBS "")
set(LFLAGS /MANIFESTUAC:NO /INCREMENTAL:NO)
set(CFLAGS "-Wall -Werror")

INCLUDE(prebuild)
add_executable(${PRJNAME} ${SRCS} ${INCS})
INCLUDE(build)

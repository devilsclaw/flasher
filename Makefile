# Simple makefile -- cmake is overkill.

CC=		gcc
CFLAGS=		-Wall -Werror

OBJS=		libcmdh.o cmds_all.o cui_all.o lg_renesas.o misc.o mmc_all.o \
		mmc_debug.o mmc_linux.o

all:		lgflasher

lgflasher:	$(OBJS)
		gcc -o lgflasher $(OBJS)

clean:
		rm -f lgflasher *.o *~

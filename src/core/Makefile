MAKE=make -s
CC=gcc

all:
	@$(MAKE) core

core:
	@echo " CC   internal.c"
	@$(CC) `pkg-config --cflags glib-2.0` -I$(LOCINC) -c internal.c -o internal.o

clean:
	@echo " RM   internal.o"
	@rm -rf internal.o

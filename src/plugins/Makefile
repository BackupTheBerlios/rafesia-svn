MAKE=make -s
CC=gcc -g

all:
	@$(MAKE) plugins
	@cd rf-media-xine && $(MAKE)
	@cd rf-media-mplayer && $(MAKE)

plugins:
	@echo " CC   plugins.c"
	@$(CC) `pkg-config --cflags gmodule-2.0 gtk+-2.0` -I$(LOCINC) -c plugins.c -o plugins.o

clean:
	@echo " RM   plugins.o"
	@rm -rf plugins.o
	@cd rf-media-xine && $(MAKE) clean
	@cd rf-media-mplayer && $(MAKE) clean

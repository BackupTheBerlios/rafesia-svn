MAKE=make -s
CC=gcc

all:
	@$(MAKE) ui
ui:
	@echo " CC   rf-widget-mixer.c"
	@$(CC) `pkg-config --cflags gtk+-2.0` -I$(LOCINC) -c rf-widget-mixer.c -o rf-widget-mixer.o
	@echo " CC   rf-window-main.c"
	@$(CC) `pkg-config --cflags gtk+-2.0` -I$(LOCINC) -c rf-window-main.c -o rf-window-main.o
	@echo " CC   rf-window-about.c"
	@$(CC) `pkg-config --cflags gtk+-2.0` -I$(LOCINC) -c rf-window-about.c -o rf-window-about.o
#	@gcc `pkg-config --cflags gtk+-2.0` -I$(LOCINC) -c rf-window-about.c -o rf-window-about.o

clean:
	@echo " RM   rf-window-main.o"
	@echo " RM   rf-window-about.o"
	@echo " RM   rf-widget-mixer.o"
	@rm -rf rf-window-main.o rf-window-about.o rf-widget-mixer.o
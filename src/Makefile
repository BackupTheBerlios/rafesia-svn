EXPORT=LOCINC=../$(LOCINC)
MAKE=make -s
CC=gcc -g

all:
	@cd core && $(EXPORT) $(MAKE)
	@cd ui && $(EXPORT) $(MAKE)
	@cd plugins && $(EXPORT) $(MAKE)
	@make -s rafesia

rafesia:
	@echo " CC   rafesia.c"
	@$(CC) `pkg-config --cflags gtk+-2.0 libgnomeui-2.0` -I$(LOCINC) -c rafesia.c -o rafesia.o
	@echo " LN   rafesia"
	@$(CC) `pkg-config --cflags --libs gtk+-2.0 gthread-2.0 libgnomeui-2.0` ./core/internal.o ./ui/rf-widget-mixer.o ./ui/rf-window-main.o ./plugins/plugins.c ./ui/rf-window-about.o  rafesia.o -o ../rafesia
#	@gcc `pkg-config --cflags gtk+-2.0` -I$(LOCINC) -c main.c -o main.o
#	@gcc `pkg-config --cflags --libs gtk+-2.0 gthread-2.0` ./core/internal.o ./ui/interface.o ./plugins/plugins.c ./ui/rf-window-about.o  main.o -o ../rafesia

clean:
	@cd core && $(EXPORT) $(MAKE) clean
	@cd ui && $(EXPORT) $(MAKE) clean
	@cd plugins && $(EXPORT) $(MAKE) clean
	@echo " RM   rafesia.o"
	@echo " RM   rafesia"
	@rm -rf ../rafesia rafesia.o

CC=gcc -g
CFLAGS=
INC=-I$(LOCINC)

all:
	@echo " CC   (P) rf-media-mplayer | rf-media-mplayer.c"
	@$(CC) `pkg-config --cflags gtk+-2.0` $(CFLAGS) -c -fPIC rf-media-mplayer.c -o rf-media-mplayer.o
	@echo " CC   (P) rf-media-mplayer | rf-media-module-funcs.c"
	@$(CC) `pkg-config --cflags gtk+-2.0` $(CFLAGS) -c -fPIC rf-media-module-funcs.c -o rf-media-module-funcs.o
	@echo " LN   (P) rf-media-mplayer | rf-media-mplayer.so"
	@$(CC) -shared -fPIC `pkg-config --cflags --libs gtk+-2.0` $(CFLAGS) -o rf-media-mplayer.so rf-media-mplayer.o rf-media-module-funcs.o
	@mv ./rf-media-mplayer.so ../../../plugins/

clean:
	@echo " RM   (P) rf-media-mplayer | rf-media-mplayer.so"
	@echo " RM   (P) rf-media-mplayer | rf-media-mplayer.o"
	@echo " RM   (P) rf-media-mplayer | rf-media-module-funcs.o"
	@rm ../../../plugins/rf-media-mplayer.so rf-media-mplayer.o rf-media-module-funcs.o

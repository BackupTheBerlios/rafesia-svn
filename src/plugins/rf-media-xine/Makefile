CC=gcc -g
CFLAGS=
INC=-I$(LOCINC)

all:
	@echo " CC   (P) rf-media-xine | rf-media-xine.c"
	@$(CC) `pkg-config --cflags gtk+-2.0` `xine-config --cflags` $(CFLAGS) -c -fPIC rf-media-xine.c -o rf-media-xine.o
	@echo " CC   (P) rf-media-xine | rf-media-module-funcs.c"
	@$(CC) `pkg-config --cflags gtk+-2.0` `xine-config --cflags` $(CFLAGS) -c -fPIC rf-media-module-funcs.c -o rf-media-module-funcs.o
	@echo " LN   (P) rf-media-xine | rf-media-xine.so"
	@$(CC) -shared -fPIC `xine-config --cflags --libs --cflags` `pkg-config --cflags --libs gtk+-2.0` $(CFLAGS) -o rf-media-xine.so rf-media-xine.o rf-media-module-funcs.o
	@mv ./rf-media-xine.so ../../../plugins/

clean:
	@echo " RM   (P) rf-media-xine | rf-media-xine.so"
	@echo " RM   (P) rf-media-xine | rf-media-xine.o"
	@echo " RM   (P) rf-media-xine | rf-media-module-funcs.o"
	@rm ../../../plugins/rf-media-xine.so rf-media-xine.o rf-media-module-funcs.o

CC=gcc 
CFLAGS=
INC=-I$(LOCINC)

all:
	@echo " CC   (P) xine | gtkxine.c"
	@$(CC) `pkg-config --cflags gtk+-2.0` `xine-config --cflags` $(CFLAGS) -c -fPIC gtkxine.c -o gtkxine.o
	@echo " CC   (P) xine | xine_media_module.c"
	@$(CC) `pkg-config --cflags gtk+-2.0` `xine-config --cflags` $(CFLAGS) -c -fPIC xine_media_module.c -o xine_media_module.o
	@echo " LN   (P) xine | xine.so"
	@$(CC) -shared -fPIC `xine-config --cflags --libs --cflags` `pkg-config --cflags --libs gtk+-2.0` -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 -lXext -lXtst -lm $(CFLAGS) -o xine.so gtkxine.o xine_media_module.o
	@mv ./xine.so ../../../plugins/

clean:
	@echo " RM   (P) xine | xine.so"
	@echo " RM   (P) xine | gtkxine.o"
	@echo " RM   (P) xine | xine_media_module.o"
	@rm ../../../plugins/xine.so gtkxine.o xine_media_module.o

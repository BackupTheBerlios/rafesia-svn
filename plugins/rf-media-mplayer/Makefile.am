#pkgdir = $(libdir)/rafesia

#pkg_LTLIBRARIES = mplayer.la

#mplayer_la_CFLAGS = $(GTK_CFLAGS) -I$(top_builddir)/include

#mplayer_la_LDFLAGS = -module -avoid-version

#mplayer_la_SOURCES = \
#	rf-media-mplayer.c\
#	rf-media-mplayer.h\
#	rf-media-module-funcs.c

all:
	$(CC) $(GTK_CFLAGS) -I$(top_builddir)/include -fPIC -c rf-media-mplayer.c -o rf-media-mplayer.o
	$(CC) $(GTK_CFLAGS) -I$(top_builddir)/include -fPIC -c rf-media-module-funcs.c -o rf-media-module-funcs.o
	$(CC) $(GTK_CFLAGS) $(GTK_LIBS) -I$(top_builddir)/include -shared -fPIC rf-media-mplayer.o rf-media-module-funcs.o -o mplayer.so

install:
	$(INSTALL) -d $(libdir)/rafesia/
	$(INSTALL) ./mplayer.so $(libdir)/rafesia/

clean:
	rm rf-media-mplayer.o rf-media-module-funcs.o mplayer.so

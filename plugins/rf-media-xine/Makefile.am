#pkgdir = $(libdir)/rafesia

#pkg_LTLIBRARIES = xine.la

#xine_la_CFLAGS = $(GTK_CFLAGS) $(XINE_CFLAGS) -I$(top_builddir)/include $(XINE_LIBS)

#xine_la_LDFLAGS = -module -avoid-version

#xine_la_LIBS = $(XINE_LIBS) $(GTK_LIBS)

#xine_la_SOURCES = \
#	rf-media-xine.c\
#	rf-media-xine.h\
#	rf-media-module-funcs.c

all:
	$(CC) $(GTK_CFLAGS) $(XINE_CFLAGS) -I$(top_builddir)/include -fPIC -c rf-media-xine.c -o rf-media-xine.o
	$(CC) $(GTK_CFLAGS) $(XINE_CFLAGS) -I$(top_builddir)/include -fPIC -c rf-media-module-funcs.c -o rf-media-module-funcs.o
	$(CC) $(GTK_CFLAGS) $(XINE_CFLAGS) $(XINE_LIBS) $(GTK_LIBS) -I$(top_builddir)/include -shared -fPIC rf-media-xine.o rf-media-module-funcs.o -o xine.so

install:
	$(INSTALL) -d $(libdir)/rafesia/
	$(INSTALL) ./xine.so $(libdir)/rafesia/

clean:
	rm rf-media-xine.o rf-media-module-funcs.o xine.so

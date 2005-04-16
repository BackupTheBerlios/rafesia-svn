INCLUDES = \
	$(GTK_CFLAGS)\
	-I$(top_builddir)/include\
	-DRF_LIBDIR="\"$(pkglibdir)\""

AM_CFLAGS =\
	 -g

bin_PROGRAMS = rafesia

rafesia_SOURCES = \
	core/internal.c\
	core/internal.h\
	ui/rf-widget-mixer.c\
	ui/rf-widget-mixer.h\
	ui/rf-window-about.c\
	ui/rf-window-main.c\
	ui/rf-window-main.h\
	plugins.c\
	rafesia.c\
	rafesia.h

rafesia_LDFLAGS = 

rafesia_LDADD =  \
	$(GTK_LIBS)

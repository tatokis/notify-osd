NULL =

transform = s/_/-/g

INCLUDES =			\
	-I.			\
	-I$(srcdir)		\
	-I$(top_srcdir)		\
	$(NULL)

notify_osddir = $(libexecdir)

notify_osd_PROGRAMS = \
	notify-osd		\
	$(NULL)

notify_osd_sources =		\
	bubble.c		\
	defaults.c		\
	dialog.c		\
	main.c			\
	observer.c		\
	stack.c			\
	dbus.c			\
	dnd.c			\
	apport.c		\
	log.c			\
	util.c			\
	stack-blur.c				\
	exponential-blur.c			\
	gaussian-blur.c				\
	raico-blur.c				\
	tile.c					\
	bubble-window.c				\
	bubble-window-accessible.c		\
	bubble-window-accessible-factory.c	\
	$(top_srcdir)/egg/egg-fixed.c		\
	$(top_srcdir)/egg/egg-units.c		\
	$(top_srcdir)/egg/egg-timeline.c	\
	$(top_srcdir)/egg/egg-timeout-pool.c	\
	$(top_srcdir)/egg/egg-alpha.c		\
	$(top_srcdir)/egg/egg-hack.c		\
	$(NULL)

notify_osd_headers =				\
	bubble.h				\
	defaults.h				\
	dialog.h				\
	observer.h				\
	stack.h					\
	stack-glue.h				\
	dbus.h					\
	dnd.h					\
	log.h					\
	apport.h				\
	util.h					\
	stack-blur.h				\
	exponential-blur.h			\
	gaussian-blur.h				\
	raico-blur.h				\
	tile.h					\
	bubble-window.h				\
	bubble-window-accessible.h		\
	bubble-window-accessible-factory.h	\
	$(top_srcdir)/egg/egg-fixed.h		\
	$(top_srcdir)/egg/egg-units.h		\
	$(top_srcdir)/egg/egg-timeline.h	\
	$(top_srcdir)/egg/egg-timeout-pool.h	\
	$(top_srcdir)/egg/egg-alpha.h		\
	$(top_srcdir)/egg/egg-debug.h		\
	$(top_srcdir)/egg/egg-hack.h		\
	$(NULL)

notify_osd_SOURCES =		\
	$(notify_osd_sources)	\
	$(notify_osd_headers)	\
	$(NULL)

notify_osd_LDADD =			\
	$(LIBM)			\
	$(X_LIBS)		\
	$(GLIB_LIBS) 		\
	$(GTK_LIBS) 		\
	$(NOTIFY_OSD_LIBS)	\
	$(DBUS_LIBS)     	\
	$(WNCK_LIBS)     	\
	-lm			\
	$(NULL)

notify_osd_CFLAGS = 		\
	-DDATADIR=\""$(datadir)"\" \
	$(GTK_CFLAGS) 		\
	$(NOTIFY_OSD_CFLAGS)	\
	$(GLIB_CFLAGS) 		\
	$(DBUS_CFLAGS)		\
	-DWNCK_I_KNOW_THIS_IS_UNSTABLE \
	$(WNCK_CFLAGS)		\
	$(INCLUDES)		\
	$(NULL)

notify_osd_LDFLAGS = 		\
	-Xlinker -lm -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions \
	$(NULL)

# this comes from distutils.sysconfig.get_config_var('LINKFORSHARED')

stack-glue.h: notify-osd.xml Makefile
	$(LIBTOOL) --mode=execute $(DBUS_GLIB_BIN)/dbus-binding-tool --prefix=stack --mode=glib-server --output=$@ $<

BUILT_SOURCES =		\
	stack-glue.h

EXTRA_DIST =		\
	notify-osd.xml	\
	dialog.c	\
	display.c	\
	$(NULL)

glib_enum_headers = defaults.h

enum-types.h: $(glib_enum_headers) enum-types.h.in
	$(AM_V_GEN)$(GLIB_MKENUMS) \
	--template=enum-types.h.in \
	--output=$@ \
	$(glib_enum_headers)

enum-types.c: $(glib_enum_headers) enum-types.c.in enum-types.h
	$(AM_V_GEN)$(GLIB_MKENUMS) \
	--template=enum-types.c.in \
	--output=$@ \
	$(glib_enum_headers)

# Build the enum types files before every other target
BUILT_SOURCES += enum-types.h enum-types.c
CLEANFILES = enum-types.h enum-types.c
EXTRA_DIST += enum-types.h.in enum-types.c.in
notify_osd_SOURCES += enum-types.h enum-types.c

dist-hook:
	cd $(distdir) ; rm -f $(CLEANFILES)

DISTCLEANFILES = $(BUILT_SOURCES)


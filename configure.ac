AC_INIT(notify-osd, 0.9.34, dx-team@canonical.com)

AC_CONFIG_SRCDIR(src/main.c)
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
GNOME_COMMON_INIT
GNOME_COMPILE_WARNINGS(maximum)

AM_MAINTAINER_MODE
AM_DISABLE_STATIC
AM_PROG_LIBTOOL
AC_SUBST(ACLOCAL_AMFLAGS, "$ACLOCAL_FLAGS -I m4")
AC_PROG_CC
AC_PROG_CC_C99
AM_PROG_CC_C_O
AC_PROG_INSTALL

#
# pkg-config
#
PKG_PROG_PKG_CONFIG

#
# use all the GSettings-magic
#
GLIB_GSETTINGS

#
# glib, we need 2.16.0 for the unit-tests
#
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.36.0 gthread-2.0 gio-2.0])

#
# libwnck used by the dnd code
#
PKG_CHECK_MODULES([WNCK], [libwnck-3.0])
AC_CHECK_LIB(wnck-3,wnck_shutdown, have_wnck_shutdown=yes,)
if test "$have_wnck_shutdown" = "yes"; then
  AC_DEFINE(HAVE_WNCK_SHUTDOWN, 1, [Define to 1 if libwnck provides wnck_shutdown()])
fi


#
# libnotify, used unit-tests 
#
PKG_CHECK_MODULES([LIBNOTIFY], [libnotify >= 0.4.5])

#
# dbus
#
PKG_CHECK_MODULES([DBUS], [dbus-glib-1 >= 0.76])
DBUS_GLIB_BIN="`$PKG_CONFIG --variable=exec_prefix dbus-glib-1`/bin"
AC_SUBST(DBUS_GLIB_BIN)

#
# libX11
#

#
# Gnome/GTK checks
#

PKG_CHECK_MODULES([X], [x11])
AC_SUBST(X_CFLAGS)
AC_SUBST(X_LIBS)

AM_PATH_GTK_3_0

PKG_CHECK_MODULES([NOTIFY_OSD], [pixman-1 gtk+-3.0 >= 3.8.0])
AC_SUBST(NOTIFY_OSD_CFLAGS)
AC_SUBST(NOTIFY_OSD_LIBS)

#
# libm
#

AC_CHECK_LIBM
AC_SUBST(LIBM)

#
# glib-mkenums
#

PKG_CHECK_VAR([GLIB_MKENUMS], [glib-2.0], [glib_mkenums])

#
# checks for building C- or C#-examples
#
AC_DEFUN([CHECK_FOR_MONO_STUFF],
[
        NAME="gmcs"
        AC_PATH_PROG(GMCS, $NAME, no)
        AC_SUBST(GMCS)
        if test "x$GMCS" = "xno"; then
                AC_MSG_ERROR([You need to install '$NAME'])
        fi

        NAME="Mono.Posix"
        AC_MSG_CHECKING([for Mono 2.0 GAC $NAME.dll])
        if test -e "$(pkg-config --variable=libdir mono)/mono/2.0/$NAME.dll";
        then
                AC_MSG_RESULT([found])
        else
                AC_MSG_RESULT([not found])
                AC_MSG_ERROR([missing reqired Mono 2.0 assembly $NAME.dll])
        fi

        NAME="notify-sharp"
        AC_MSG_CHECKING([for Mono 2.0 CIL $NAME])
        pkg-config --exists $NAME
        if test "$?" == "0"
        then
                AC_MSG_RESULT([found])
        else
                AC_MSG_RESULT([not found])
                AC_MSG_ERROR([missing reqired Mono 2.0 CIL $NAME])
        fi
])

AC_ARG_WITH(examples, [  --with-examples=[[all/c/mono]]
                          build C or C#-examples using libnotify
                          demonstrating special features of notify-osd
                          there are Python-examples too, but they don't
                          need to be build])

c_examples=no
csharp_examples=no
if test "x$with_examples" = "xc"; then
        c_examples=yes
        AC_MSG_NOTICE([Build with ANSI-C examples])
fi
if test "x$with_examples" = "xmono"; then
        CHECK_FOR_MONO_STUFF
        csharp_examples=yes
        AC_MSG_NOTICE([Build with C-sharp examples])
fi
if test "x$with_examples" = "xall"; then
        CHECK_FOR_MONO_STUFF
        c_examples=yes
        csharp_examples=yes
        AC_MSG_NOTICE([Build with ANSI-C examples])
        AC_MSG_NOTICE([Build with C-sharp examples])
fi

AM_CONDITIONAL(BUILD_C_EXAMPLES, test "x$c_examples" != "xno")
AM_CONDITIONAL(BUILD_MONO_EXAMPLES, test "x$csharp_examples" != "xno")

dnl CFLAGS
CFLAGS="$CFLAGS -Wall"

AC_ARG_ENABLE([deprecations],
  [AS_HELP_STRING([--enable-deprecations],
    [allow deprecated API usage @<:@default=yes@:>@])],
  [],
  [enable_deprecations=yes])
AS_IF([test "x$enable_deprecations" = xno],
  [CFLAGS="$CFLAGS -DG_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE"]
)

AC_OUTPUT([
Makefile
src/Makefile
tests/Makefile
examples/Makefile
data/Makefile
data/icons/Makefile
data/icons/scalable/Makefile
])

echo ""
echo "                 Notify OSD $VERSION"
echo "             ============================"
echo ""
echo "                   prefix:   ${prefix}"
echo ""
echo "         Build C Examples:   ${c_examples}"
echo ""
echo "        Build C# Examples:   ${csharp_examples}"
echo ""

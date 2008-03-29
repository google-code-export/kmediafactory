# Configure paths for libogg
# Andreas Heinchen <andreas.heinchen@gmx.de> 04-18-2003
# Shamelessly adapted from Jack Moffitt's version for libvorbis
# who had stolen it from Owen Taylor and Manish Singh

dnl AM_PATH_OGG([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl -----------------------------------------------------------------
dnl AC_LIB_OGG([ACTION-IF-TRUE], [ACTION-IF-FALSE])
dnl -----------------------------------------------------------------

OPTION="  --with-libogg=DIR   prefix for ogg library files and headers"

AC_DEFUN([AC_LIB_OGG], [
  AH_TEMPLATE([HAVE_LIBOGG], [Define if ogg is available])

  AC_FIND_LIB("libogg", libpath, libsuff)

  AC_ARG_WITH(libogg, [$OPTION], [
    if test "$withval" = "no"; then
      ac_ogg_path=
      $2
    elif test "$withval" = "yes"; then
      ac_ogg_path=$libpath
    else
      ac_ogg_path="$withval"
    fi
  ],[ac_ogg_path=$libpath])

  if test "$ac_theora_path" = "$lib_path"; then
    ac_libsuff=$libsuff
  else
    ac_libsuff=$kdelibsuff
  fi

  if test "$ac_ogg_path" != ""; then
    saved_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS -I$ac_ogg_path/include"

    AC_CHECK_HEADERS(ogg/ogg.h,, OGG=no)
    AC_CHECK_HEADERS(ogg/ogg.h,, OGG=no)
    if test "$OGG" != "no"; then
      saved_LDFLAGS="$LDFLAGS"
      LDFLAGS="$LDFLAGS -L$ac_ogg_path/lib$ac_libsuff"
      AC_CHECK_LIB(ogg, ogg_stream_init, [
        AC_SUBST(OGG_CPPFLAGS, [-I$ac_ogg_path/include])
        AC_SUBST(OGG_LDFLAGS, [-L$ac_ogg_path/lib$ac_libsuff])
        AC_SUBST(OGG_LIBS, ["-logg -logg"])
        AC_DEFINE([HAVE_LIBOGG])
        ac_have_libogg="yes"
        $1
      ], [
  :
        $2
      ], -logg)
      LDFLAGS="$saved_LDFLAGS"
    else
  :
      $2
    fi
    CPPFLAGS="$saved_CPPFLAGS"
  fi
  AM_CONDITIONAL(HAVE_LIBOGG, test x$ac_have_libogg = "xyes")
])

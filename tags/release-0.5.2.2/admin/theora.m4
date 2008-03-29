# Configure paths for libtheora
# Andreas Heinchen <andreas.heinchen@gmx.de> 04-18-2003
# Shamelessly adapted from Jack Moffitt's version for libvorbis
# who had stolen it from Owen Taylor and Manish Singh

dnl AM_PATH_THEORA([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl -----------------------------------------------------------------
dnl AC_LIB_THEORA([ACTION-IF-TRUE], [ACTION-IF-FALSE])
dnl -----------------------------------------------------------------

OPTION="  --with-libtheora=DIR   prefix for theora library files and headers"

AC_DEFUN([AC_LIB_THEORA], [
  AH_TEMPLATE([HAVE_LIBTHEORA], [Define if theora is available])

  AC_FIND_LIB("libtheora", libpath, libsuff)

  AC_ARG_WITH(libtheora, [$OPTION], [
    if test "$withval" = "no"; then
      ac_theora_path=
      $2
    elif test "$withval" = "yes"; then
      ac_theora_path=$libpath
    else
      ac_theora_path="$withval"
    fi
  ],[ac_theora_path=$libpath])

  if test "$ac_theora_path" = "$lib_path"; then
    ac_libsuff=$libsuff
  else
    ac_libsuff=$kdelibsuff
  fi

  if test "$ac_theora_path" != ""; then
    saved_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS -I$ac_theora_path/include"

    AC_CHECK_HEADERS(ogg/ogg.h,, THEORA=no)
    AC_CHECK_HEADERS(theora/theora.h,, THEORA=no)
    if test "$THEORA" != "no"; then
      saved_LDFLAGS="$LDFLAGS"
      LDFLAGS="$LDFLAGS -L$ac_theora_path/lib$ac_libsuff"
      AC_CHECK_LIB(theora, theora_decode_init, [
        AC_SUBST(THEORA_CPPFLAGS, [-I$ac_theora_path/include])
        AC_SUBST(THEORA_LDFLAGS, [-L$ac_theora_path/lib$ac_libsuff])
        AC_SUBST(THEORA_LIBS, ["-ltheora -logg"])
        AC_DEFINE([HAVE_LIBTHEORA])
        ac_have_libtheora="yes"
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
  AM_CONDITIONAL(HAVE_LIBTHEORA, test x$ac_have_libtheora = "xyes")
])

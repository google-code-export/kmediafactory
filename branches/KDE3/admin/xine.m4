dnl -----------------------------------------------------------------
dnl AC_LIB_XINE([ACTION-IF-TRUE], [ACTION-IF-FALSE])
dnl -----------------------------------------------------------------

OPTION="  --with-libxine=DIR   prefix for xine library files and headers"

AC_DEFUN([AC_LIB_XINE], [
  AH_TEMPLATE([HAVE_LIBXINE], [Define if libxine is available])

  AC_FIND_LIB("libxine", libpath, libsuff)

  AC_ARG_WITH(libxine, [$OPTION], [
    if test "$withval" = "no"; then
      ac_xine_path=
      $2
    elif test "$withval" = "yes"; then
      ac_xine_path=$libpath
    else
      ac_xine_path="$withval"
    fi
  ],[ac_xine_path=$libpath])

  if test "$ac_theora_path" = "$lib_path"; then
    ac_libsuff=$libsuff
  else
    ac_libsuff=$kdelibsuff
  fi

  if test "$ac_xine_path" != ""; then
    saved_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS -I$ac_xine_path/include"
    AC_CHECK_HEADER([xine.h], [
      saved_LDFLAGS="$LDFLAGS"
      LDFLAGS="$LDFLAGS -L$ac_xine_path/lib$ac_libsuff"
      AC_CHECK_LIB(xine, xine_get_version_string, [
        AC_SUBST(XINE_CPPFLAGS, [-I$ac_xine_path/include])
        AC_SUBST(XINE_LDFLAGS, [-L$ac_xine_path/lib$ac_libsuff])
        AC_SUBST(XINE_LIBS, [-lxine])
        AC_DEFINE([HAVE_LIBXINE])
        ac_have_libxine="yes"
      $1
      ], [
  :
        $2
      ])
      LDFLAGS="$saved_LDFLAGS"
    ], [
  :
      $2
    ])
    CPPFLAGS="$saved_CPPFLAGS"
  fi
  AM_CONDITIONAL(HAVE_LIBXINE, test x$ac_have_libxine = "xyes")
])
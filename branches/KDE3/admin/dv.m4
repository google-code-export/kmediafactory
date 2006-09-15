dnl -----------------------------------------------------------------
dnl AC_LIB_DV([ACTION-IF-TRUE], [ACTION-IF-FALSE])
dnl -----------------------------------------------------------------

OPTION="  --with-libdv=DIR   prefix for dv library files and headers"

AC_DEFUN([AC_LIB_DV], [
  AH_TEMPLATE([HAVE_LIBDV], [Define if dv is available])

  AC_FIND_LIB("libdv", libpath, libsuff)

  AC_ARG_WITH(libdv, [$OPTION], [
    if test "$withval" = "no"; then
      ac_dv_path=
      $2
    elif test "$withval" = "yes"; then
      ac_dv_path=$libpath
    else
      ac_dv_path="$withval"
    fi
  ],[ac_dv_path=$libpath])

  if test "$ac_theora_path" = "$lib_path"; then
    ac_libsuff=$libsuff
  else
    ac_libsuff=$kdelibsuff
  fi

  if test "$ac_dv_path" != ""; then
    saved_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS -I$ac_dv_path/include"
    AC_CHECK_HEADER([libdv/dv.h], [
      saved_LDFLAGS="$LDFLAGS"
      LDFLAGS="$LDFLAGS -L$ac_dv_path/lib$ac_libsuff"
      AC_CHECK_LIB(dv, dv_decoder_new, [
        AC_SUBST(DV_CPPFLAGS, [-I$ac_dv_path/include])
        AC_SUBST(DV_LDFLAGS, [-L$ac_dv_path/lib$ac_libsuff])
        AC_SUBST(DV_LIBS, [-ldv])
        AC_DEFINE([HAVE_LIBDV])
        ac_have_libdv="yes"
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
  AM_CONDITIONAL(HAVE_LIBDV, test "$ac_have_libdv" = "yes")
])
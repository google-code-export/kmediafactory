dnl -----------------------------------------------------------------
dnl AC_LIB_DVDREAD([ACTION-IF-TRUE], [ACTION-IF-FALSE])
dnl -----------------------------------------------------------------

OPTION="  --with-libdvdread=DIR   prefix for dvdread library files and headers"

AC_DEFUN([AC_LIB_DVDREAD], [
  AH_TEMPLATE([HAVE_LIBDVDREAD], [Define if libdvdread is available])

  AC_FIND_LIB("libdvdread", libpath, libsuff)

  AC_ARG_WITH(libdvdread, [$OPTION], [
    if test "$withval" = "no"; then
      ac_dvdread_path=
      $2
    elif test "$withval" = "yes"; then
      ac_dvdread_path=$libpath
    else
      ac_dvdread_path="$withval"
    fi
  ],[ac_dvdread_path=$libpath])

  if test "$ac_theora_path" = "$lib_path"; then
    ac_libsuff=$libsuff
  else
    ac_libsuff=$kdelibsuff
  fi

  if test "$ac_dvdread_path" != ""; then
    saved_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS -I$ac_dvdread_path/include"
    AC_CHECK_HEADER([dvdread/dvd_reader.h], [
      saved_LDFLAGS="$LDFLAGS"
      LDFLAGS="$LDFLAGS -L$ac_dvdread_path/lib$ac_libsuff"
      AC_CHECK_LIB(dvdread, DVDOpen, [
        AC_SUBST(DVDREAD_CPPFLAGS, [-I$ac_dvdread_path/include])
        AC_SUBST(DVDREAD_LDFLAGS, [-L$ac_dvdread_path/lib$ac_libsuff])
        AC_SUBST(DVDREAD_LIBS, [-ldvdread])
        AC_DEFINE([HAVE_LIBDVDREAD])
        ac_have_libdvdread="yes"
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
  AM_CONDITIONAL(HAVE_LIBDVDREAD, test x$ac_have_libdvdread = "xyes")
])
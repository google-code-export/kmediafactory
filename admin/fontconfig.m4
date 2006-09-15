dnl -----------------------------------------------------------------
dnl AC_LIB_FONTCONFIG([ACTION-IF-TRUE], [ACTION-IF-FALSE])
dnl -----------------------------------------------------------------

OPTION="  --with-libfontconfig=DIR   prefix for fontconfig library files and
headers"

AC_DEFUN([AC_LIB_FONTCONFIG], [
  AH_TEMPLATE([HAVE_LIBFONTCONFIG], [Define if fontconfig is available])

  AC_FIND_LIB("libfontconfig", libpath, libsuff)

  AC_ARG_WITH(libfontconfig, [$OPTION], [
    if test "$withval" = "no"; then
      ac_fontconfig_path=
      $2
    elif test "$withval" = "yes"; then
      ac_fontconfig_path=$libpath
    else
      ac_fontconfig_path="$withval"
    fi
  ],[ac_fontconfig_path=$libpath])

  if test "$ac_theora_path" = "$lib_path"; then
    ac_libsuff=$libsuff
  else
    ac_libsuff=$kdelibsuff
  fi

  if test "$ac_fontconfig_path" != ""; then
    saved_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS -I$ac_fontconfig_path/include"
    AC_CHECK_HEADER([fontconfig/fontconfig.h], [
      saved_LDFLAGS="$LDFLAGS"
      LDFLAGS="$LDFLAGS -L$ac_fontconfig_path/lib$ac_libsuff"
      AC_CHECK_LIB(fontconfig, FcConfigHome, [
        AC_SUBST(FONTCONFIG_CPPFLAGS, [-I$ac_fontconfig_path/include])
        AC_SUBST(FONTCONFIG_LDFLAGS, [-L$ac_fontconfig_path/lib$ac_libsuff])
        AC_SUBST(FONTCONFIG_LIBS, [-lfontconfig])
        AC_DEFINE([HAVE_LIBFONTCONFIG])
        ac_have_libfontconfig="yes"
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
  AM_CONDITIONAL(HAVE_LIBFONTCONFIG, test "$ac_have_libfontconfig" = "yes")
])
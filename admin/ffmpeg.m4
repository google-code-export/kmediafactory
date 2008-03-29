dnl -----------------------------------------------------------------
dnl AC_LIB_FFMPEG([ACTION-IF-TRUE], [ACTION-IF-FALSE])
dnl -----------------------------------------------------------------

OPTION="  --with-libffmpeg=DIR   prefix for ffmpeg library files and headers"

AC_DEFUN([AC_LIB_FFMPEG], [
  AH_TEMPLATE([HAVE_LIBFFMPEG], [Define if ffmpeg is available])

  AC_FIND_LIB("libavcodec", libpath, libsuff)

  AC_ARG_WITH(libavcodec, [$OPTION], [
    if test "$withval" = "no"; then
      ac_ffmpeg_path=
      $2
    elif test "$withval" = "yes"; then
      ac_ffmpeg_path=$libpath
    else
      ac_ffmpeg_path="$withval"
    fi
  ],[ac_ffmpeg_path=$libpath])

  if test "$ac_ffmpeg_path" = "$lib_path"; then
    ac_libsuff=$libsuff
  else
    ac_libsuff=$kdelibsuff
  fi

  if test "$ac_ffmpeg_path" != ""; then
    saved_CPPFLAGS="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS -I$ac_ffmpeg_path/include"
    AC_CHECK_HEADER([ffmpeg/avcodec.h], [
      saved_LDFLAGS="$LDFLAGS"
      LDFLAGS="$LDFLAGS -L$ac_ffmpeg_path/lib$ac_libsuff"
      AC_CHECK_LIB(avcodec, avcodec_version, [
        AC_SUBST(FFMPEG_CPPFLAGS, [-I$ac_ffmpeg_path/include])
        AC_SUBST(FFMPEG_LDFLAGS, [-L$ac_ffmpeg_path/lib$ac_libsuff])
        AC_SUBST(FFMPEG_LIBS, ["-lavutil -lavcodec -lavformat"])
        AC_DEFINE([HAVE_LIBFFMPEG])
        ac_have_libffmpeg="yes"
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
  AM_CONDITIONAL(HAVE_LIBFFMPEG, test "$ac_have_libffmpeg" = "yes")
])
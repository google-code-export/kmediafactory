dnl -----------------------------------------------------------------
dnl AC_XINERAMA()
dnl -----------------------------------------------------------------
dnl From kaffeine configure.in.in

AC_DEFUN([AC_XINERAMA], [
  AC_CHECK_LIB(Xinerama, XineramaQueryExtension,
              X_PRE_LIBS="$X_PRE_LIBS -lXinerama"
              AC_DEFINE(HAVE_XINERAMA,,
                    [Define this if you have libXinerama installed])
              ac_have_xinerama="yes",,
              $X_LIBS $X_PRE_LIBS -lXext $X_EXTRA_LIBS)
  AM_CONDITIONAL(HAVE_XINERAMA, test x$ac_have_xinerama = "xyes")
])


dnl -----------------------------------------------------------------
dnl AC_IMAGEMAGICK([ACTION-IF-TRUE], [ACTION-IF-FALSE])
dnl -----------------------------------------------------------------
dnl From koffice configure.in.in
dnl Check for ImageMagick...


AC_DEFUN([AC_IMAGEMAGICK], [
  KDE_FIND_PATH(Magick++-config, MAGICKPP_CONFIG,
                [$kde_default_bindirs], $2)

  if test -n "$MAGICKPP_CONFIG"; then
    vers=`$MAGICKPP_CONFIG --version 2>/dev/null | $AWK 'BEGIN { FS = "."; } \
          { printf "%d", ($[]1 * 1000 + \$[]2) * 1000 + $[]3;}'`
    if test -n "$vers" && test "$vers" -ge 6000003
    then
      dnl Fedora crashes if linked with -lgs
      LIBMAGICK_LIBS="`$MAGICKPP_CONFIG --libs | sed -e 's/-lgs//g'`"
      LIBMAGICK_LDFLAGS="`$MAGICKPP_CONFIG --ldflags`"
      LIBMAGICK_RPATH=
      for args in $LIBMAGICK_LIBS; do
            case $args in
              -L*)
                LIBMAGICK_RPATH="$LIBMAGICK_RPATH $args"
                ;;
            esac
      done
      LIBMAGICK_RPATH=`echo $LIBMAGICK_RPATH | $SED -e "s/-L/-R/g"`
      LIBMAGICK_CPPFLAGS="`$MAGICKPP_CONFIG --cppflags`"
      if test ! "$USE_RPATH" = "yes"; then
          LIBMAGICK_RPATH=
      fi
      AC_SUBST(LIBMAGICK_LIBS)
      AC_SUBST(LIBMAGICK_LDFLAGS)
      AC_SUBST(LIBMAGICK_CPPFLAGS)
      AC_SUBST(LIBMAGICK_RPATH)
      AC_DEFINE(HAVE_LIBMAGICK,1, [ImageMagick is available])
      ac_have_libmagick="yes"
      $1
    else
      :
      $2
    fi
  fi
  AM_CONDITIONAL(HAVE_LIBMAGICK, test x$ac_have_libmagick = "xyes")
])
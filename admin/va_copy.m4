dnl -----------------------------------------------------------------
dnl AC_VA_COPY()
dnl -----------------------------------------------------------------
dnl From samba configure.in

AC_DEFUN([AC_VA_COPY], [
  AC_CACHE_CHECK([for va_copy],DOPR_HAVE_VA_COPY,[
  AC_TRY_LINK([#include <stdarg.h>
  va_list ap1,ap2;], [va_copy(ap1,ap2);],
  DOPR_HAVE_VA_COPY=yes,
  DOPR_HAVE_VA_COPY=no)])
  if test x"$DOPR_HAVE_VA_COPY" = x"yes"; then
      AC_DEFINE(DOPR_HAVE_VA_COPY,1,[Whether va_copy() is available])
  else
      AC_CACHE_CHECK([for __va_copy],DOPR_HAVE___VA_COPY,[
      AC_TRY_LINK([#include <stdarg.h>
      va_list ap1,ap2;], [__va_copy(ap1,ap2);],
      DOPR_HAVE___VA_COPY=yes,
      DOPR_HAVE___VA_COPY=no)])
      if test x"$DOPR_HAVE___VA_COPY" = x"yes"; then
          AC_DEFINE(DOPR_HAVE___VA_COPY,1,[Whether __va_copy() is available])
      fi
  fi
])


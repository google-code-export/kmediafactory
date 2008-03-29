dnl -----------------------------------------------------------------
dnl AC_UNOPKG([ACTION-IF-TRUE], [ACTION-IF-FALSE])
dnl -----------------------------------------------------------------

OPTION="  --with-unopkg=/path/unopkg   unopkg location"

AC_DEFUN([AC_UNOPKG], [
  AH_TEMPLATE([HAVE_UNOPKG], [Define if unopkg is available])
  AC_ARG_WITH(unopkg, [$OPTION], [
    if test "$withval" = "no"; then
      ac_unopkg_path=""
    elif test "$withval" = "yes"; then
      ac_unopkg_path="yes"
    else
      ac_unopkg_path="$withval"
    fi
  ],[ac_unopkg_path="yes"])

  if test "$ac_unopkg_path" = "yes"; then
    ac_unopkg_path=""
    KDE_FIND_PATH(unopkg, ac_unopkg_path, [$kde_default_bindirs])

    for dir in "openoffice" "openoffice2" \
               "openoffice.org2.0" "openoffice.org-2.0" \
               "ooo-1.9" "ooo-2.0"; do
      if test "$ac_unopkg_path" = ""; then
        AC_MSG_CHECKING([for $dir/program/unopkg])
        for lib in "/usr/lib" "/opt" `grep "^/" /etc/ld.so.conf`; do
          if test -x $lib/$dir/program/unopkg; then
            ac_unopkg_path=$lib/$dir/program/unopkg
            AC_MSG_RESULT($ac_unopkg_path)
            break
          fi
        done
        if test "$ac_unopkg_path" == ""; then
          AC_MSG_RESULT(not found)
        else
          break
        fi
      fi
    done
  fi

  if test "$ac_unopkg_path" != ""; then
    UNOPKG=$ac_unopkg_path
    AC_SUBST(UNOPKG)
    AC_DEFINE([HAVE_UNOPKG])
  else
  :
    $2
  fi

  AM_CONDITIONAL(HAVE_UNOPKG, test "$ac_unopkg_path" != "")
])
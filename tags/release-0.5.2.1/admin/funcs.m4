dnl ------------------------------------------------------------------------
dnl Find a lib
dnl ------------------------------------------------------------------------
dnl

AC_DEFUN([AC_FIND_LIB],
[
$2=""
$3=""

if test "x$kdelibsuff" = "x"; then
  lib_list="lib"
else
  lib_list="lib$kdelibsuff lib"
fi

for j in $lib_list;
do
  for i in "/usr" "/usr/local" "/opt" "/usr/X11R6";
  do
    for ext in a so sl; do
      if test -r "$i/$j/$1.$ext"; then
        $2=$i
        $3=${j:3}
        break 3
      fi
    done
  done
done
])

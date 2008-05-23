#! /usr/bin/env bash
export EXTRACTRC=`which extractrc`
export podir="$(pwd)/po"

XGETTEXT=`which xgettext`
XGETTEXT_FLAGS="--copyright-holder=This_file_is_part_of_KMediaFactory --from-code=UTF-8 \
-C --kde -ci18n -ki18n:1 -ki18nc:1c,2 -ki18np:1,2 -ki18ncp:1c,2,3 -ktr2i18n:1 -kI18N_NOOP:1 \
-kI18N_NOOP2:1c,2 -kaliasLocale -kki18n:1 -kki18nc:1c,2 -kki18np:1,2 -kki18ncp:1c,2,3 \
--msgid-bugs-address=damu@iki.fi"

echo "extractrc : $EXTRACTRC"
echo "xgettext  : $XGETTEXT"
echo "podir     : $podir"
echo ""

export XGETTEXT="$XGETTEXT $XGETTEXT_FLAGS"

for msh in `find . -mindepth 2 -name Messages.sh`; do
  echo "Processing dir: $(dirname $msh)"
  cd `dirname $msh`
  bash `basename $msh`
  cd - > /dev/null
done
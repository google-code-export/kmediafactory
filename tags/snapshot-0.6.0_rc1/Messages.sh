#! /usr/bin/env bash
export EXTRACTRC=`which extractrc`
export XGETTEXT=`which xgettext`
export podir="$(pwd)/po"

for msh in `find . -mindepth 2 -name Messages.sh`; do
  cd `dirname $msh`
  bash `basename $msh`
  cd - > /dev/null
done
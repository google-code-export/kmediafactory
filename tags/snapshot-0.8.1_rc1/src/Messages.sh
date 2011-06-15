#! /usr/bin/env bash

$EXTRACTRC *.ui >>  rc.cpp
$EXTRACTRC *.rc >> rc.cpp
$XGETTEXT *.kcfg *.cpp *.h -o $podir/kmediafactory.pot
rm -f rc.cpp

#! /usr/bin/env bash

$EXTRACTRC *.ui >  rc.cpp
$EXTRACTRC *.rc >> rc.cpp
$XGETTEXT *.cpp *.h -o $podir/libkmf.pot
rm -f rc.cpp
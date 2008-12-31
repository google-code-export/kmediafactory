#! /usr/bin/env bash

$EXTRACTRC *.ui >  rc.cpp
$EXTRACTRC *.rc >> rc.cpp
$XGETTEXT *.cpp *.h -o $podir/kmediafactory_interface.pot
rm -f rc.cpp
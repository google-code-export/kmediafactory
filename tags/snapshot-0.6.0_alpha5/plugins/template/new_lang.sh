#!/bin/sh

if [ "$1" == "" ]; then
    echo "No language specified."
    exit 1
fi

SUBDIRS=`grep "SUBDIRS =" Makefile.am | sed 's/SUBDIRS = //'`
ROOT=`pwd`

echo "make kmft-messages"

for subdir in $SUBDIRS; do
    d1="$ROOT/$subdir/locale"
    d2="$d1/$1"
    d3="$d2/LC_MESSAGES"

    mkdir $d2
    mkdir $d3

    echo " $1" >> $d1/Makefile.am

    echo "SUBDIRS = LC_MESSAGES" > $d2/Makefile.am

    echo "all:" > $d3/Makefile.am
    echo "	\$(MSGFMT) \$(srcdir)/template.po -o template.mo" >> \
            $d3/Makefile.am
    echo "	\$(MSGFMT) \$(srcdir)/ui.po -o ui.mo" >> $d3/Makefile.am

    mv $subdir/ui.pot $d3/ui.po
    mv $subdir/template.pot $d3/template.po

    if [ -e $ROOT/.svn ]; then
        svn add $d2
        svn propset svn:ignore Makefile.in $d2
        svn propset svn:ignore Makefile.in $d3
    fi
done

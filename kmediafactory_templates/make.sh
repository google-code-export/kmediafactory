#!/bin/bash

function package_template()
{
    mkdir .tmp
    cp $1/* .tmp
    cp -ax $1/images .tmp

    cd $1
    mkdir ../.tmp/locale
    for directory in `find locale -maxdepth 1 -type d -name "??"`; do
        echo $directory
        mkdir ../.tmp/$directory
        mkdir ../.tmp/$directory/LC_MESSAGES
        msgfmt $directory/LC_MESSAGES/template.po \
               -o ../.tmp/$directory/LC_MESSAGES/template.mo
        msgfmt $directory/LC_MESSAGES/ui.po \
               -o ../.tmp/$directory/LC_MESSAGES/ui.mo
    done
    cd ..

    rm $1.kmft
    cd .tmp
    zip -9 -X ../$1.kmft -r . -x "*.svn/*"
    cd ..
    rm -fR .tmp
}

package_template simple_ii
package_template preview_6_fixed
package_template travel
#!/bin/bash

function package_tool()
{
    echo "Compressing: $1"
    cd $1
    tar -cvj --exclude='*~' --exclude='.svn' -f ../$1.tar.bz2 *
    cd ..
}

package_tool dvb
package_tool dvdauthor

#!/bin/sh

TARGET=`pwd`
BUILD="$TARGET/debug"
LINKS="$HOME/.kde4"

echo "Build : $BUILD"
echo "Links : $LINKS"

plugins=(
    "video"
    "template"
    "output"
    "slideshow"
)

libs=(
    "libkmf"
    "libkmediafactoryinterfaces"
    "libkmediafactorykstore"
)

function softlink()
{
  if [ -e $1 ]; then
    BN=`basename $1`
    echo "Link  : $BN"
    if [ -e $2 ]; then
      rm $2
    fi
    ln -s $1 $2
  else
    echo "NOT FO: $1"
  fi
}

function makedir()
{
  if [ ! -d $1 ]; then
    echo "mkdir : $1"
    mkdir -p $1
  fi
}

makedir $LINKS/lib
makedir $LINKS/lib/kde4

makedir $LINKS/share
makedir $LINKS/share/locale
makedir $LINKS/share/locale/fi
makedir $LINKS/share/locale/fi/LC_MESSAGES

makedir $LINKS/lib/kde4/plugins
makedir $LINKS/lib/kde4/plugins/designer

# mo softlinks
for lang in $BUILD/po/*; do
    if [ -d $lang ]; then
        la=`basename $lang`
        for gmo in $BUILD/po/$la/*.gmo; do
            tmp=`basename $gmo`
            mo=${tmp%"gmo"}"mo"
            echo "$la - $mo"
            makedir $LINKS/share/locale/$la
            makedir $LINKS/share/locale/$la/LC_MESSAGES
            softlink $gmo $LINKS/share/locale/$la/LC_MESSAGES/$mo
        done
    fi
done

# Plugin libs
for plugin in "${plugins[@]}"
do
    softlink $BUILD/lib/kmediafactory_$plugin.la \
             $LINKS/lib/kde4/kmediafactory_$plugin.la
    softlink $BUILD/lib/kmediafactory_$plugin.so \
             $LINKS/lib/kde4/kmediafactory_$plugin.so
done

# lib softlinks
for lib in "${libs[@]}"
do
    softlink $BUILD/lib/$lib.la $LINKS/lib/$lib.la
    softlink $BUILD/lib/$lib.so $LINKS/lib/$lib.so
    softlink $BUILD/lib/$lib.so.1 $LINKS/lib/$lib.so.1
    softlink $BUILD/lib/$lib.so.1.0.0 $LINKS/lib/$lib.so.1.0.0
done

# designer plugin
softlink $BUILD/lib/kmfwidgets.so \
        $LINKS/lib/kde4/plugins/designer/kmfwidgets.so
softlink $BUILD/lib/kmfwidgets.la \
        $LINKS/lib/kde4/plugins/designer/kmfwidgets.la

# template links
cd $BUILD/plugins/template/
for file in `find . -name *.kmft`; do
  basename=`basename $file`
  softlink $BUILD/plugins/template/$file \
           $LINKS/share/apps/kmediafactory_template/$basename
done
cd -
#!/bin/bash

TARGET=`pwd`
BUILD=$TARGET'/debug'
LINKS=`echo ~`'/.kde'
HOME=`echo ~`

plugins=(
    "video"
    "template"
    "output"
    "dvimport"
    "slideshow"
)

icons=(
    "16"
    "22"
    "32"
    "48"
    "64"
    "128"
    "sc"
)

libs=(
    "libkmf.so.0.0.0"
    "libkmf.so.0"
    "libkmf.so"
    "libkmf.la"
)

headers=(
  "kmfobject.h"
  "plugin.h"
  "projectinterface.h"
  "uiinterface.h"
  "common.h"
  "color.h"
  "font.h"
  "rect.h"
)

function softlink()
{
  if [ -L $2 ]; then
    rm $2
  fi
  ln -s $1 $2
}

function makedir()
{
  if [ ! -d $1 ]; then
    mkdir $1
  fi
}

makedir $LINKS/lib
makedir $LINKS/lib/kde3

makedir $LINKS/share
makedir $LINKS/share/locale
makedir $LINKS/share/locale/fi
makedir $LINKS/share/locale/fi/LC_MESSAGES

makedir $LINKS/share/apps/kmediafactory/tools

softlink $TARGET/scripts \
         $LINKS/share/apps/kmediafactory/scripts

# dvb tool softlinks
#echo "dvb tool"
#softlink $TARGET/dvb/add_dvb.desktop \
#    $LINKS/share/apps/kmediafactory/tools/add_dvb.desktop
#softlink $TARGET/dvb/kmf_dvb_edit.sh \
#    $HOME/bin/kmf_dvb_edit.sh

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

# Plugin softlinks
for plugin in "${plugins[@]}"
do
    # Plugin libs
    echo "kmediafactory_$plugin.la"
    softlink $BUILD/plugins/$plugin/.libs/kmediafactory_$plugin.la \
             $LINKS/lib/kde3/kmediafactory_$plugin.la
    echo "kmediafactory_$plugin.so"
    softlink $BUILD/plugins/$plugin/.libs/kmediafactory_$plugin.so \
             $LINKS/lib/kde3/kmediafactory_$plugin.so
    echo "makedir kmediafactory_$plugin"
    # apps
    makedir $LINKS/share/apps/kmediafactory_$plugin
    echo "kmediafactory_${plugin}ui.rc"
    softlink $TARGET/plugins/$plugin/kmediafactory_${plugin}ui.rc \
      $LINKS/share/apps/kmediafactory_$plugin/kmediafactory_${plugin}ui.rc
    # Services
    echo "kmediafactory_$plugin.desktop"
    softlink $TARGET/plugins/$plugin/kmediafactory_$plugin.desktop \
             $LINKS/share/services/kmediafactory_$plugin.desktop
done

# lib softlinks
for lib in "${libs[@]}"
do
    echo "$lib"
    softlink $BUILD/kmediafactory/.libs/$lib $LINKS/lib/$lib
done

# apps
makedir $LINKS/bin
echo "executables"
softlink $BUILD/player/kmediafactoryplayer \
         $HOME/bin/kmediafactoryplayer
softlink $BUILD/src/kmediafactory \
         $HOME/bin/kmediafactory

makedir $LINKS/share/apps/kmediafactory
echo "kmediafactoryui.rc"
softlink $TARGET/src/kmediafactoryui.rc \
         $LINKS/share/apps/kmediafactory/kmediafactoryui.rc
echo "kmediafactoryplayerui.rc"
softlink $TARGET/player/kmediafactoryplayerui.rc \
         $LINKS/share/apps/kmediafactory/kmediafactoryplayerui.rc

# Service types
echo "kmediafactory_$plugin.desktop"
softlink $TARGET/kmediafactory/kmediafactoryplugin.desktop \
         $LINKS/share/servicetypes/kmediafactoryplugin.desktop

# app softlinks
makedir $LINKS/share/applnk/Utilities
echo "kmediafactory.desktop"
softlink $TARGET/src/kmediafactory.desktop \
         $LINKS/share/applnk/Utilities/kmediafactory.desktop
echo "kmediafactoryplayer.desktop"
softlink $TARGET/player/kmediafactoryplayer.desktop \
         $LINKS/share/applnk/Utilities/kmediafactoryplayer.desktop

# mime softlink
echo "kmediafactory_$plugin.desktop"
softlink $TARGET/mimetypes/x-kmediafactory.desktop \
         $LINKS/share/mimelnk/application/x-kmediafactory.desktop

# include softlink
makedir $LINKS/include
makedir $LINKS/include/kmediafactory
makedir $LINKS/include/kmediafactory/plugin
for header in "${headers[@]}"
do
  echo "$header"
  softlink $TARGET/kmediafactory/$header \
          $LINKS/include/kmediafactory/$header
done
echo "videoobject.h"
softlink $TARGET/plugins/video/videoobject.h \
        $LINKS/include/kmediafactory/plugin/videoobject.h
echo "qffmpeg.h"
softlink $TARGET/ffmpeg/cppapi/qffmpeg.h \
        $LINKS/include/kmediafactory/qffmpeg.h
echo "qdvdinfo.h"
softlink $TARGET/ffmpeg/cppapi/qdvdinfo.h \
        $LINKS/include/kmediafactory/qdvdinfo.h

echo "designer plugin"
makedir $LINKS/lib/kde3/plugins
makedir $LINKS/lib/kde3/plugins/designer
softlink $BUILD/lib/.libs/kmfwidgets.so \
        $LINKS/lib/kde3/plugins/designer/kmfwidgets.so
softlink $BUILD/lib/.libs/kmfwidgets.la \
        $LINKS/lib/kde3/plugins/designer/kmfwidgets.la
makedir $LINKS/share/apps/kmediafactorywidgets
makedir $LINKS/share/apps/kmediafactorywidgets/pics
cd $TARGET/lib/pics
for file in `find . -name "*.png"`; do
  softlink $TARGET/lib/pics/$file \
           $LINKS/share/apps/kmediafactorywidgets/pics/$file
done


# icon softlinks
ICONS=$LINKS'/share/icons/crystalsvg'
makedir $LINKS/share/icons
makedir $ICONS
for icon in "${icons[@]}"
do
    if [ "$icon" == "sc" ]; then
        echo "$ICONS/scalable"
        makedir "$ICONS/scalable"
        makedir "$ICONS/scalable/mimetypes"
        makedir "$ICONS/scalable/apps"
        softlink "$TARGET/icons/cr""$icon""-mime-kmediafactory_project.svgz" \
                 "$ICONS/scalable/mimetypes/kmediafactory_project.svgz"
        softlink "$TARGET/icons/cr""$icon""-app-kmediafactory.svgz" \
                 "$ICONS/scalable/apps/kmediafactory.svgz"
    else
        echo "$ICONS/""$icon""x""$icon"
        makedir "$ICONS/""$icon""x""$icon"
        makedir "$ICONS/""$icon""x""$icon""/mimetypes"
        makedir "$ICONS/""$icon""x""$icon""/apps"
        softlink "$TARGET/icons/cr""$icon""-mime-kmediafactory_project.png" \
              "$ICONS/""$icon""x""$icon""/mimetypes/kmediafactory_project.png"
        softlink "$TARGET/icons/cr""$icon""-app-kmediafactory.png" \
                 "$ICONS/""$icon""x""$icon""/apps/kmediafactory.png"
    fi
done

# template png links
echo "$BUILD/plugins/template/ kmfts"
cd $BUILD/plugins/template/
for file in `find . -name *.kmft`; do
  basename=`basename $file`
  softlink $BUILD/plugins/template/$file \
           $LINKS/share/apps/kmediafactory_template/$basename
done
cd -
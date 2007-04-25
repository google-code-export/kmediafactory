#!/bin/bash

# Generate list from install dir like this:
# find . -type f -name "*" -printf "\"%P\"\n"

if [[ "$1" == "--remove" ]]; then
  REMOVE="1"
fi

TARGET=`pwd`
LINKS="$HOME/.kde4"

echo "Target: $TARGET"
echo "Links : $LINKS"

FILES=(
"lib/libkmf.so.1.0.0"
"*lib/kde4/plugins/designer/kmfwidgets.so"
"lib/kde4/kmediafactory_template.so"
"lib/kde4/kmediafactory_output.so"
"lib/kde4/kmediafactory_slideshow.so"
"lib/kde4/kmediafactory_video.so"
"lib/libkmediafactorykstore.so.1.0.0"
"lib/libkmediafactoryinterfaces.so.1.0.0"
"share/apps/kmfwidgets/pics/kmffontchooser.png"
"share/apps/kmfwidgets/pics/kmflanguagelistbox.png"
"share/apps/kmfwidgets/pics/kmflanguagecombobox.png"
"share/apps/kmfwidgets/pics/kmfimageview.png"
"share/apps/kmediafactory/kmediafactoryui.rc"
"share/apps/kmediafactory/scripts/frame"
"share/apps/kmediafactory/scripts/info"
"share/apps/kmediafactory/scripts/make_mpeg"
"share/apps/kmediafactory/scripts/tools"
"share/apps/kmediafactory/scripts/kmf_converter.oxt"
"share/apps/kmediafactory/scripts/oo2pdf"
"share/apps/kmediafactory/media/silence.mp2"
"share/apps/kmediafactory_template/kmediafactory_templateui.rc"
"share/apps/kmediafactory_template/simple.kmft"
"share/apps/kmediafactory_template/preview_6.kmft"
"share/apps/kmediafactory_template/preview_3.kmft"
"share/apps/kmediafactory_template/preview_1.kmft"
"share/apps/kmediafactory_output/kmediafactory_outputui.rc"
"share/apps/kmediafactory_slideshow/kmediafactory_slideshowui.rc"
"share/apps/kmediafactory_video/kmediafactory_videoui.rc"
"*share/kde4/servicetypes/kmediafactoryplugin.desktop"
"*share/kde4/services/kmediafactory_video.desktop"
"*share/kde4/services/kmediafactory_template.desktop"
"*share/kde4/services/kmediafactory_output.desktop"
"*share/kde4/services/kmediafactory_slideshow.desktop"
"share/config/kmediafactory.knsrc"
"share/config/kmediafactory_template.knsrc"
"share/config.kcfg/kmediafactory.kcfg"
"share/config.kcfg/templateplugin.kcfg"
"share/config.kcfg/slideshowplugin.kcfg"
"share/config.kcfg/videoplugin.kcfg"
"share/mimelnk/application/x-kmediafactory.desktop"
"share/mimelnk/application/x-kmediafactory-template.desktop"
"share/icons/oxygen/128x128/apps/kmediafactory.png"
"share/icons/oxygen/128x128/mimetypes/kmediafactory_project.png"
"share/icons/oxygen/32x32/actions/add_video.png"
"share/icons/oxygen/32x32/mimetypes/kmediafactory_project.png"
"share/icons/oxygen/32x32/apps/kmediafactory.png"
"share/icons/oxygen/22x22/apps/kmediafactory.png"
"share/icons/oxygen/22x22/mimetypes/kmediafactory_project.png"
"share/icons/oxygen/16x16/mimetypes/kmediafactory_project.png"
"share/icons/oxygen/16x16/apps/kmediafactory.png"
"share/icons/oxygen/64x64/mimetypes/kmediafactory_project.png"
"share/icons/oxygen/64x64/apps/kmediafactory.png"
"share/icons/oxygen/48x48/mimetypes/kmediafactory_project.png"
"share/icons/oxygen/48x48/apps/kmediafactory.png"
"share/icons/oxygen/scalable/apps/kmediafactory.svgz"
"share/icons/oxygen/scalable/mimetypes/kmediafactory_project.svgz"
"include/kmediafactory/plugin.h"
"include/kmediafactory/projectinterface.h"
"include/kmediafactory/uiinterface.h"
"include/kmediafactory/kmfobject.h"
"bin/kmediafactory"
)

function makefilesdir()
{
  DIR=`dirname $1`
  if [ ! -d $DIR ]; then
    mkdir -p $DIR
  fi
}

function softlink()
{
  if [ -e $1 ]; then
    if [ -e $2 ]; then
      rm $2
    else
      makefilesdir $2
    fi
    if [[ $REMOVE != "1" ]]; then
      echo "LINK: $2"
      echo "   -> $1"
      ln -s $1 $2
    else
      echo "REMOVED: $2"
    fi
  else
    echo "NOT FOUND: $1"
  fi
}

function copy()
{
  if [ -e $1 ]; then
    if [ -e $2 ]; then
      rm $2
    else
      makefilesdir $2
    fi
    if [[ $REMOVE != "1" ]]; then
      echo "COPY: $1"
      echo "   -> $2"
      cp $1 $2
    else
      echo "REMOVED: $2"
    fi
  else
    echo "NOT FOUND: $1"
  fi
}

for FILE in "${FILES[@]}"
do
  if [[ "${FILE:0:1}" == "*" ]]; then
    COPY="1"
    FILE=${FILE:1}
  else
    COPY="0"
  fi
  NAME=`basename $FILE`
  FOUND=`find . -type f -name "$NAME" -printf "%P"`
  if [[ "$FOUND" == "" ]]; then
    N=`echo $FILE | sed -e 's/.*\/\([0-9]*\)[x0-9]*\/.*/\1/'`
    if [[ "$N" != "$FILE" ]]; then
      FOUND=`find . -type f -name "*$NAME" -printf "%P\n" | grep $N`
    else
      FOUND=`find . -type f -name "*$NAME" -printf "%P"`
    fi
  fi
  if [[ "$FOUND" != "" ]]; then
    if [[ "$COPY" == "1" ]]; then
      copy "$TARGET/$FOUND" "$LINKS/$FILE"
    else
      softlink "$TARGET/$FOUND" "$LINKS/$FILE"
    fi
  else
    echo "NOT FOUND: $FILE"
  fi
done


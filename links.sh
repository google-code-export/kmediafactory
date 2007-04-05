#!/bin/bash

# Generate list from install dir like this:
# find . -type f -name "*" -printf "\"%P\"\n"

TARGET=`pwd`
LINKS="$HOME/.kde4"

echo "Target: $TARGET"
echo "Links : $LINKS"

FILES=(
"lib/libkmf.so.1.0.0"
"lib/libkmf.la"
"lib/kde4/plugins/designer/kmfwidgets.so"
"lib/kde4/plugins/designer/kmfwidgets.la"
"lib/kde4/libkmediafactorykstore.la"
"lib/kde4/kmediafactory_template.la"
"lib/kde4/kmediafactory_template.so"
"lib/kde4/kmediafactory_output.la"
"lib/kde4/kmediafactory_output.so"
"lib/kde4/kmediafactory_slideshow.la"
"lib/kde4/kmediafactory_slideshow.so"
"lib/kde4/kmediafactory_video.la"
"lib/kde4/kmediafactory_video.so"
"lib/libkmediafactorykstore.so.1.0.0"
"lib/libkmediafactoryinterfaces.so.1.0.0"
"share/apps/kmediafactorywidgets/pics/kmffontchooser.png"
"share/apps/kmediafactorywidgets/pics/kmfimageview.png"
"share/apps/kmediafactory/kmediafactoryui.rc"
"share/apps/kmediafactory_template/kmediafactory_templateui.rc"
"share/apps/kmediafactory_template/simple.kmft"
"share/apps/kmediafactory_template/preview_6.kmft"
"share/apps/kmediafactory_template/preview_3.kmft"
"share/apps/kmediafactory_template/preview_1.kmft"
"share/apps/kmediafactory_output/kmediafactory_outputui.rc"
"share/apps/kmediafactory_slideshow/kmediafactory_slideshowui.rc"
"share/apps/kmediafactory_video/kmediafactory_videoui.rc"
"share/kde4/servicetypes/kmediafactoryplugin.desktop"
"share/kde4/services/kmediafactory_template.desktop"
"share/kde4/services/kmediafactory_output.desktop"
"share/kde4/services/kmediafactory_slideshow.desktop"
"share/kde4/services/kmediafactory_video.desktop"
"share/config.kcfg/kmediafactory.kcfg"
"share/config.kcfg/templateplugin.kcfg"
"share/config.kcfg/slideshowplugin.kcfg"
"share/config.kcfg/videoplugin.kcfg"
"share/mimelnk/application/x-kmediafactory.desktop"
"share/mimelnk/application/x-kmediafactory-template.desktop"
"share/icons/crystalsvg/128x128/apps/kmediafactory.png"
"share/icons/crystalsvg/128x128/mimetypes/kmediafactory_project.png"
"share/icons/crystalsvg/32x32/actions/add_video.png"
"share/icons/crystalsvg/32x32/mimetypes/kmediafactory_project.png"
"share/icons/crystalsvg/32x32/apps/kmediafactory.png"
"share/icons/crystalsvg/22x22/apps/kmediafactory.png"
"share/icons/crystalsvg/22x22/mimetypes/kmediafactory_project.png"
"share/icons/crystalsvg/16x16/mimetypes/kmediafactory_project.png"
"share/icons/crystalsvg/16x16/apps/kmediafactory.png"
"share/icons/crystalsvg/64x64/mimetypes/kmediafactory_project.png"
"share/icons/crystalsvg/64x64/apps/kmediafactory.png"
"share/icons/crystalsvg/48x48/mimetypes/kmediafactory_project.png"
"share/icons/crystalsvg/48x48/apps/kmediafactory.png"
"share/icons/crystalsvg/scalable/apps/kmediafactory.svgz"
"share/icons/crystalsvg/scalable/mimetypes/kmediafactory_project.svgz"
"include/kmediafactory/plugin.h"
"include/kmediafactory/projectinterface.h"
"include/kmediafactory/uiinterface.h"
"include/kmediafactory/kmfobject.h"
"bin/kmediafactory"
)

function softlink()
{
  if [ -e $1 ]; then
#if [ -e $2 ]; then
#     rm $2
#   fi
    echo "LINK: $1"
    echo "   -> $2"
    #ln -s $1 $2
  else
    echo "NOT FOUND: $1"
  fi
}

for FILE in "${FILES[@]}"
do
  NAME=`basename $FILE`
  FOUND=`find . -type f -name "$NAME" -printf "%P"`
  if [[ "$FOUND" == "" ]]; then
    N=`echo $FILE | sed -e 's/.*\/\([0-9]*\)[x0-9]*\/.*/\1/'`
    if [[ "$N" != "$FILE" ]]; then
      FOUND=`find . -type f -name "*$NAME" -printf "%P" | grep $N`
    else
      FOUND=`find . -type f -name "*$NAME" -printf "%P"`
    fi
  fi
  softlink "$LINKS/$FILE" "$TARGET/$FOUND"
done


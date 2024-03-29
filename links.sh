#!/bin/bash

# Generate list from install dir like this:
# find . -type f -name "*" -printf "\"%P\"\n"

SO=`grep "KMF_MAJOR_VERSION " CMakeLists.txt | sed 's/set(KMF_MAJOR_VERSION "//' | sed 's/")//'`
mi=`grep "KMF_MINOR_VERSION " CMakeLists.txt | sed 's/set(KMF_MINOR_VERSION "//' | sed 's/")//'`
bu=`grep "KMF_BUILD_VERSION " CMakeLists.txt | sed 's/set(KMF_BUILD_VERSION "//' | sed 's/")//'`
VERSION="$SO.$mi.$bu"

if [[ "$1" == "--remove" ]]; then
  REMOVE="1"
fi

BUILD="$HOME/Development/build/kmediafactory"

TARGETS=(
"$BUILD"
"$HOME/Development/src/kmediafactory"
)
LINKS="$HOME/.kde"

echo "Targets: $TARGETS"
echo "Links : $LINKS"

FILES=(
"share/icons/oxygen/16x16/apps/kmediafactory.png"
"share/icons/oxygen/16x16/mimetypes/application-x-kmediafactory.png"
"share/icons/oxygen/16x16/mimetypes/application-x-kmediafactory-template.png"
"share/icons/oxygen/22x22/apps/kmediafactory.png"
"share/icons/oxygen/22x22/mimetypes/application-x-kmediafactory.png"
"share/icons/oxygen/22x22/mimetypes/application-x-kmediafactory-template.png"
"share/icons/oxygen/32x32/apps/kmediafactory.png"
"share/icons/oxygen/32x32/mimetypes/application-x-kmediafactory.png"
"share/icons/oxygen/32x32/mimetypes/application-x-kmediafactory-template.png"
"share/icons/oxygen/48x48/apps/kmediafactory.png"
"share/icons/oxygen/48x48/mimetypes/application-x-kmediafactory.png"
"share/icons/oxygen/48x48/mimetypes/application-x-kmediafactory-template.png"
"share/icons/oxygen/64x64/apps/kmediafactory.png"
"share/icons/oxygen/64x64/mimetypes/application-x-kmediafactory.png"
"share/icons/oxygen/64x64/mimetypes/application-x-kmediafactory-template.png"
"share/icons/oxygen/128x128/apps/kmediafactory.png"
"share/icons/oxygen/128x128/mimetypes/application-x-kmediafactory.png"
"share/icons/oxygen/128x128/mimetypes/application-x-kmediafactory-template.png"
"share/icons/oxygen/scalable/apps/kmediafactory.svgz"
"share/icons/oxygen/scalable/mimetypes/application-x-kmediafactory.svgz"
"share/icons/oxygen/scalable/mimetypes/application-x-kmediafactory-template.svgz"
"*lib/kde4/plugins/designer/kmfwidgets.so"
"lib/kde4/kmediafactory_template.so"
"lib/kde4/kmediafactory_output.so"
"lib/kde4/kmediafactory_slideshow.so"
"lib/kde4/kmediafactory_video.so"
"lib/kde4/kmediafactory_plugin_kross.so"
"lib/libkmf.so"
"lib/libkmediafactorykstore.so"
"lib/libkmediafactoryinterfaces.so"
"lib/libkmf.so.$SO"
"lib/libkmediafactorykstore.so.$SO"
"lib/libkmediafactoryinterfaces.so.$SO"
"lib/libkmf.so.$VERSION"
"lib/libkmediafactorykstore.so.$VERSION"
"lib/libkmediafactoryinterfaces.so.$VERSION"
"share/apps/kmfwidgets/pics/kmfimageview.png"
"share/apps/kmfwidgets/pics/kmflanguagecombobox.png"
"share/apps/kmfwidgets/pics/kmflanguagelistbox.png"
"share/apps/kmediafactory/kmediafactoryui.rc"
"share/apps/kmediafactory/scripts/kmf_converter.oxt"
"share/apps/kmediafactory/scripts/kmf_comment"
"share/apps/kmediafactory/scripts/kmf_frame"
"share/apps/kmediafactory/scripts/kmf_info"
"share/apps/kmediafactory/scripts/kmf_make_mpeg"
"share/apps/kmediafactory/scripts/kmf_oo2pdf"
"share/apps/kmediafactory/scripts/tools"
"share/apps/kmediafactory/media/silence.mp2"
"share/apps/kmediafactory/kmediafactory_templateui.rc"
"share/apps/kmediafactory_template/simple.kmft"
"share/apps/kmediafactory_template/preview_6.kmft"
"share/apps/kmediafactory_template/preview_3.kmft"
"share/apps/kmediafactory_template/preview_1.kmft"
"share/apps/kmediafactory/kmediafactory_outputui.rc"
"share/apps/kmediafactory/kmediafactory_slideshowui.rc"
"share/apps/kmediafactory/kmediafactory_videoui.rc"
"*share/kde4/servicetypes/kmediafactoryplugin.desktop"
"*share/kde4/services/kmediafactory_template.desktop"
"*share/kde4/services/kmediafactory_output.desktop"
"*share/kde4/services/kmediafactory_slideshow.desktop"
"*share/kde4/services/kmediafactory_video.desktop"
"share/config.kcfg/kmediafactory.kcfg"
"share/config.kcfg/templateplugin.kcfg"
"share/config.kcfg/slideshowplugin.kcfg"
"share/config.kcfg/videoplugin.kcfg"
"share/config/kmediafactory.knsrc"
"share/config/kmediafactory_template.knsrc"
"share/applications/kde4/kmediafactory.desktop"
"share/mime/packages/kmediafactory.xml"
"include/kmediafactory/plugin.h"
"include/kmediafactory/interface.h"
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
  if [ -e "$1" ]; then
    if [ -e "$2" ]; then
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

# mo softlinks
for lang in $BUILD/po/*; do
    if [ "$lang"!="CMakeFiles" -a -d $lang ]; then
        la=`basename $lang`
        for mo in $BUILD/po/$la/*.mo; do
            tmp=`basename $mo`
            softlink $mo $LINKS/share/locale/$la/LC_MESSAGES/$tmp
        done
    fi
done

for TARGET in "${TARGETS[@]}"
do
  cd $TARGET
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
      FOUND=`find . -type l -name "$NAME" -printf "%P"`
    fi
    if [[ "$FOUND" == "" ]]; then
      # ICONS
      END=`echo $FILE | sed -e 's/.*\/oxygen\/\(.*\)/\1/'`
      if [ "${END:0:2}" == "sc" ]; then
        END="sc-$(echo $END | sed -e 's/scalable\/\(.*\)/\1/')"
      else
        END=`echo $END | sed -e 's/.*x\([0-9].*\)/\1/'`
      fi
      END=`echo $END | sed -e 's/\//-/g'`
      FOUND=`find . -type f -name "*$END" -printf "%P"`
    fi
    if [[ "$FOUND" != "" ]]; then
      if [[ "$COPY" == "1" ]]; then
        copy "$TARGET/$FOUND" "$LINKS/$FILE"
      else
        softlink "$TARGET/$FOUND" "$LINKS/$FILE"
      fi
    fi
  done
done

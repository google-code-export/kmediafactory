#/bin/bash

NEXT_VERSION="0.5.3"

HOME=`echo ~`
LOCALKMFDIR="$HOME/public_html/software/kmediafactory"
WEB="susku.pyhaselka.fi:/home/damu/public_html/software/kmediafactory/"
CHANGELOG="snapshot.changelog"
EBUILD_BASE="kmediafactory-$NEXT_VERSION.ebuild"
NEWSMAIL="kmf-news@susku.pyhaselka.fi"
PORTAGE_OVERLAY="/usr/local/portage"
PORTAGE_KMF="$PORTAGE_OVERLAY/media-video/kmediafactory"
US_DATE=`date +%Y-%m-%d`

KMF=`pwd`
ABC="bcdefghijklmnopqrstuvxyz"
VER=""
I=0

function snapshot_name()
{
  echo -n "Snapshot name: "
  cd $KMF

  if [ "$1" == "" ]; then
    while [ 1 ]; do
      SNAPSHOT=`date +%Y%m%d`$VER
      BZ2FILE="kmediafactory-$SNAPSHOT.tar.bz2"
      DESTINATION="$LOCALKMFDIR/$BZ2FILE"
      if [ ! -e $DESTINATION ]; then
        break
      fi
      VER=${ABC:$I:1}
      let "I+=1"
    done
  else
    SNAPSHOT="$1"
    BZ2FILE="kmediafactory-$SNAPSHOT.tar.bz2"
    DESTINATION="$LOCALKMFDIR/$BZ2FILE"
  fi

  echo $DESTINATION
}


function make_ebuild()
{
  echo "Making ebuild..."
  cd $PORTAGE_KMF

  rm $PORTAGE_KMF/Manifest -f
  rm $PORTAGE_KMF/kmediafactory-200*.ebuild
  rm $PORTAGE_KMF/files/digest-kmediafactory-200*

  cp $KMF/distros/gentoo/media-video/kmediafactory/$EBUILD_BASE $PORTAGE_KMF

  EBUILD="kmediafactory-$SNAPSHOT.ebuild"
  if [ "$SNAPSHOT" != "$NEXT_VERSION" ]; then
    cat $EBUILD_BASE | sed 's/x86/~x86/g; s/amd64/~amd64/g' > $EBUILD
    rm $EBUILD_BASE
  fi

  ebuild $EBUILD digest
  cp $PORTAGE_KMF/$EBUILD $LOCALKMFDIR
}

function fix_versions()
{
  echo "Fixing configure.in.in for new version: $SNAPSHOT"
  cd $KMF

  S="AM_INIT_AUTOMAKE(\"kmediafactory\","
  sed -i -e "s/$S \".*\")/$S \"$SNAPSHOT\")/" configure.in.in

  echo "Fixing lsm file versions..."
  sed -i -e "s/Version:.*/Version:        $SNAPSHOT/" kmediafactory.lsm
  sed -i -e "s/Entered-date:.*/Entered-date:   $US_DATE/" kmediafactory.lsm
}

function edit_changelog()
{
  echo "Edit changelog"
  cd $KMF
  nano $CHANGELOG
}

function make_snapshot()
{
  echo "Making snapshot..."
  cd $KMF

  make -f Makefile.cvs
  ./configure
  make dist-bz2
  make distclean
  FILE=`ls --color=none *.tar.bz2`

  if [ "$FILE" == "" ]; then
    echo "No file"
    exit
  fi

  echo "Moving $FILE to local KMF dir and distfiles."
  cd $KMF

  mv $FILE $DESTINATION
  cp $DESTINATION "/usr/portage/distfiles/$BZ2FILE"

  echo "Making md5."
  cd $LOCALKMFDIR
  md5sum $BZ2FILE > $BZ2FILE.md5
}

function tag_svn()
{
  echo "Tagging svn..."
  cd $KMF
  if [ "$SNAPSHOT" != "$NEXT_VERSION" ]; then
    comment="Tagging the "$SNAPSHOT" snapshot."
    tag="snapshot-$SNAPSHOT"
  else
    comment="Tagging the "$SNAPSHOT" release."
    tag="release-$SNAPSHOT"
  fi
  svn copy https://susku.pyhaselka.fi/svn/kmediafactory/trunk \
      https://susku.pyhaselka.fi/svn/kmediafactory/tags/$tag \
      -m "$comment"
}

function make_html()
{
  echo "Making html files..."

  HTML="$LOCALKMFDIR/snapshot.html"

  echo "HTML Changelog to web page."

  echo -e "<h1>Snapshots</h1>\n" > $HTML
  date +"<h2>%d.%m.%Y</h2>" >> $HTML

  echo -e "<h3>Changelog</h3>\n" >> $HTML
  sed -e "s/-\(.*\)/<li>\1<\/li>/" \
      -e "s/\(.*\):/\1:\n<ul>/" \
      -e "s/=\(.*\)=/\n<br\/>/" \
      -e "s/^$/<\/ul>/" $CHANGELOG >> $HTML
  echo "</ul>" >> $HTML

  echo -e "<h3>Packages</h3>\n" >> $HTML
  echo "<ul>" >> $HTML
  echo "<li>Source package: <a href=\"$BZ2FILE\">$BZ2FILE</a> "\
      "<a href=\"$BZ2FILE.md5\">MD5</a>.</li>" >> $HTML
  echo "<li>Gentoo ebuild: <a href=\"$EBUILD\">$EBUILD</a> " >> $HTML
  echo "</ul>" >> $HTML
}

function upload()
{
  echo "Uploading files to web..."

  echo -e "put $DESTINATION.md5\nput $DESTINATION\nput " \
      "$LOCALKMFDIR/snapshot.html\nput $LOCALKMFDIR/$EBUILD" | sftp -b - $WEB
}

function mail_to_news()
{
  echo "Sending mail to news..."
  cd $KMF

  if [ "$SNAPSHOT" != "$NEXT_VERSION" ]; then
    mail -s "New snapshot - $SNAPSHOT" $NEWSMAIL < snapshot.changelog
  else
    mail -s "New release - $SNAPSHOT" $NEWSMAIL < snapshot.changelog
  fi
}

snapshot_name $1
fix_versions
edit_changelog
./commit.sh --nopause
make_snapshot

echo -n "Proceed with susku (y/N): "
read ans
if [ "$ans" == y -o "$ans" == Y ]; then
  tag_svn
  make_html
  make_ebuild
  upload
  #mail_to_news
fi

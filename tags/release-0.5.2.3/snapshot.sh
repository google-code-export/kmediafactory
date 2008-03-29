#/bin/bash

VERSION_MM="0.5.2."
NEXT_VERSION="3" # BUILD_VERSION

HOME=`echo ~`
SITE="aryhma.oy.cx"
LOCALKMFDIR="$HOME/public_html/$SITE/damu/software/kmediafactory"
WEBDIR="httpdocs/damu/software/kmediafactory"
CHANGELOG="snapshot.changelog"
US_DATE=`date +%Y-%m-%d`
SVN="https://kmediafactory.googlecode.com/svn/"
SNAPSHOT_HTML="snapshot.html"
NEWSMAIL=`cat $HOME/.kmf_mailinglist`

KMF=`pwd`
ABC="bcdefghijklmnopqrstuvxyz"
I=0

function snapshot_name()
{
  cd $KMF

  VER=""
  while [ 1 ]; do
    NEXT_FREE_SNAPSHOT="$NEXT_VERSION""_`date +%Y%m%d`$VER"
    FILE="$LOCALKMFDIR/kmediafactory-$VERSION_MM$NEXT_FREE_SNAPSHOT.tar.bz2"
    if [ ! -e $FILE ]; then
      break
    fi
    VER=${ABC:$I:1}
    let "I+=1"
  done

  echo "1. $VERSION_MM$NEXT_VERSION (Release)"
  echo "2. $VERSION_MM$NEXT_FREE_SNAPSHOT"
  echo "3. $VERSION_MM$NEXT_VERSION""_[Custom]"
  echo -n "Select version: "
  read ans

  if [ "$ans" == "1" ]; then
    SNAPSHOT_BUILD="$NEXT_VERSION"
    RELEASE="1"
  elif [ "$ans" == "2" ]; then
    SNAPSHOT_BUILD="$NEXT_FREE_SNAPSHOT"
    RELEASE="0"
  elif [ "$ans" == "3" ]; then
    echo -n "Version: "
    read ans
    SNAPSHOT_BUILD="$NEXT_VERSION""_$ans"
    RELEASE="0"
  else
    exit 0
  fi

  SNAPSHOT="$VERSION_MM$SNAPSHOT_BUILD"
  BZ2FILE="kmediafactory-$SNAPSHOT.tar.bz2"
  DESTINATION="$LOCALKMFDIR/$BZ2FILE"

  echo -n "Snapshot name: "
  echo $DESTINATION
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

  echo "Moving $FILE to local KMF dir."
  cd $KMF

  mv $FILE $DESTINATION

  echo "Making md5."
  cd $LOCALKMFDIR
  md5sum $BZ2FILE > $BZ2FILE.md5
}

function tag_svn()
{
  echo "Tagging svn..."
  cd $KMF
  if [ "$RELEASE" != "1" ]; then
    comment="Tagging the "$SNAPSHOT" snapshot."
    tag="snapshot-$SNAPSHOT"
  else
    comment="Tagging the "$SNAPSHOT" release."
    tag="release-$SNAPSHOT"
  fi
  svn copy $SVN/branches/KDE3 $SVN/tags/$tag -m "$comment"
}

function make_html()
{
  echo "Making html files..."
  cd $KMF

  HTML="$LOCALKMFDIR/$SNAPSHOT_HTML"

  echo "HTML Changelog to web page."

  echo -e "<h1>KDE 3 snapshots</h1>\n" > $HTML
  date +"<h2>%d.%m.%Y</h2>" >> $HTML

  echo -e "<h3>Changelog</h3>\n" >> $HTML
  grep -v "^#" $CHANGELOG | \
        sed -e "s/-\(.*\)/<li>\1<\/li>/" \
            -e "s/\(.*\):/\1:\n<ul>/" \
            -e "s/=\(.*\)=/\n<br\/>/" \
            -e "s/^$/<\/ul>/" >> $HTML
  echo "</ul>" >> $HTML

  echo -e "<h3>Packages</h3>\n" >> $HTML
  echo "<ul>" >> $HTML
  echo "<li>Source package: <a href=\"$BZ2FILE\">$BZ2FILE</a> "\
      "<a href=\"$BZ2FILE.md5\">MD5</a>.</li>" >> $HTML
  echo "</ul>" >> $HTML
}

function upload()
{
  echo "Uploading files to web..."

  cd $LOCALKMFDIR
  kioclient copy $BZ2FILE.md5 $BZ2FILE $SNAPSHOT_HTML ftp://$SITE/$WEBDIR/
}

function mail_to_news()
{
  echo "Sending mail to news..."
  cd $KMF

  if [ "$RELEASE" != "1" ]; then
    mail -s "New snapshot - $SNAPSHOT" $NEWSMAIL < snapshot.changelog
  else
    mail -s "New release - $SNAPSHOT" $NEWSMAIL < snapshot.changelog
  fi
}

snapshot_name
fix_versions
edit_changelog
./commit.sh --nopause $SNAPSHOT
make_snapshot

echo -n "Tag SVN (y/N): "
read ans
if [ "$ans" == y -o "$ans" == Y ]; then
  tag_svn
fi

echo -n "Upload (y/N): "
read ans
if [ "$ans" == y -o "$ans" == Y ]; then
  make_html
  upload
fi

echo -n "Send release email (y/N): "
read ans
if [ "$ans" == y -o "$ans" == Y ]; then
  mail_to_news
fi

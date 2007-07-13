#/bin/bash

VERSION_MM="0.6."
NEXT_VERSION="0" # BUILD_VERSION

HOME=`echo ~`
SITE="aryhma.oy.cx"
LOCALKMFDIR="$HOME/public_html/$SITE/damu/software/kmediafactory"
WEBDIR="/httpdocs/damu/software/kmediafactory"
CHANGELOG="snapshot.changelog"
US_DATE=`date +%Y-%m-%d`
SVN="https://kmediafactory.googlecode.com/svn/"

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
      SNAPSHOT_BUILD="$NEXT_VERSION_`date +%Y%m%d`$VER"
      SNAPSHOT="$VERSION_MM$SNAPSHOT_BUILD"
      BZ2FILE="kmediafactory-$SNAPSHOT.tar.bz2"
      DESTINATION="$LOCALKMFDIR/$BZ2FILE"
      if [ ! -e $DESTINATION ]; then
        break
      fi
      VER=${ABC:$I:1}
      let "I+=1"
    done
  else
    SNAPSHOT_BUILD="$NEXT_VERSION""_$1"
    SNAPSHOT="$VERSION_MM$SNAPSHOT_BUILD"
    BZ2FILE="kmediafactory-$SNAPSHOT.tar.bz2"
    DESTINATION="$LOCALKMFDIR/$BZ2FILE"
  fi

  echo $DESTINATION
}

function fix_versions()
{
  echo "Fixing CMakeLists.txt for new version: $SNAPSHOT"
  cd $KMF

  S="set(KMF_BUILD_VERSION"
  sed -i -e "s/$S \".*\")/$S \"$SNAPSHOT_BUILD\")/" CMakeLists.txt

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
  mkdir debug
  cd debug
  rm *.tar.bz2
  cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/kde-debug \
      -DCMAKE_BUILD_TYPE=debugfull -G KDevelop3
  make package_source
  FILE=`ls --color=none *.tar.bz2`

  if [ "$FILE" == "" ]; then
    echo "No file"
    exit
  fi

  echo "Moving $FILE to local KMF dir and distfiles."
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
  svn copy $SVN/trunk $SVN/tags/$tag -m "$comment"
}

function make_html()
{
  echo "Making html files..."

  HTML="$LOCALKMFDIR/snapshot_kde4.html"

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
  echo "</ul>" >> $HTML
}

function upload()
{
  echo "Uploading files to web..."

  NAME=`dcop kded kwalletd networkWallet`
  ID=`dcop kded kwalletd open $NAME 0`
  USER=`dcop kded kwalletd readPassword $ID ftp $SITE-user`
  PASS=`dcop kded kwalletd readPassword $ID ftp $SITE-pass`
  dcop kded kwalletd close $ID
  ftp -un $hostname <<EOF
USER $USER
PASS $PASS
binary
cd $WEBDIR
put $DESTINATION.md5
put $DESTINATION
put $LOCALKMFDIR/snapshot.html
quit
EOF
}

snapshot_name $1
fix_versions
edit_changelog
./commit.sh --nopause
make_snapshot

echo -n "Upload (y/N): "
read ans
if [ "$ans" == y -o "$ans" == Y ]; then
  tag_svn
  make_html
  upload
fi

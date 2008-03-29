#/bin/bash

if [ -z "$1" ]
then
  echo "No command-line arguments."
  exit
fi

sed -i -e "s/AM_INIT_AUTOMAKE(\"kmediafactory\", \".*\")/AM_INIT_AUTOMAKE(\"kmediafactory\", \"$1\")/" configure.in.in

make -f Makefile.cvs
./configure
make dist-bz2
make distclean
FILE=`ls --color=none *.tar.bz2`
md5sum $FILE > $FILE.md5
cp $FILE* /home/damu/public_html/software/kmediafactory/

#fi

echo -n "Copy bz2 files to /usr/portage/distfiles (y/N): "
read ans
if [ "$ans" == y -o "$ans" == Y ]
then
  cp /home/damu/public_html/software/kmediafactory/*.tar.bz2 /usr/portage/distfiles/
fi

echo -n "Make ebuilds (y/N): "
read ans
if [ "$ans" == y -o "$ans" == Y ]
then
  cp distros/gentoo/media-video/kmediafactory/*.ebuild /usr/local/portage/media-video/kmediafactory/
  cd /usr/local/portage/media-video/kmediafactory/
  for file in `ls --color=none *.ebuild`; do
    basename=`basename $file`
    ebuild $basename digest
  done
  cd -
  tar cjv -f kmediafactory.ebuild.tar.bz2 -C /usr/local/portage/ media-video/kmediafactory/
  cp kmediafactory.ebuild.tar.bz2 /home/damu/public_html/software/kmediafactory/
fi


echo -n "Copy to susku (y/N): "
read ans
if [ "$ans" == y -o "$ans" == Y ]
then
  echo "put $FILE $FILE.md5 kmediafactory.ebuild.tar.bz2 " | sftp -b \
  - susku.pyhaselka.fi:/home/damu/public_html/software/kmediafactory/
fi
rm $FILE $FILE.md5 kmediafactory.ebuild.tar.bz2

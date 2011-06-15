#!/bin/bash

if svn status | grep "^\?"; then
  echo -n "Add these files (y/N): "
  read ans
  if [ "$ans" == y -o "$ans" == Y ]; then
    svn status | grep "^\?" | awk 'BEGIN { FS=" " }; { system("svn add " $2) }'
  else
    exit
  fi
fi

status=`svn status`
if [ "$status" != "" ]; then
  svn status
  if [ "$1" != "--pause" ]; then
    comment="$1"
  else
    comment="$2"
  fi
  
  if [ "$comment" == "" ]; then
    echo -n "Comment: "
    read comment
  fi
  
  svn commit -m "$comment"
else
  echo "svn is up to date."
fi

echo "Finished."
if [ "$1" == "--pause" ]; then
  read
fi

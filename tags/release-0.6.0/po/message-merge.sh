#! /usr/bin/env bash

for f in *; do
  if test -d "$f"; then
    for pot in *.pot; do
      msgmerge -U "$f/${pot%*.*}.po" "$pot";
    done
  fi
done

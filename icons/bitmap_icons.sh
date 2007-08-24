#!/bin/bash

SIZES=( 16 22 32 48 64 128 )

for ICON in *.svgz; do
  BASE=${ICON:5}
  BASE=${BASE/svgz/png}
  for SIZE in "${SIZES[@]}"; do
    PNG="ox$SIZE-$BASE"
    inkscape --without-gui --file=$ICON --export-png=$PNG \
        --export-width=$SIZE --export-height=$SIZE
  done
done

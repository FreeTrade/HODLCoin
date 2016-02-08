#!/bin/bash
# create multiresolution windows icon
ICON_SRC=../../src/qt/res/icons/hodlcoin.png
ICON_DST=../../src/qt/res/icons/hodlcoin.ico
convert ${ICON_SRC} -resize 16x16 hodlcoin-16.png
convert ${ICON_SRC} -resize 32x32 hodlcoin-32.png
convert ${ICON_SRC} -resize 48x48 hodlcoin-48.png
convert hodlcoin-16.png hodlcoin-32.png hodlcoin-48.png ${ICON_DST}


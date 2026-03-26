#!/bin/bash


URL=https://upinfo.univ-cotedazur.fr/~obaldellon/L2/C/valisp/
OPTIONS="--no-verbose --no-parent  -r -nd"

rm -rf images_test/
rm -f lib_tests.c
rm -f lib_tests.h

wget $OPTIONS -A.img -P images_test/ $URL/images_test/
wget $OPTIONS $URL/lib_tests.c
wget $OPTIONS $URL/lib_tests.h

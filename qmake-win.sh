#!/bin/bash -e
QMAKE="$HOME/Compilers/mxe/usr/i686-w64-mingw32.static/qt5/bin/qmake"
mkdir -p x86
"$QMAKE" rpgtools-ui.pro
make
mv release/rpgtools-ui.exe x86
make clean

QMAKE="$HOME/Compilers/mxe/usr/x86_64-w64-mingw32.static/qt5/bin/qmake"
mkdir -p x86_64
"$QMAKE" rpgtools-ui.pro
make
mv release/rpgtools-ui.exe x86_64
make clean

rm -rf debug release Makefile*
upx -qq x86/*.exe x86_64/*.exe

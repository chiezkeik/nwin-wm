#! /usr/bin/env bash
$EXTRACTRC *.ui >> rc.cpp || exit 11
$XGETTEXT *.cpp -o $podir/kcmnwinscreenedges.pot
rm -f rc.cpp

#!/bin/sh

#  build.sh
#  
#
#  Created by user256874 on 6/16/24.

rm -rf window.app

mkdir -p window.app/Contents/MacOS

clang -WNO-deprecated-declarations -arch -r arm64 -o window.app/Contents/MacOS/Window Window.m -framework foundation -framework Cocoa



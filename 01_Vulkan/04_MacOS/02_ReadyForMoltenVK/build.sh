#!/bin/sh

rm -rf window.app

mkdir -p window.app/Contents/MacOS

clang -Wno-deprecated-declarations -arch arm64 -o window.app/Contents/MacOS/Window Window.m -framework Foundation -framework Cocoa -framework QuartzCore









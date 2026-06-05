#!/bin/sh

rm -rf VK.app

mkdir -p VK.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -arch arm64 -o VK.app/Contents/MacOS/VK VK.mm -framework Foundation -framework Cocoa -framework QuartzCore









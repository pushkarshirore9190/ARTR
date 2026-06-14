#!/bin/sh

rm -rf VK.app

rm -rf log.txt

mkdir -p VK.app/Contents/MacOS

clang++ -Wno-deprecated-declarations -arch arm64 -o VK.app/Contents/MacOS/VK -I$HOME/VulkanSDK/Vulkan/MacOS/include -L$HOME/VulkanSDK/Vulkan/MacOS/lib -F$HOME/VulkanSDK/Vulkan/MacOS/Frameworks -rpath $HOME/VulkanSDK/Vulkan/MacOS/Frameworks VK.mm -framework Foundation -framework Cocoa -framework QuartzCore -framework vulkan











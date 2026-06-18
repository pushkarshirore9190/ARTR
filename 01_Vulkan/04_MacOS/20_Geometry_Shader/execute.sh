#!/bin/sh

VULKAN_MAC_SDK="$HOME/VULKANSDK/VULKAN"

export VK_ICD_FILENAMES="$VULKAN_MAC_SDK/macOS/share/vulkan/icd.d/MoltenVK_icd.json"

export VK_LAYER_PATH="$VULKAN_MAC_SDK/macOS/share/vulkan/explicit_layer.d"

./VK.app/Contents/MacOS/VK






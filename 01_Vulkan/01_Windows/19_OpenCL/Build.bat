cl.exe /c /EHsc /I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.2\include" /I C:\VulkanSDK\Vulkan\Include VK.cpp
rc.exe VK.rc
link.exe VK.obj VK.res /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.2\lib\x64" /LIBPATH:C:\VulkanSDK\Vulkan\Lib user32.lib gdi32.lib /SUBSYSTEM:WINDOWS
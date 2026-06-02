cls

cl.exe /c /EHsc /I C:\VulkanSDK\Vulkan\include VK.cpp 

rc.exe VK.rc

link.exe VK.obj VK.res /LIBPATH:C:\VulkanSDK\Vulkan\lib user32.lib gdi32.lib /SUBSYSTEM:WINDOWS































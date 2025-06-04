cls

cl.exe /MD /std:c++17 /c /EHsc /I C:\VulkanSDK\Vulkan\include /I C:\VulkanSDK\Vulkan\include\glslang VK.cpp 

rc.exe VK.rc

link.exe VK.obj VK.res /LIBPATH:C:\VulkanSDK\Vulkan\lib user32.lib gdi32.lib /SUBSYSTEM:WINDOWS



































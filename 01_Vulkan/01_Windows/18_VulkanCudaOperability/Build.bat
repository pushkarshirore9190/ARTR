
cls

rc.exe VK.rc

nvcc.exe -I C:\VulkanSDK\Vulkan\Include -L C:\VulkanSDK\Vulkan\Lib -o Vk.exe user32.lib gdi32.lib Vk.res Vk.cu -Xcudafe "--diag_suppress=20012,--diag_suppress=20013,--diag_suppress=20014,--diag_suppress=20015"




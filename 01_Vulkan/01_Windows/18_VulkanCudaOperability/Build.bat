cls

rc.exe VK.rc

nvcc.exe -I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.2\include" -I "C:\VulkanSDK\Vulkan\include" -L "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.2\lib\x64" -L "C:\VulkanSDK\Vulkan\lib" -o VK.exe user32.lib gdi32.lib VK.res VK.cu -diag-suppress 20012 -diag-suppress 20013 -diag-suppress 20014 -diag-suppress 20015








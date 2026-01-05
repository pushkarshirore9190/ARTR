cls

rc.exe VK.rc

nvcc.exe -arch=sm_120 -m64 -D__CUDACC_VER_MAJOR__=12 -I"C:\VulkanSDK\Vulkan\include" -L"C:\VulkanSDK\Vulkan\lib" VK.cu -o VK.exe vulkan-1.lib user32.lib gdi32.lib VK.res --diag-suppress 20012,20014 --expt-relaxed-constexpr -allow-unsupported-compiler



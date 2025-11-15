
gcc -c VK.c -o VK.o \
    -I$HOME/VulkanSDK/Vulkan/1.4.304.0/x86_64/include



gcc VK.o -o VK \
    -L$HOME/VulkanSDK/Vulkan/1.4.304.0/x86_64/lib \
    -lvulkan -lX11 -lm








g++ -c VK.cpp -o VK.o \
    -I$HOME/VulkanSDK/Vulkan/1.4.304.0/x86_64/include


g++ VK.o -o VK \
    -L$HOME/VulkanSDK/Vulkan/1.4.304.0/x86_64/lib \
    -lvulkan -lX11 -lm




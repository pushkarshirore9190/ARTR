g++ -c VK.cpp -o VK.o \
    -std=c++17 \
    -I$HOME/VulkanSDK/Vulkan/1.4.304.0/x86_64/include \
    -I$HOME/VulkanSDK/Vulkan/1.4.304.0/x86_64/include/glslang \
    -I$HOME/VulkanSDK/Vulkan/1.4.304.0/x86_64/include/SPIRV

g++ VK.o -o VK \
    -L$HOME/VulkanSDK/Vulkan/1.4.304.0/x86_64/lib \
    -lvulkan -lX11 -lm \
    -lglslang -lSPIRV -lglslang-default-resource-limits




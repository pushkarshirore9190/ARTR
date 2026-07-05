rm -f vk.o vk

# Compile
g++ -c VK.cpp -o vk.o \
    -std=c++17 \
    -DVK_USE_PLATFORM_XLIB_KHR \
    -I$HOME/VulkanSDK/vulkan/x86_64/include

# Link
g++ -o vk vk.o \
    -L$HOME/VulkanSDK/vulkan/x86_64/lib \
    -lvulkan \
    -lOpenCL \
    -lX11 \
    -lm \
    -ldl \
    -lpthread
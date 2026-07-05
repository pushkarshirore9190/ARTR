
# # g++ -c VK.cpp -o VK.o \
# #     -I$HOME/VulkanSDK/Vulkan/1.4.304.0/x86_64/include

# g++ -c -I $HOME/VulkanSDK/vulkan/x86_64/include -o vk.o VK.cpp

# # g++ VK.o -o VK \
# #     -L$HOME/VulkanSDK/Vulkan/1.4.304.0/x86_64/lib \
# #     -lvulkan -lX11 -lm

# g++ -o vk vk.o -lX11 -lm -L $HOME/VulkanSDK/vulkan/x86_64/lib -lvulkan

# nvcc -c VK.cu -o vk.o \
#     -ccbin clang-14 \
#     -I$HOME/VulkanSDK/vulkan/x86_64/include \
#     -std=c++17 \
#     -x cu


# nvcc -o vk vk.o \
#     -ccbin clang-14 \
#     -lX11 \
#     -lm \
#     -lvulkan \
#     -lcudart \
#     -L$HOME/VulkanSDK/vulkan/x86_64/lib


rm -f vk.o

# Compile
nvcc -c VK.cu -o vk.o \
    -ccbin clang-14 \
    -DVK_USE_PLATFORM_XLIB_KHR \
    -std=c++17 \
    -x cu \
    --expt-relaxed-constexpr \
    -diag-suppress 20013 \
    -diag-suppress 177 \
    -diag-suppress 550 \
    -w

# Link
nvcc -o vk vk.o \
    -ccbin clang-14 \
    -lX11 \
    -lm \
    -lvulkan \
    -lcudart \
    -lstdc++
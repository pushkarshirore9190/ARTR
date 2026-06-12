
-(VkResult) createVulkanInstance
{

    // variable declarations
    VkResult vkresult = VK_SUCCESS;

    // code
    vkresult = [self fillExtensionNames];
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createVulkanInstance() : fillExtensionNames function failed\n");
        return(vkresult);
    }
    else
    {
        fprintf(gpFile, "createVulkanInstance() : fillExtensionNames() succeeded\n");
    }

    // Fill validation layers
    if (bValidation == YES)
    {
        vkresult = [self fillValidationLayerNames];
        if (vkresult != VK_SUCCESS)
        {
            fprintf(gpFile, "createVulkanInstance() : fillValidationLayerNames() function failed (VkResult: %d)\n", vkresult);
            return vkresult;
        }
        else
        {
            fprintf(gpFile, "createVulkanInstance() : fillValidationLayerNames() succeeded\n");
        }
    }


    // step : 2 initlalise struct VkApplicationInfo

    VkApplicationInfo vkApplicationInfo;
    memset((void*)&vkApplicationInfo, 0, sizeof(VkApplicationInfo));

    vkApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vkApplicationInfo.pNext = NULL;
    vkApplicationInfo.pApplicationName = gpszAppName;
    vkApplicationInfo.applicationVersion = 1;
    vkApplicationInfo.pEngineName = gpszAppName;
    vkApplicationInfo.engineVersion = 1;
    vkApplicationInfo.apiVersion = VK_API_VERSION_1_3;


    // step 3 : Initialize struct VkInstanceCreateInfo
    VkInstanceCreateInfo vkInstanceCreateInfo;
    memset((void*)&vkInstanceCreateInfo, 0, sizeof(VkInstanceCreateInfo));

    vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;  // Corrected the structure type
    vkInstanceCreateInfo.pNext = NULL;
    vkInstanceCreateInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;  // for MoltenVK to avoid VK_ERROR_INCOMPATIBLE_DRIVER error
    vkInstanceCreateInfo.pApplicationInfo = &vkApplicationInfo;
    vkInstanceCreateInfo.enabledExtensionCount = enabledInstanceExtensionCount;
    vkInstanceCreateInfo.ppEnabledExtensionNames = enabledInstanceExtensionNames_array;  // Corrected the field name

    if (bValidation == YES)
    {
        vkInstanceCreateInfo.enabledLayerCount = enabledValidationLayerCount;
        vkInstanceCreateInfo.ppEnabledLayerNames = enabledValidationLayerNames_Array;
    }
    else
    {
        vkInstanceCreateInfo.enabledLayerCount = 0;
        vkInstanceCreateInfo.ppEnabledLayerNames = NULL;
    }


    // step : 4 create instance by calling vkCreateInstance
    vkresult = vkCreateInstance(&vkInstanceCreateInfo, NULL, &vkInstance);

    if (vkresult == VK_ERROR_INCOMPATIBLE_DRIVER)
    {
        fprintf(gpFile, "createVulkanInstance() : vkCreateInstance() function failed due to incompatible driver (%d)\n", vkresult);
        return vkresult;
    }
    else if (vkresult == VK_ERROR_EXTENSION_NOT_PRESENT)
    {
        fprintf(gpFile, "createVulkanInstance() : vkCreateInstance() function failed due to required extension not present (%d)\n", vkresult);
        return vkresult;
    }
    else if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createVulkanInstance() : vkCreateInstance() function failed due to unknown reason (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createVulkanInstance() : vkCreateInstance() function succeeded\n");
    }

    // do for validationcallback
    if (bValidation == YES)
    {
        vkresult = [self createValidationCallbackfuntion];
        if (vkresult != VK_SUCCESS)
        {
            fprintf(gpFile, "createVulkanInstance() : createValidationCallbackfuntion() function failed (VkResult: %d)\n", vkresult);
            return vkresult;
        }
        else
        {
            fprintf(gpFile, "createVulkanInstance() : createValidationCallbackfuntion() succeeded\n");
        }
    }

    // Return after completing the loop
    return vkresult;
}


-(VkResult) fillExtensionNames
{
    // variable declarations
    VkResult vkresult = VK_SUCCESS;

    // step - 1: How many instance extensions are supported by the Vulkan driver of this version
    uint32_t instanceExtensionCount = 0;

    // Query how many extensions are supported
    vkresult = vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, NULL);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "fillExtensionNames() : vkEnumerateInstanceExtensionProperties function 1st call failed with error code %d\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "fillExtensionNames() : vkEnumerateInstanceExtensionProperties() 1st call succeeded. Extension count: %d\n", instanceExtensionCount);
    }

    // step 2: Allocate memory for extensions
    VkExtensionProperties* vkExtensionProperties_array = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * instanceExtensionCount);
    if (!vkExtensionProperties_array)
    {
        fprintf(gpFile, "fillExtensionNames() : Memory allocation failed for VkExtensionProperties_array\n");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    // Query extension properties
    vkresult = vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, vkExtensionProperties_array);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "fillExtensionNames() : vkEnumerateInstanceExtensionProperties function 2nd call failed with error code %d\n", vkresult);
        free(vkExtensionProperties_array);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "fillExtensionNames() : vkEnumerateInstanceExtensionProperties() 2nd call succeeded\n");
    }

    // step 3: Fill string array by names of extensions obtained from VkExtension array
    char** InstanceExtensionNames_array = (char**)malloc(sizeof(char*) * instanceExtensionCount);
    if (!InstanceExtensionNames_array)
    {
        fprintf(gpFile, "fillExtensionNames() : Memory allocation failed for InstanceExtensionNames_array\n");
        free(vkExtensionProperties_array);
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (uint32_t i = 0; i < instanceExtensionCount; i++)
    {
        InstanceExtensionNames_array[i] = (char*)malloc(sizeof(char) * strlen(vkExtensionProperties_array[i].extensionName) + 1);
        memcpy(InstanceExtensionNames_array[i], vkExtensionProperties_array[i].extensionName, strlen(vkExtensionProperties_array[i].extensionName) + 1);
        fprintf(gpFile, "fillExtensionNames() : Vulkan Extension name = %s\n", InstanceExtensionNames_array[i]);
    }

    free(vkExtensionProperties_array);

    // step 4: Check for required extensions
    VkBool32 vulkunSurfaceExtensionFound = VK_FALSE;
    VkBool32 macOSSurfaceExtensionFound = VK_FALSE;
    VkBool32 debugReportExtensionFound = VK_FALSE;
    VkBool32 vulkanPortabilityEnumerationExtensionFound = VK_FALSE; // 1.2.31.0

    for (uint32_t i = 0; i < instanceExtensionCount; i++)
    {
        if (strcmp(InstanceExtensionNames_array[i], VK_KHR_SURFACE_EXTENSION_NAME) == 0)
        {
            vulkunSurfaceExtensionFound = VK_TRUE;
            enabledInstanceExtensionNames_array[enabledInstanceExtensionCount++] = VK_KHR_SURFACE_EXTENSION_NAME;
        }

        // VK_MVK_MACOS_SURFACE_EXTENSION_NAME

        if (strcmp(InstanceExtensionNames_array[i], VK_MVK_MACOS_SURFACE_EXTENSION_NAME) == 0)
        {
            macOSSurfaceExtensionFound = VK_TRUE;
            enabledInstanceExtensionNames_array[enabledInstanceExtensionCount++] = VK_MVK_MACOS_SURFACE_EXTENSION_NAME;
        }

        if (strcmp(InstanceExtensionNames_array[i], VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0)
        {
            vulkanPortabilityEnumerationExtensionFound = VK_TRUE;
            enabledInstanceExtensionNames_array[enabledInstanceExtensionCount++] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
        }

        if (strcmp(InstanceExtensionNames_array[i], VK_EXT_DEBUG_REPORT_EXTENSION_NAME) == 0)
        {
            debugReportExtensionFound = VK_TRUE;

            if (bValidation == YES)
            {
                enabledInstanceExtensionNames_array[enabledInstanceExtensionCount++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
            }
            else
            {
                // array will not have entry of VK_EXT_DEBUG_REPORT_EXTENSION_NAME
            }
        }

    }

    // step 5: Free the allocated memory
    for (uint32_t i = 0; i < instanceExtensionCount; i++)
    {
        free(InstanceExtensionNames_array[i]);
    }

    free(InstanceExtensionNames_array);

    // step 6: Check and print the required extension names
    if (vulkunSurfaceExtensionFound == VK_FALSE)
    {
        vkresult = VK_ERROR_INITIALIZATION_FAILED; // return hardcoded failure
        fprintf(gpFile, "fillExtensionNames() : VK_KHR_SURFACE_EXTENSION_NAME not found\n");
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "fillExtensionNames() : VK_KHR_SURFACE_EXTENSION_NAME found\n");
    }

    if (macOSSurfaceExtensionFound == VK_FALSE)
    {
        vkresult = VK_ERROR_INITIALIZATION_FAILED; // return hardcoded failure
        fprintf(gpFile, "fillExtensionNames() : VK_MVK_MACOS_SURFACE_EXTENSION_NAME not found\n");
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "fillExtensionNames() : VK_MVK_MACOS_SURFACE_EXTENSION_NAME found\n");
    }

    if (vulkanPortabilityEnumerationExtensionFound == VK_FALSE)
    {
        vkresult = VK_ERROR_INITIALIZATION_FAILED; // return hardcoded failure
        fprintf(gpFile, "fillExtensionNames() : VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME not found\n");
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "fillExtensionNames() : VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME found\n");
    }


    if (vulkunSurfaceExtensionFound == VK_FALSE)
    {
        if (bValidation == YES)
        {
            vkresult = VK_ERROR_INITIALIZATION_FAILED; // return hardcoded failure
            fprintf(gpFile, "fillExtensionNames() : validation is on but required  VK_EXT_DEBUG_REPORT_EXTENSION_NAME not found\n");
            return vkresult;
        }
        else
        {
            fprintf(gpFile, "fillExtensionNames() :  validation is off and VK_EXT_DEBUG_REPORT_EXTENSION_NAME is not suppoerted found\n");
        }
    }
    else
    {
        if (bValidation == YES)
        {
            fprintf(gpFile, "fillExtensionNames() : validation is on and  VK_EXT_DEBUG_REPORT_EXTENSION_NAME is also found\n");
        }
        else
        {
            fprintf(gpFile, "fillExtensionNames() : validation is off and VK_EXT_DEBUG_REPORT_EXTENSION_NAME is suppoerted found\n");
        }
    }

    // Step 7: Print only enabled extension names
    for (uint32_t i = 0; i < enabledInstanceExtensionCount; i++)
    {
        fprintf(gpFile, "fillExtensionNames() : enabled Vulkan extension name = %s\n", enabledInstanceExtensionNames_array[i]);
    }

    // Return after completing the loop
    return vkresult;
}


-(VkResult) getSupportedSurface
{
    // code

    VkResult vkresult = VK_SUCCESS;

    VkMacOSSurfaceCreateInfoMVK vkMacOSSurfaceCreateInfoMVK;
    memset((void*)&vkMacOSSurfaceCreateInfoMVK, 0, sizeof(VkMacOSSurfaceCreateInfoMVK));

    vkMacOSSurfaceCreateInfoMVK.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    vkMacOSSurfaceCreateInfoMVK.pNext = NULL;
    vkMacOSSurfaceCreateInfoMVK.flags = 0;
    vkMacOSSurfaceCreateInfoMVK.pView = gpView;

    vkresult = vkCreateMacOSSurfaceMVK(vkInstance, &vkMacOSSurfaceCreateInfoMVK, NULL, &vkSurfaceKHR);
    if(vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "getSupportedSurface() : vkCreateMacOSSurfaceMVK Failed\n");
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "getSupportedSurface() : vkCreateMacOSSurfaceMVK succeded \n");
    }

    return vkresult;

}


-(VkResult) fillDeviceExtensionNames
{
    // Variable declarations
    VkResult vkresult = VK_SUCCESS;

    // Step 1: Query how many device extensions are supported
    uint32_t deviceExtensionCount = 0;
    vkresult = vkEnumerateDeviceExtensionProperties(vkPhysicalDevice_selected, NULL, &deviceExtensionCount, NULL);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "fillDeviceExtensionNames(): vkEnumerateDeviceExtensionProperties() 1st call failed with error code %d\n", vkresult);
        return vkresult;
    }
    fprintf(gpFile, "fillDeviceExtensionNames(): vkEnumerateDeviceExtensionProperties() 1st call succeeded. Device extension count: %d\n", deviceExtensionCount);

    // Step 2: Allocate memory for extension properties
    VkExtensionProperties* vkExtensionProperties_array = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * deviceExtensionCount);
    if (!vkExtensionProperties_array)
    {
        fprintf(gpFile, "fillDeviceExtensionNames(): Memory allocation failed for vkExtensionProperties_array\n");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    // Query extension properties
    vkresult = vkEnumerateDeviceExtensionProperties(vkPhysicalDevice_selected, NULL, &deviceExtensionCount, vkExtensionProperties_array);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "fillDeviceExtensionNames(): vkEnumerateDeviceExtensionProperties() 2nd call failed with error code %d\n", vkresult);
        free(vkExtensionProperties_array);
        return vkresult;
    }
    fprintf(gpFile, "fillDeviceExtensionNames(): vkEnumerateDeviceExtensionProperties() 2nd call succeeded\n");

    // Step 3: Allocate memory for extension name strings
    char** DeviceExtensionNames_array = (char**)malloc(sizeof(char*) * deviceExtensionCount);
    if (!DeviceExtensionNames_array)
    {
        fprintf(gpFile, "fillDeviceExtensionNames(): Memory allocation failed for DeviceExtensionNames_array\n");
        free(vkExtensionProperties_array);
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (uint32_t i = 0; i < deviceExtensionCount; i++)
    {
        DeviceExtensionNames_array[i] = (char*)malloc(strlen(vkExtensionProperties_array[i].extensionName) + 1);
        if (!DeviceExtensionNames_array[i])
        {
            fprintf(gpFile, "fillDeviceExtensionNames(): Memory allocation failed for DeviceExtensionNames_array[%d]\n", i);
            // Free previously allocated memory before returning
            for (uint32_t j = 0; j < i; j++)
                free(DeviceExtensionNames_array[j]);
            free(DeviceExtensionNames_array);
            free(vkExtensionProperties_array);
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }

        strcpy(DeviceExtensionNames_array[i], vkExtensionProperties_array[i].extensionName);
        fprintf(gpFile, "fillDeviceExtensionNames(): Vulkan Device Extension = %s\n", DeviceExtensionNames_array[i]);
    }

    free(vkExtensionProperties_array); // Free extension properties after copying names

    // Step 4: Check for required extensions
    VkBool32 vulkanSwapchainExtensionFound = VK_FALSE;
    VkBool32 vulkanPortabilitySubsetExtensionFound = VK_FALSE; 

    for (uint32_t i = 0; i < deviceExtensionCount; i++)
    {
        if (strcmp(DeviceExtensionNames_array[i], VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
        {
            vulkanSwapchainExtensionFound = VK_TRUE;
            enabledDeviceExtensionNames_array[enableDeviceExtensionCount++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
        }

        // Check for VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
        if (strcmp(DeviceExtensionNames_array[i], VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME) == 0)
        {
            vulkanPortabilitySubsetExtensionFound = VK_TRUE;
            enabledDeviceExtensionNames_array[enableDeviceExtensionCount++] = VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME;
        }
    }

    // Step 5: Free the allocated memory for extension names
    for (uint32_t i = 0; i < deviceExtensionCount; i++)
    {
        free(DeviceExtensionNames_array[i]);
    }
    free(DeviceExtensionNames_array);

    // Step 6: Check if the required extension is found
    if (vulkanSwapchainExtensionFound == VK_FALSE)
    {
        vkresult = VK_ERROR_INITIALIZATION_FAILED; // Return hardcoded failure
        fprintf(gpFile, "fillDeviceExtensionNames(): VK_KHR_SWAPCHAIN_EXTENSION_NAME not found\n");
        return vkresult;
    }
    fprintf(gpFile, "fillDeviceExtensionNames(): VK_KHR_SWAPCHAIN_EXTENSION_NAME found\n");

    if (vulkanPortabilitySubsetExtensionFound == VK_FALSE)
    {
        vkresult = VK_ERROR_INITIALIZATION_FAILED; // Return hardcoded failure
        fprintf(gpFile, "fillDeviceExtensionNames(): VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME not found\n");
        return vkresult;
    }
    fprintf(gpFile, "fillDeviceExtensionNames(): VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME found\n");


    // Step 7: Print enabled extension names
    for (uint32_t i = 0; i < enableDeviceExtensionCount; i++)
    {
        fprintf(gpFile, "fillDeviceExtensionNames(): Enabled Vulkan Device Extension = %s\n", enabledDeviceExtensionNames_array[i]);
    }

    return vkresult;
}


-(VkResult) createSwapchain:(VkBool32)vsync
{
    // variable declaration
    VkResult vkresult = VK_SUCCESS;

    // code
    vkresult = [self getPhysicalDeviceSurfaceFormatAndColorSpace];
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createSwapchain() : getPhysicalDeviceSurfaceFormatAndColorSpace() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        fprintf(gpFile, "createSwapchain() : getPhysicalDeviceSurfaceFormatAndColorSpace() succeeded\n");
    }

    // step 2 : get physical device surface capabilities
    VkSurfaceCapabilitiesKHR vkSurfaceCapabilitiesKHR;
    memset((void*)&vkSurfaceCapabilitiesKHR, 0, sizeof(VkSurfaceCapabilitiesKHR));

    vkresult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice_selected, vkSurfaceKHR, &vkSurfaceCapabilitiesKHR);

    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createSwapchain() : vkGetPhysicalDeviceSurfaceCapabilitiesKHR() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        fprintf(gpFile, "createSwapchain() : vkGetPhysicalDeviceSurfaceCapabilitiesKHR() succeeded\n");
    }

    // step 3 : find out desired number of swapchain images
    uint32_t testingNoofSwapchainImages = vkSurfaceCapabilitiesKHR.minImageCount + 1;
    uint32_t desiredNoofSwapchainImages = 0;

    if (vkSurfaceCapabilitiesKHR.maxImageCount > 0 && vkSurfaceCapabilitiesKHR.maxImageCount < testingNoofSwapchainImages)
    {
        desiredNoofSwapchainImages = vkSurfaceCapabilitiesKHR.maxImageCount;
        fprintf(gpFile, "\nmaxImageCount\n");
    }
    else
    {
        desiredNoofSwapchainImages = vkSurfaceCapabilitiesKHR.minImageCount;
        fprintf(gpFile, "\nminImageCount\n");
    }

    fprintf(gpFile, "\n\n\n%d %d %d !!!\n", vkSurfaceCapabilitiesKHR.maxImageCount, vkSurfaceCapabilitiesKHR.minImageCount, desiredNoofSwapchainImages);



    // step 4: choose size of the swapchain image
    memset((void*)&vkExtent2D_Swapchain, 0, sizeof(vkExtent2D_Swapchain));

    if (vkSurfaceCapabilitiesKHR.currentExtent.width != UINT32_MAX)
    {
        vkExtent2D_Swapchain.width = vkSurfaceCapabilitiesKHR.currentExtent.width;
        vkExtent2D_Swapchain.height = vkSurfaceCapabilitiesKHR.currentExtent.height;

        fprintf(gpFile, "createSwapchain() : swapchain image width = %d height = %d\n", vkExtent2D_Swapchain.width, vkExtent2D_Swapchain.height);
    }
    else
    {
        // if surface size is undefined, set it manually
        VkExtent2D vkExtent2D;
        memset((void*)&vkExtent2D, 0, sizeof(VkExtent2D));

        vkExtent2D.width = (uint32_t)winWidth;
        vkExtent2D.height = (uint32_t)winHeight;

        vkExtent2D_Swapchain.width = glm::max(vkSurfaceCapabilitiesKHR.minImageExtent.width, glm::min(vkSurfaceCapabilitiesKHR.maxImageExtent.width, vkExtent2D.width));
        vkExtent2D_Swapchain.height = glm::max(vkSurfaceCapabilitiesKHR.minImageExtent.height, glm::min(vkSurfaceCapabilitiesKHR.maxImageExtent.height, vkExtent2D.height));

        fprintf(gpFile, "createSwapchain() : swapchain image width = %d height = %d\n", vkExtent2D_Swapchain.width, vkExtent2D_Swapchain.height);
    }

    // step 5 : set swapchain image usage flag
    VkImageUsageFlags vkImageUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    // step 6 : whether to consider pretransform or not
    VkSurfaceTransformFlagBitsKHR vkSurfaceTransformFlagBitsKHR;

    if (vkSurfaceCapabilitiesKHR.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        vkSurfaceTransformFlagBitsKHR = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else
    {
        vkSurfaceTransformFlagBitsKHR = vkSurfaceCapabilitiesKHR.currentTransform;
    }

    // step 7 : get presentation mode
    vkresult = [self getPhysicalDevicePresentMode];
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createSwapchain() : getPhysicalDevicePresentMode() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        fprintf(gpFile, "createSwapchain() : getPhysicalDevicePresentMode() succeeded\n");
    }

    // step 8: initialize VkSwapchainCreateInfoKHR structure
    VkSwapchainCreateInfoKHR vkSwapchainCreateInfoKHR;
    memset((void*)&vkSwapchainCreateInfoKHR, 0, sizeof(VkSwapchainCreateInfoKHR));

    vkSwapchainCreateInfoKHR.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    vkSwapchainCreateInfoKHR.pNext = NULL;
    vkSwapchainCreateInfoKHR.flags = 0;
    vkSwapchainCreateInfoKHR.surface = vkSurfaceKHR;
    vkSwapchainCreateInfoKHR.minImageCount = desiredNoofSwapchainImages;
    vkSwapchainCreateInfoKHR.imageFormat = vkFormat_color;
    vkSwapchainCreateInfoKHR.imageColorSpace = vkColorSpaceKHR;
    vkSwapchainCreateInfoKHR.imageExtent.width = vkExtent2D_Swapchain.width;
    vkSwapchainCreateInfoKHR.imageExtent.height = vkExtent2D_Swapchain.height;
    vkSwapchainCreateInfoKHR.imageUsage = vkImageUsageFlags;
    vkSwapchainCreateInfoKHR.preTransform = vkSurfaceTransformFlagBitsKHR;
    vkSwapchainCreateInfoKHR.imageArrayLayers = 1;
    vkSwapchainCreateInfoKHR.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkSwapchainCreateInfoKHR.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    vkSwapchainCreateInfoKHR.presentMode = vkPresentModeKHR;
    vkSwapchainCreateInfoKHR.clipped = VK_TRUE;

    // step 9 : call the function
    vkresult = vkCreateSwapchainKHR(vkDevice, &vkSwapchainCreateInfoKHR, NULL, &vkSwapchainKHR);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createSwapchain() : vkCreateSwapchainKHR() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        fprintf(gpFile, "createSwapchain() : vkCreateSwapchainKHR() succeeded\n");
    }

    return vkresult;
}


-(VkResult) createImagesAndImageViews
{

    // variable declaration
    VkResult vkresult = VK_SUCCESS;

    // get swapchain image count
    vkresult = vkGetSwapchainImagesKHR(vkDevice, vkSwapchainKHR, &swapchainImageCount, NULL);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createImagesAndImageViews() 1st call : vkGetSwapchainImagesKHR() function failed (%d)\n", vkresult);
        return vkresult;
    }

    else if (swapchainImageCount == 0)
    {
        fprintf(gpFile, "createImagesAndImageViews() 1st call : swapchain image count is zero, returning hardcoded error value\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    else
    {
        fprintf(gpFile, "createImagesAndImageViews() 1st call : this func is giving the swapchain image count = %d\n", swapchainImageCount);
    }

    //// allocate the swapchain image array
    SwapchainImage_Array = (VkImage*)malloc(sizeof(VkImage) * swapchainImageCount);

    //// fill this array with swapchain images
    vkresult = vkGetSwapchainImagesKHR(vkDevice, vkSwapchainKHR, &swapchainImageCount, SwapchainImage_Array);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createImagesAndImageViews() 2nd call : vkGetSwapchainImagesKHR() function failed (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createImagesAndImageViews() 2nd call : vkGetSwapchainImagesKHR() succeeded\n");
    }

    //// allocate array of swapchain image views
    SwapchainImageView_Array = (VkImageView*)malloc(sizeof(VkImageView) * swapchainImageCount);

    //// initialize VkImageViewCreateInfo Structure
    VkImageViewCreateInfo vkImageViewCreateInfo;
    memset(&vkImageViewCreateInfo, 0, sizeof(VkImageViewCreateInfo));

    vkImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    vkImageViewCreateInfo.pNext = NULL;
    vkImageViewCreateInfo.flags = 0;
    vkImageViewCreateInfo.format = vkFormat_color;
    vkImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    vkImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    vkImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    vkImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    vkImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    vkImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    vkImageViewCreateInfo.subresourceRange.levelCount = 1;
    vkImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    vkImageViewCreateInfo.subresourceRange.layerCount = 1;
    vkImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

    //// now fill image view array using above struct
    for (uint32_t i = 0; i < swapchainImageCount; i++)
    {
        vkImageViewCreateInfo.image = SwapchainImage_Array[i];

        vkresult = vkCreateImageView(vkDevice, &vkImageViewCreateInfo, NULL, &SwapchainImageView_Array[i]);
        if (vkresult != VK_SUCCESS)
        {
            fprintf(gpFile, "createImagesAndImageViews() : vkCreateImageView() function failed for iteration (%d).(%d)\n", i, vkresult);
            return vkresult;
        }
        else
        {
            fprintf(gpFile, "createImagesAndImageViews() : vkCreateImageView() : succeeded for iteration (%d)\n", i);
        }
    }

    // for depth image

    vkresult = [self getSupportedDepthFormat];
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createImagesAndImageViews() : getSupportedDepthFormat() function failed for iteration (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createImagesAndImageViews() : getSupportedDepthFormat() succeeded for iteration\n");
    }

    // for depth image initialise vkImageCreateInfo
    VkImageCreateInfo vkImageCreateInfo;
    memset(&vkImageCreateInfo, 0, sizeof(VkImageCreateInfo));

    vkImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    vkImageCreateInfo.pNext = NULL;
    vkImageCreateInfo.flags = 0;
    vkImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;  // 1D, 2D, or 3D image
    vkImageCreateInfo.format = vkFormat_Depth;  // Format of image data
    // vkImageCreateInfo.extent.width = winWidth;  // Image width
    //vkImageCreateInfo.extent.height = winHeight; // Image height
    vkImageCreateInfo.extent.width = vkExtent2D_Swapchain.width;
    vkImageCreateInfo.extent.height = vkExtent2D_Swapchain.height;
    vkImageCreateInfo.extent.depth = 1;     // For 2D image, depth is 1
    vkImageCreateInfo.mipLevels = 1;        // Number of mipmap levels
    vkImageCreateInfo.arrayLayers = 1;      // Number of array layers
    vkImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;  // No multisampling
    vkImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL; // Or VK_IMAGE_TILING_LINEAR
    vkImageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    vkresult = vkCreateImage(vkDevice, &vkImageCreateInfo, NULL, &vkImage_Depth);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createImagesAndImageViews() : vkCreateImage() function failed for iteration (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createImagesAndImageViews() : vkCreateImage() succeeded for iteration\n");
    }

    // memory reqirement for depth image
    VkMemoryRequirements vkMemoryRequirements;
    memset((void*)&vkMemoryRequirements, 0, sizeof(VkMemoryRequirements));

    vkGetImageMemoryRequirements(vkDevice, vkImage_Depth, &vkMemoryRequirements);

    VkMemoryAllocateInfo vkMemoryAllocateInfo;
    memset((void*)&vkMemoryAllocateInfo, 0, sizeof(VkMemoryAllocateInfo));

    vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    vkMemoryAllocateInfo.pNext = NULL;
    vkMemoryAllocateInfo.allocationSize = vkMemoryRequirements.size;
    vkMemoryAllocateInfo.memoryTypeIndex = 0; // initial value before entering into loop

    VkBool32 foundMatchingMemoryType_Depth = VK_FALSE;

    for (uint32_t i = 0; i < vkPhysicalDeviceMemoryProperties.memoryTypeCount; i++)
    {
        if ((vkMemoryRequirements.memoryTypeBits & 1) == 1)
        {
            if (vkPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
            {
                vkMemoryAllocateInfo.memoryTypeIndex = i;
                foundMatchingMemoryType_Depth = VK_TRUE;
                break;
            }
        }

        vkMemoryRequirements.memoryTypeBits >>= 1;

    }

    if (foundMatchingMemoryType_Depth == VK_FALSE)
    {
        vkresult = VK_ERROR_OUT_OF_DEVICE_MEMORY; 
        fprintf(gpFile, "createImagesAndImageViews() : Failed to find suitable memory type for depth image\n");
        return vkresult; // Return hardcoded error code
    }
    else
    {
        fprintf(gpFile, "createImagesAndImageViews() : Suitable memory type found for depth image at index %d\n", vkMemoryAllocateInfo.memoryTypeIndex);
    }

    vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, NULL, &vkDeviceMemory_Depth);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createImagesAndImageViews() : vkAllocateMemory() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createImagesAndImageViews() : vkAllocateMemory() succeeded.\n");
    }

    vkresult = vkBindImageMemory(vkDevice, vkImage_Depth, vkDeviceMemory_Depth, 0);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createImagesAndImageViews() : vkBindImageMemory() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createImagesAndImageViews() : vkBindImageMemory() succeeded.\n");
    }

    // crateImageView For above image view
    memset(&vkImageViewCreateInfo, 0, sizeof(VkImageViewCreateInfo));

    vkImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    vkImageViewCreateInfo.pNext = NULL;
    vkImageViewCreateInfo.flags = 0;
    vkImageViewCreateInfo.format = vkFormat_Depth;
    vkImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    if(vkFormat_Depth == VK_FORMAT_D32_SFLOAT_S8_UINT || vkFormat_Depth == VK_FORMAT_D24_UNORM_S8_UINT || vkFormat_Depth == VK_FORMAT_D16_UNORM_S8_UINT)
    {
        vkImageViewCreateInfo.subresourceRange.aspectMask = vkImageViewCreateInfo.subresourceRange.aspectMask | VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    vkImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    vkImageViewCreateInfo.subresourceRange.levelCount = 1;
    vkImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    vkImageViewCreateInfo.subresourceRange.layerCount = 1;
    vkImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    vkImageViewCreateInfo.image = vkImage_Depth;

    vkresult = vkCreateImageView(vkDevice, &vkImageViewCreateInfo, NULL, &vkImageView_Depth);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createImagesAndImageViews() : VkCreateImageView() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createImagesAndImageViews() : VkCreateImageView() succeeded.\n");
    }


    return vkresult;
}


-(VkResult) createVertexBuffer
{

    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    float traingle_Position[] =
    {
        0.0f,1.0f,0.0f,
        -1.0f,-1.0f,0.0f,
        1.0f,-1.0f,0.0f
    };

    memset((void*)&vertexData_Position, 0, sizeof(VertexData));

    VkBufferCreateInfo vkBufferCreateInfo;
    memset((void*)& vkBufferCreateInfo, 0, sizeof(VkBufferCreateInfo));

    vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vkBufferCreateInfo.pNext = NULL;
    vkBufferCreateInfo.flags = 0;
    vkBufferCreateInfo.size = sizeof(traingle_Position);
    vkBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    vkresult = vkCreateBuffer(vkDevice, &vkBufferCreateInfo, NULL, &vertexData_Position.vkBuffer);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createVertexBuffer() : vkCreateBuffer() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createVertexBuffer() : vkCreateBuffer() succeeded.\n");
    }

    VkMemoryRequirements vkMemoryRequirements;
    memset((void*)&vkMemoryRequirements, 0, sizeof(VkMemoryRequirements));

    vkGetBufferMemoryRequirements(vkDevice, vertexData_Position.vkBuffer, &vkMemoryRequirements);

    VkMemoryAllocateInfo vkMemoryAllocateInfo;
    memset((void*)&vkMemoryAllocateInfo, 0, sizeof(VkMemoryAllocateInfo));

    vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    vkMemoryAllocateInfo.pNext = NULL;
    vkMemoryAllocateInfo.allocationSize = vkMemoryRequirements.size;
    vkMemoryAllocateInfo.memoryTypeIndex = 0; // initial value before entering into loop

    VkBool32 foundMatchingMemoryType_Vertex = VK_FALSE;

    for (uint32_t i = 0; i < vkPhysicalDeviceMemoryProperties.memoryTypeCount; i++)
    {
        if ((vkMemoryRequirements.memoryTypeBits & 1) == 1)
        {
            if (vkPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
            {
                vkMemoryAllocateInfo.memoryTypeIndex = i; 
                foundMatchingMemoryType_Vertex = VK_TRUE;
                break;
            }
        }

        vkMemoryRequirements.memoryTypeBits >>= 1;
    
    }

    if (foundMatchingMemoryType_Vertex == VK_FALSE)
    {
        vkresult = VK_ERROR_OUT_OF_HOST_MEMORY; 
        fprintf(gpFile, "createVertexBuffer() : Failed to find suitable memory type for vertex buffer\n");
        return vkresult; // Return hardcoded error code
    }
    else
    {
        fprintf(gpFile, "createVertexBuffer() : Suitable memory type found for vertex buffer at index %d\n", vkMemoryAllocateInfo.memoryTypeIndex);
    }

    vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, NULL, &vertexData_Position.vkDeviceMemory);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createVertexBuffer() : vkAllocateMemory() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createVertexBuffer() : vkAllocateMemory() succeeded.\n");
    }

    vkresult = vkBindBufferMemory(vkDevice, vertexData_Position.vkBuffer, vertexData_Position.vkDeviceMemory, 0);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createVertexBuffer() : vkBindBufferMemory() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createVertexBuffer() : vkBindBufferMemory() succeeded.\n");
    }

    void* data = NULL;

    vkresult = vkMapMemory(vkDevice, vertexData_Position.vkDeviceMemory, 0, vkMemoryAllocateInfo.allocationSize, 0, &data);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createVertexBuffer() : vkMapMemory() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createVertexBuffer() : vkMapMemory() succeeded.\n");
    }

    // actual memory mapped

    memcpy(data, traingle_Position, sizeof(traingle_Position));

    vkUnmapMemory(vkDevice, vertexData_Position.vkDeviceMemory);


    return vkresult;

}


-(VkResult) createUniformBuffer
{

    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    // code
    VkBufferCreateInfo vkBufferCreateInfo;
    memset((void*)&vkBufferCreateInfo, 0, sizeof(VkBufferCreateInfo));

    vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vkBufferCreateInfo.pNext = NULL;
    vkBufferCreateInfo.flags = 0;
    vkBufferCreateInfo.size = sizeof(MyUniformData);
    vkBufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    memset((void*)&uniformData, 0, sizeof(UniformData));

    vkresult = vkCreateBuffer(vkDevice, &vkBufferCreateInfo, NULL, &uniformData.vkBuffer);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createUniformBuffer() : vkCreateBuffer() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createUniformBuffer() : vkCreateBuffer() succeeded.\n");
    }

    VkMemoryRequirements vkMemoryRequirements;
    memset((void*)&vkMemoryRequirements, 0, sizeof(VkMemoryRequirements));

    vkGetBufferMemoryRequirements(vkDevice, uniformData.vkBuffer, &vkMemoryRequirements);

    VkMemoryAllocateInfo vkMemoryAllocateInfo;
    memset((void*)&vkMemoryAllocateInfo, 0, sizeof(VkMemoryAllocateInfo));

    vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    vkMemoryAllocateInfo.pNext = NULL;
    vkMemoryAllocateInfo.allocationSize = vkMemoryRequirements.size;
    vkMemoryAllocateInfo.memoryTypeIndex = 0; // initial value before entering into loop

    VkBool32 foundMatchingMemoryType_Uniform = VK_FALSE;

    for (uint32_t i = 0; i < vkPhysicalDeviceMemoryProperties.memoryTypeCount; i++)
    {
        if ((vkMemoryRequirements.memoryTypeBits & 1) == 1)
        {
            if (vkPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
            {
                vkMemoryAllocateInfo.memoryTypeIndex = i;
                foundMatchingMemoryType_Uniform = VK_TRUE;
                break;
            }
        }

        vkMemoryRequirements.memoryTypeBits >>= 1;

    }

    if (foundMatchingMemoryType_Uniform == VK_FALSE)
    {
        vkresult = VK_ERROR_OUT_OF_HOST_MEMORY;
        fprintf(gpFile, "createUniformBuffer() : Failed to find suitable memory type for uniform buffer\n");
        return vkresult; // Return hardcoded error code
    }
    else
    {
        fprintf(gpFile, "createUniformBuffer() : Suitable memory type found for uniform buffer at index %d\n", vkMemoryAllocateInfo.memoryTypeIndex);
    }

    vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, NULL, &uniformData.vkDeviceMemory);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createUniformBuffer() : vkAllocateMemory() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createUniformBuffer() : vkAllocateMemory() succeeded.\n");
    }

    vkresult = vkBindBufferMemory(vkDevice, uniformData.vkBuffer, uniformData.vkDeviceMemory, 0);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createUniformBuffer() : vkBindBufferMemory() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createUniformBuffer() : vkBindBufferMemory() succeeded.\n");
    }

    // call updateUnifomBuffer
    vkresult = [self updateUniformbuffer];
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createUniformBuffer() : updateUniformbuffer() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createUniformBuffer() : updateUniformbuffer() succeeded.\n");
    }


    return vkresult;
}


-(VkResult) createShaders
{
    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    //code

    // for vertex shaders
    NSBundle* appBundle = [NSBundle mainBundle];

    NSString* appDirname = [appBundle bundlePath];

    NSString* parentDirPath = [appDirname stringByDeletingLastPathComponent];

    const char* szfileName = "Shader.vert.spv";

    NSString* shaderfileNameWithPath = [NSString stringWithFormat:@"%@/%s", parentDirPath, szfileName];

    const char* pszshaderFileNameWithPath = [shaderfileNameWithPath cStringUsingEncoding:NSUTF8StringEncoding];
    
    FILE* fp = NULL;

    size_t size;

    fp = fopen(pszshaderFileNameWithPath, "rb");

    if (fp == NULL)
    {
        fprintf(gpFile, "createShaders() : createShader failed to open vertexshader.spv file\n");
        vkresult = VK_ERROR_INITIALIZATION_FAILED;
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createShaders() : createShader succeeded to open vertexshader.spv file\n");
    }

    fseek(fp, 0L, SEEK_END);

    size = ftell(fp);

    if (size == 0)
    {
        fprintf(gpFile, "createShaders() : createShader failed and give file size of vertex shader 0\n");
        vkresult = VK_ERROR_INITIALIZATION_FAILED;
        return vkresult;
    }

    fseek(fp, 0L, SEEK_SET);

    char* shaderData = (char*)malloc(sizeof(char) * size);

    size_t retVal = fread(shaderData, size, 1, fp);

    if (retVal != 1)
    {
        fprintf(gpFile, "createShaders() : createShader failed to read vertexshader.spv file\n");
        vkresult = VK_ERROR_INITIALIZATION_FAILED;
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createShaders() : createShader succeeded to read vertexshader.spv file\n");
    }

    fclose(fp);

    VkShaderModuleCreateInfo vkShaderModuleCreateInfo;
    memset((void*)&vkShaderModuleCreateInfo, 0, sizeof(VkShaderModuleCreateInfo));

    vkShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    vkShaderModuleCreateInfo.pNext = NULL; 
    vkShaderModuleCreateInfo.flags = 0; // future use can be haapend but now zero
    vkShaderModuleCreateInfo.codeSize = size;
    vkShaderModuleCreateInfo.pCode = (uint32_t*)shaderData;

    vkresult = vkCreateShaderModule(vkDevice, &vkShaderModuleCreateInfo, NULL, &vkShaderModule_vertex_shader);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createShaders() : vkCreateShaderModule() function failed.for vertex shader Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createShaders() : vkCreateShaderModule() succeeded fro vertex shader.\n");
        fflush(gpFile);
    }


    if (shaderData)
    {
        free(shaderData);
        shaderData = NULL;
    }

    fprintf(gpFile, "Vertexshader Module sucessfully created\n");

    // for fragmnt shader

    // for fragment shader

    szfileName = "Shader.frag.spv";

    size = 0;

    shaderfileNameWithPath = [NSString stringWithFormat:@"%@/%s", parentDirPath, szfileName];

    pszshaderFileNameWithPath = [shaderfileNameWithPath cStringUsingEncoding:NSUTF8StringEncoding];

    fp = fopen(pszshaderFileNameWithPath, "rb");

    if (fp == NULL)
    {
        fprintf(gpFile, "createShaders() : createShader failed to open fragmentshader.spv file\n");
        vkresult = VK_ERROR_INITIALIZATION_FAILED;
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createShaders() : createShader succeeded to open fragmentshader.spv file\n");
    }

    fseek(fp, 0L, SEEK_END);

    size = ftell(fp);

    if (size == 0)
    {
        fprintf(gpFile, "createShaders() : createShader failed and give file size of fragment shader 0\n");
        vkresult = VK_ERROR_INITIALIZATION_FAILED;
        return vkresult;
    }

    fseek(fp, 0L, SEEK_SET);

    shaderData = (char*)malloc(sizeof(char) * size);

    retVal = fread(shaderData, size, 1, fp);

    if (retVal != 1)
    {
        fprintf(gpFile, "createShaders() : createShader failed to read fragmentshader.spv file\n");
        vkresult = VK_ERROR_INITIALIZATION_FAILED;
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createShaders() : createShader succeeded to read fragmentshader.spv file\n");
    }

    fclose(fp);

    memset((void*)&vkShaderModuleCreateInfo, 0, sizeof(VkShaderModuleCreateInfo));

    vkShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    vkShaderModuleCreateInfo.pNext = NULL;
    vkShaderModuleCreateInfo.flags = 0;
    vkShaderModuleCreateInfo.codeSize = size;
    vkShaderModuleCreateInfo.pCode = (uint32_t*)shaderData;

    vkresult = vkCreateShaderModule(
                    vkDevice,
                    &vkShaderModuleCreateInfo,
                    NULL,
                    &vkShaderModule_fragment_shader);

    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile,
                "createShaders() : vkCreateShaderModule() function failed for fragment shader. Error Code: (%d)\n",
                vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile,
                "createShaders() : vkCreateShaderModule() succeeded for fragment shader.\n");
        fflush(gpFile);
    }

    if (shaderData)
    {
        free(shaderData);
        shaderData = NULL;
    }

    fprintf(gpFile, "Fragment Shader Module successfully created\n");

    return vkresult;
}


-(VkResult) createRenderPass
{
    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    VkAttachmentDescription vkAttachmentDescription_array[2]; // now depth come so its 2

    memset((void*)vkAttachmentDescription_array, 0, sizeof(VkAttachmentDescription) * _ARRAYSIZE(vkAttachmentDescription_array));

    // for color
    vkAttachmentDescription_array[0].flags = 0;
    vkAttachmentDescription_array[0].format = vkFormat_color;
    vkAttachmentDescription_array[0].samples = VK_SAMPLE_COUNT_1_BIT;   //No multi sampling so 1 bit is enough
    vkAttachmentDescription_array[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    vkAttachmentDescription_array[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;                //color attachment related
    vkAttachmentDescription_array[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;            //This is for both Depth and Stencil although it is for stencil
    vkAttachmentDescription_array[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    vkAttachmentDescription_array[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;                                                    //Image data when in and when out
    vkAttachmentDescription_array[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // for depth
    vkAttachmentDescription_array[1].flags = 0;
    vkAttachmentDescription_array[1].format = vkFormat_Depth;
    vkAttachmentDescription_array[1].samples = VK_SAMPLE_COUNT_1_BIT;   //No multi sampling so 1 bit is enough
    vkAttachmentDescription_array[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    vkAttachmentDescription_array[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;                //color attachment related
    vkAttachmentDescription_array[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;            //This is for both Depth and Stencil although it is for stencil
    vkAttachmentDescription_array[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    vkAttachmentDescription_array[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;                                                    //Image data when in and when out
    vkAttachmentDescription_array[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    // for color

    //Declare and initialize vkAttachmentReference structure
    VkAttachmentReference vkAttachmentRefrence_Color;
    memset((void*)&vkAttachmentRefrence_Color, 0, sizeof(VkAttachmentReference));
    vkAttachmentRefrence_Color.attachment = 0;            //This means above given array 0th Ataachment reference, O means it is the index number
    vkAttachmentRefrence_Color.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;        //Tis means this attachment i can use it color attachment so keep it optimal

    // for depth atttachment
    VkAttachmentReference vkAttachmentRefrence_Depth;
    memset((void*)&vkAttachmentRefrence_Depth, 0, sizeof(VkAttachmentReference));
    vkAttachmentRefrence_Depth.attachment = 1;            //This means above given array 0th Ataachment reference, O means it is the index number
    vkAttachmentRefrence_Depth.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    //Step 3 : Declare and Initialize vkSubpassDescription

    VkSubpassDescription vkSubpassDesciption;
    memset((void*)&vkSubpassDesciption, 0, sizeof(VkSubpassDescription));
    vkSubpassDesciption.flags = 0;
    vkSubpassDesciption.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    vkSubpassDesciption.inputAttachmentCount = 0;
    vkSubpassDesciption.pInputAttachments = NULL;
    vkSubpassDesciption.colorAttachmentCount = 1; // this count should be the count of vkattachment refrence used for color
    vkSubpassDesciption.pColorAttachments = &vkAttachmentRefrence_Color;
    vkSubpassDesciption.pResolveAttachments = NULL;
    vkSubpassDesciption.pDepthStencilAttachment = &vkAttachmentRefrence_Depth;
    vkSubpassDesciption.preserveAttachmentCount = 0;
    vkSubpassDesciption.pPreserveAttachments = NULL;

    // if subpass dependency synchronization

    VkSubpassDependency vkSubpassDependency_Array[2];
    memset((void*)vkSubpassDependency_Array, 0, sizeof(VkSubpassDependency) * _ARRAYSIZE(vkSubpassDependency_Array));

    // for Color 
    vkSubpassDependency_Array[0].srcSubpass = VK_SUBPASS_EXTERNAL; // this means implicit subpass before render pass begin
    vkSubpassDependency_Array[0].dstSubpass = 0; // this means first subpass which is 0th index in subpass array
    vkSubpassDependency_Array[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // this means color attachment stage of src subpass
    vkSubpassDependency_Array[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // this means color attachment stage of dst subpass    
    vkSubpassDependency_Array[0].srcAccessMask = 0; // this means which type of access of src subpass
    vkSubpassDependency_Array[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // this means which type of access of dst subpass
    vkSubpassDependency_Array[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT; 

    // for depth
    vkSubpassDependency_Array[1].srcSubpass = VK_SUBPASS_EXTERNAL; // this means implicit subpass before render pass begin
    vkSubpassDependency_Array[1].dstSubpass = 0; // this means first subpass which is 0th index in subpass array
    vkSubpassDependency_Array[1].srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT; 
    vkSubpassDependency_Array[1].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT; // this means depth attachment stage of dst subpass    
    vkSubpassDependency_Array[1].srcAccessMask = 0; // this means which type of access of src subpass
    vkSubpassDependency_Array[1].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; // this means which type of access of dst subpass
    vkSubpassDependency_Array[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT; // this means by default dependency



    //Step 4: Declare and initialize vkrenderpass create info structure
    VkRenderPassCreateInfo vkRenderPassCreateInfo;
    memset((void*)&vkRenderPassCreateInfo, 0, sizeof(VkRenderPassCreateInfo));
    vkRenderPassCreateInfo.flags = 0;
    vkRenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    vkRenderPassCreateInfo.pNext = NULL;
    vkRenderPassCreateInfo.attachmentCount = _ARRAYSIZE(vkAttachmentDescription_array);
    vkRenderPassCreateInfo.pAttachments = vkAttachmentDescription_array;
    vkRenderPassCreateInfo.subpassCount = 1;
    vkRenderPassCreateInfo.pSubpasses = &vkSubpassDesciption;
    vkRenderPassCreateInfo.dependencyCount = _ARRAYSIZE(vkSubpassDependency_Array);
    vkRenderPassCreateInfo.pDependencies = vkSubpassDependency_Array;


    // Create render pass
    vkresult = vkCreateRenderPass(vkDevice, &vkRenderPassCreateInfo, NULL, &vkRenderpass);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createRenderPass() : vkCreateRenderPass() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createRenderPass() : vkCreateRenderPass() succeeded.\n");
        fflush(gpFile);
    }

    return vkresult;
}


-(VkResult) createPipline
{
    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    // vertex input state
    VkVertexInputBindingDescription vkVertexInputBindingDescription_Array[1];
    memset((void*)vkVertexInputBindingDescription_Array, 0, sizeof(VkVertexInputBindingDescription) * _ARRAYSIZE(vkVertexInputBindingDescription_Array));

    vkVertexInputBindingDescription_Array[0].binding = 0;
    vkVertexInputBindingDescription_Array[0].stride = sizeof(float) * 3;
    vkVertexInputBindingDescription_Array[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription vkVertexInputAttributeDescription_Array[1];
    memset((void*)vkVertexInputAttributeDescription_Array, 0, sizeof(VkVertexInputAttributeDescription) * _ARRAYSIZE(vkVertexInputAttributeDescription_Array));

    vkVertexInputAttributeDescription_Array[0].binding = 0;
    vkVertexInputAttributeDescription_Array[0].location = 0;
    vkVertexInputAttributeDescription_Array[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vkVertexInputAttributeDescription_Array[0].offset = 0;

    VkPipelineVertexInputStateCreateInfo vkPipelineVertexInputStateCreateInfo;
    memset((void*)&vkPipelineVertexInputStateCreateInfo, 0, sizeof(VkPipelineVertexInputStateCreateInfo));

    vkPipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vkPipelineVertexInputStateCreateInfo.pNext = NULL;
    vkPipelineVertexInputStateCreateInfo.flags = 0;
    vkPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = _ARRAYSIZE(vkVertexInputBindingDescription_Array);
    vkPipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vkVertexInputBindingDescription_Array;
    vkPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = _ARRAYSIZE(vkVertexInputAttributeDescription_Array);
    vkPipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vkVertexInputAttributeDescription_Array;


    /// Input Assembly state

    VkPipelineInputAssemblyStateCreateInfo vkPipelineInputAssemblyStateCreateInfo;
    memset((void*)&vkPipelineInputAssemblyStateCreateInfo, 0, sizeof(VkPipelineInputAssemblyStateCreateInfo));

    vkPipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    vkPipelineInputAssemblyStateCreateInfo.pNext = NULL;
    vkPipelineInputAssemblyStateCreateInfo.flags = 0;
    vkPipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;


    // Rasterizer State 

    VkPipelineRasterizationStateCreateInfo vkPipelineRasterizationStateCreateInfo;
    memset((void*)&vkPipelineRasterizationStateCreateInfo, 0, sizeof(VkPipelineRasterizationStateCreateInfo));


    vkPipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    vkPipelineRasterizationStateCreateInfo.pNext = NULL;
    vkPipelineRasterizationStateCreateInfo.flags = 0;

    vkPipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    vkPipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    vkPipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    vkPipelineRasterizationStateCreateInfo.lineWidth = 1.0f;


    // colorBlend State
    VkPipelineColorBlendAttachmentState vkPipelineColorBlendAttachmentState_Array[1];
    memset((void*)vkPipelineColorBlendAttachmentState_Array, 0, sizeof(VkPipelineColorBlendAttachmentState) * _ARRAYSIZE(vkPipelineColorBlendAttachmentState_Array));

    vkPipelineColorBlendAttachmentState_Array[0].blendEnable = VK_FALSE;
    vkPipelineColorBlendAttachmentState_Array[0].colorWriteMask = VK_COLOR_COMPONENT_G_BIT;


    VkPipelineColorBlendStateCreateInfo vkPipelineColorBlendStateCreateInfo;
    memset((void*)&vkPipelineColorBlendStateCreateInfo, 0, sizeof(VkPipelineColorBlendStateCreateInfo));

    vkPipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    vkPipelineColorBlendStateCreateInfo.pNext = NULL;
    vkPipelineColorBlendStateCreateInfo.flags = 0;

    vkPipelineColorBlendStateCreateInfo.attachmentCount = _ARRAYSIZE(vkPipelineColorBlendAttachmentState_Array);
    vkPipelineColorBlendStateCreateInfo.pAttachments = vkPipelineColorBlendAttachmentState_Array;


    // Viewport Sessor state
    VkPipelineViewportStateCreateInfo vkPipelineViewportStateCreateInfo;
    memset((void*)&vkPipelineViewportStateCreateInfo, 0, sizeof(VkPipelineViewportStateCreateInfo));

    vkPipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vkPipelineViewportStateCreateInfo.pNext = NULL;
    vkPipelineViewportStateCreateInfo.flags = 0;
    vkPipelineViewportStateCreateInfo.viewportCount = 1;

    memset((void*)&vkViewport, 0, sizeof(VkViewport));
    vkViewport.x = 0;
    vkViewport.y = 0;
    vkViewport.width = (float)vkExtent2D_Swapchain.width;
    vkViewport.height = (float)vkExtent2D_Swapchain.height;
    vkViewport.minDepth = 0.0f;
    vkViewport.maxDepth = 1.0f;

    vkPipelineViewportStateCreateInfo.pViewports = &vkViewport;
    vkPipelineViewportStateCreateInfo.scissorCount = 1;
    memset((void*)&vkRect2D_Scissor, 0, sizeof(VkRect2D));
    vkRect2D_Scissor.offset.x = 0;
    vkRect2D_Scissor.offset.y = 0;
    vkRect2D_Scissor.extent.width = vkExtent2D_Swapchain.width;
    vkRect2D_Scissor.extent.height = vkExtent2D_Swapchain.height;
    vkPipelineViewportStateCreateInfo.pScissors = &vkRect2D_Scissor;


    // detpth stencil state

    VkPipelineDepthStencilStateCreateInfo vkPipelineDepthStencilStateCreateInfo;
    memset((void*)&vkPipelineDepthStencilStateCreateInfo, 0, sizeof(VkPipelineDepthStencilStateCreateInfo));

    vkPipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    vkPipelineDepthStencilStateCreateInfo.pNext = NULL;
    vkPipelineDepthStencilStateCreateInfo.flags = 0;
    vkPipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;            // Enable depth test
    vkPipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;          
    vkPipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL; 
    vkPipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;  
    vkPipelineDepthStencilStateCreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
    vkPipelineDepthStencilStateCreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
    vkPipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
    vkPipelineDepthStencilStateCreateInfo.front = vkPipelineDepthStencilStateCreateInfo.back;
    vkPipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;



    // dynamic state

    // as we dont have any dynamic state so we can skip this


    // multisample state
    VkPipelineMultisampleStateCreateInfo vkPipelineMultisampleStateCreateInfo;
    memset((void*)&vkPipelineMultisampleStateCreateInfo, 0, sizeof(VkPipelineMultisampleStateCreateInfo));

    vkPipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    vkPipelineMultisampleStateCreateInfo.pNext = NULL;
    vkPipelineMultisampleStateCreateInfo.flags = 0;
    vkPipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // shader state
    VkPipelineShaderStageCreateInfo vkPipelineShaderStageCreateInfo_Array[2];
    memset((void*)vkPipelineShaderStageCreateInfo_Array, 0, sizeof(VkPipelineShaderStageCreateInfo)* _ARRAYSIZE(vkPipelineShaderStageCreateInfo_Array));

    // for vertex shader
    vkPipelineShaderStageCreateInfo_Array[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vkPipelineShaderStageCreateInfo_Array[0].pNext = NULL;
    vkPipelineShaderStageCreateInfo_Array[0].flags = 0;
    vkPipelineShaderStageCreateInfo_Array[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    vkPipelineShaderStageCreateInfo_Array[0].module = vkShaderModule_vertex_shader;
    vkPipelineShaderStageCreateInfo_Array[0].pName = "main";
    vkPipelineShaderStageCreateInfo_Array[0].pSpecializationInfo = NULL;

    // for fragment shader
    vkPipelineShaderStageCreateInfo_Array[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vkPipelineShaderStageCreateInfo_Array[1].pNext = NULL;
    vkPipelineShaderStageCreateInfo_Array[1].flags = 0;
    vkPipelineShaderStageCreateInfo_Array[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    vkPipelineShaderStageCreateInfo_Array[1].module = vkShaderModule_fragment_shader;
    vkPipelineShaderStageCreateInfo_Array[1].pName = "main";
    vkPipelineShaderStageCreateInfo_Array[1].pSpecializationInfo = NULL;


    // tisellation state

    // we dont have tisellation shaders so we can skip this state


    // as piplines are created from pipline cache now we will create pipline cache object

    VkPipelineCacheCreateInfo vkPipelineCacheCreateInfo;
    memset((void*)&vkPipelineCacheCreateInfo, 0, sizeof(VkPipelineCacheCreateInfo));

    vkPipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    vkPipelineCacheCreateInfo.pNext = NULL;
    vkPipelineCacheCreateInfo.flags = 0;

    VkPipelineCache vkPipelineCache = VK_NULL_HANDLE;

    vkresult = vkCreatePipelineCache(vkDevice, &vkPipelineCacheCreateInfo, NULL, &vkPipelineCache);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createPipline() : vkCreatePipelineCache() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createPipline() : vkCreatePipelineCache() succeeded.\n");
        fflush(gpFile);
    }

    // create actual graphics pipline
    VkGraphicsPipelineCreateInfo vkGraphicsPipelineCreateInfo;
    memset((void*)&vkGraphicsPipelineCreateInfo, 0, sizeof(VkGraphicsPipelineCreateInfo));

    vkGraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    vkGraphicsPipelineCreateInfo.pNext = NULL;
    vkGraphicsPipelineCreateInfo.flags = 0;

    vkGraphicsPipelineCreateInfo.pVertexInputState = &vkPipelineVertexInputStateCreateInfo;
    vkGraphicsPipelineCreateInfo.pInputAssemblyState = &vkPipelineInputAssemblyStateCreateInfo;
    vkGraphicsPipelineCreateInfo.pRasterizationState = &vkPipelineRasterizationStateCreateInfo;
    vkGraphicsPipelineCreateInfo.pColorBlendState = &vkPipelineColorBlendStateCreateInfo;
    vkGraphicsPipelineCreateInfo.pViewportState = &vkPipelineViewportStateCreateInfo;
    vkGraphicsPipelineCreateInfo.pDepthStencilState = &vkPipelineDepthStencilStateCreateInfo;
    vkGraphicsPipelineCreateInfo.pDynamicState = NULL;
    vkGraphicsPipelineCreateInfo.pMultisampleState = &vkPipelineMultisampleStateCreateInfo;
    vkGraphicsPipelineCreateInfo.stageCount = _ARRAYSIZE(vkPipelineShaderStageCreateInfo_Array);
    vkGraphicsPipelineCreateInfo.pStages = vkPipelineShaderStageCreateInfo_Array;
    vkGraphicsPipelineCreateInfo.pTessellationState = NULL;
    vkGraphicsPipelineCreateInfo.layout = vkPipelineLayout;
    vkGraphicsPipelineCreateInfo.renderPass = vkRenderpass;
    vkGraphicsPipelineCreateInfo.subpass = 0;
    vkGraphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    vkGraphicsPipelineCreateInfo.basePipelineIndex = 0;


    /// Now create the pipline

    vkresult = vkCreateGraphicsPipelines(vkDevice, vkPipelineCache, 1, &vkGraphicsPipelineCreateInfo, NULL, &vkPipeline);
    if (vkresult != VK_SUCCESS)
    {
        fprintf(gpFile, "createPipeline() : vkCreateGraphicsPipelines() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        fprintf(gpFile, "createPipeline() : vkCreateGraphicsPipelines() succeeded.\n");
        fflush(gpFile);
    }

    // we have done with pipline cache so destroy it

    if (vkPipelineCache != VK_NULL_HANDLE)
    {
        vkDestroyPipelineCache(vkDevice, vkPipelineCache, NULL);
        vkPipelineCache = VK_NULL_HANDLE;
        fprintf(gpFile, "createPipeline() : Free vkPipelineCache freed\n");
    }

    return vkresult;

}

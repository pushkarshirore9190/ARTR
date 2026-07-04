//
//  View.m
//  Template
//
//  Created by Pushkar Rajendra Shirore on 18/06/26.
//

#import "View.h"

// glm related macros and header files
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TWO_ONE
#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"

// vulkan related variables
// vulkan/vulkan.h was we were using in all 4 platfom but here it is included in below file
#import<MoltenVK/mvk_vulkan.h>

#define _ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))

// global variable declarations
int winWidth = 0;
int winHeight = 0;

const char* gpszAppName = "ARTR";

// vulkun related global variables
uint32_t enabledInstanceExtensionCount = 0;

const char* enabledInstanceExtensionNames_array[5];  // VK_KHR_SURFACE_EXTENSION_NAME and VK_KHR_MACOS_SURFACE_EXTENSION_NAME and VK_EXT_DEBUG_UTILS_EXTENSION_NAME and VK_PORTABILITY_ENUMERATION_EXTENSION_NAME and VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME

VkBool32 vulkanPortabilityEnumerationExtensionFound = VK_FALSE;

// instance extension related variables
VkInstance vkInstance = VK_NULL_HANDLE;

// Vulakn Presentation Surface
VkSurfaceKHR  vkSurfaceKHR = VK_NULL_HANDLE;

// Vulkan Physical Device Related Global Variables
VkPhysicalDevice vkPhysicalDevice_selected = VK_NULL_HANDLE;

uint32_t graphicsQueueFamilyIndex_Selected = UINT_MAX;

VkPhysicalDeviceMemoryProperties vkPhysicalDeviceMemoryProperties;

// get from getPhysicalDevice()

uint32_t physicalDeviceCount = 0;

VkPhysicalDevice* vkPhysicalDevice_Array = NULL;


uint32_t enableDeviceExtensionCount = 0;

const char* enabledDeviceExtensionNames_array[1];  // VK_KHR_SWAPCHAIN_EXTENSION_NAME

// Vulkan Device
VkDevice vkDevice = VK_NULL_HANDLE;

// device queue
VkQueue vkQueue = VK_NULL_HANDLE;

// color format and color space
VkFormat vkFormat_color = VK_FORMAT_UNDEFINED;

VkColorSpaceKHR vkColorSpaceKHR = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

// presentation mode
VkPresentModeKHR vkPresentModeKHR = VK_PRESENT_MODE_FIFO_KHR;

//swap chain related global variables
VkSwapchainKHR vkSwapchainKHR = VK_NULL_HANDLE;
VkExtent2D vkExtent2D_Swapchain;

// swapchain images and swapchain image views relrated data
uint32_t swapchainImageCount = UINT32_MAX;
VkImage* SwapchainImage_Array = NULL;
VkImageView* SwapchainImageView_Array = NULL;


// command pool
VkCommandPool vkcommandpool = VK_NULL_HANDLE;

// command buffer
VkCommandBuffer* vkCommandBuffer_Array = NULL;

// render pass
VkRenderPass vkRenderpass = VK_NULL_HANDLE;

// frameBuffer
VkFramebuffer* vkFramebuffer_Array = NULL;

// Semaphore
VkSemaphore vkSemaphore_backbuffer = VK_NULL_HANDLE;
VkSemaphore vkSemaphore_rendercomplete = VK_NULL_HANDLE;

// Fence
VkFence* vkFence_Array = NULL;

// Clear color values
VkClearColorValue vkClearColorValue;

BOOL bInitialised = NO;

uint32_t currentImageIndex = UINT32_MAX;

// Validation
BOOL bValidation = YES;

uint32_t enabledValidationLayerCount = 0;

const char* enabledValidationLayerNames_Array[1];  // for VK_LAYER_CRONOS_validation

VkDebugUtilsMessengerEXT vkDebugUtilsMessengerEXT_handle = VK_NULL_HANDLE;

PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT_fnptr = NULL;

// Pipline
VkViewport vkViewport;

VkRect2D vkRect2D_Scissor;

VkPipeline vkPipeline = VK_NULL_HANDLE;


// VertexBuffer Related Variable
typedef struct
{
    VkBuffer vkBuffer;
    VkDeviceMemory vkDeviceMemory;

} VertexData;

// poaition
VertexData vertexData_Position;

VertexData vertexData_Color;

// uniform related declarations
struct MyUniformData
{
    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};

struct UniformData
{
    VkBuffer vkBuffer;
    VkDeviceMemory vkDeviceMemory;
};

UniformData uniformData;

// shader related variables
VkShaderModule vkShaderModule_vertex_shader = VK_NULL_HANDLE;

VkShaderModule vkShaderModule_fragment_shader = VK_NULL_HANDLE;

// discrptorsetlayout object
VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;


// Pipeline Layout Object
VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;

// Descriptor pool
VkDescriptorPool vkDescriptorPool = VK_NULL_HANDLE;

// Descriptor set
VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;



@implementation View

+(Class)layerClass
{
    return [CAMetalLayer class];
}

-(id)initWithFrame:(CGRect)frameRect
{
    // variable declarations
    UITapGestureRecognizer *singletabGestureRecognizer = nil;
    UITapGestureRecognizer *doubleTabGestureRecognizer = nil;
    UISwipeGestureRecognizer *swipeGestureRecognizer = nil;
    UILongPressGestureRecognizer *longPressGestureRecognizer = nil;
    

    // code
    self = [super initWithFrame:frameRect];
    if(self)
    {
        winWidth = self.bounds.size.width;
        winHeight = self.bounds.size.height;

        // initialise
        VkResult vkresult = [self initialise];

        if (vkresult != VK_SUCCESS)
        {
            printf("Initialisation failed\n");
            return self;
        }
        else
        {
            printf("Initialisation successful\n");
        }
        
        // respond to events
        [self becomeFirstResponder];

        // gesture recognization

        // single tap
        singletabGestureRecognizer = [[UITapGestureRecognizer alloc]
                                      initWithTarget:self
                                      action:@selector(onSingleTap:)];

        [singletabGestureRecognizer setNumberOfTapsRequired:1];

        [singletabGestureRecognizer setNumberOfTouchesRequired:1];

        [singletabGestureRecognizer setDelegate:self];

        [self addGestureRecognizer:singletabGestureRecognizer];
        
        // double tap
        doubleTabGestureRecognizer = [[UITapGestureRecognizer alloc]
                                      initWithTarget:self
                                      action:@selector(onDoubleTap:)];

        [doubleTabGestureRecognizer setNumberOfTapsRequired:2];

        [doubleTabGestureRecognizer setNumberOfTouchesRequired:1];

        [doubleTabGestureRecognizer setDelegate:self];

        [self addGestureRecognizer:doubleTabGestureRecognizer];
        
        // late single tap wait double tap to fail so that the two get diffrenciated
        [singletabGestureRecognizer requireGestureRecognizerToFail:(doubleTabGestureRecognizer)];
        
        // swipe
        swipeGestureRecognizer = [[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
        
        [swipeGestureRecognizer setDelegate:self];
        
        [self addGestureRecognizer:swipeGestureRecognizer];
        
        // long press
        longPressGestureRecognizer = [[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
        
        [self addGestureRecognizer:longPressGestureRecognizer];
        
        // remember when you register your gesture recognizer object to the view means to self it retains it by incrementing the refrence count so we must decrement it
        [longPressGestureRecognizer release];
        longPressGestureRecognizer = nil;
        
        [swipeGestureRecognizer release];
        swipeGestureRecognizer = nil;
        
        [doubleTabGestureRecognizer release];
        doubleTabGestureRecognizer = nil;
        
        [singletabGestureRecognizer release];
        singletabGestureRecognizer = nil;
        
    }
    
    return self;
}

// implement this function only if you have custome deawing not in rendering and still if you kepp it empty without commenting performance hurts during animation because to painting events cuncurrenlty get called one is maintreads drawrect and another is renderer's render

/*-(void)drawRect:(CGRect)rect
{
    //code
}*/

-(void)drawView
{
    VkResult vkresult = VK_SUCCESS;

    vkresult = [self render];

    if (vkresult == VK_ERROR_DEVICE_LOST)
    {
        printf("drawView() : device lost, halting rendering\n");
        bInitialised = NO;   // stops further render attempts (render() already checks this)
        return;
    }

    if (vkresult != VK_SUCCESS &&
        vkresult != VK_FALSE &&
        vkresult != VK_SUBOPTIMAL_KHR &&
        vkresult != VK_ERROR_OUT_OF_DATE_KHR)
    {
        printf("drawView() : render failed\n");
        return;
    }

    [self update];
}


-(void)layoutSubviews
{
    // code
    [super layoutSubviews];

    // set changed winWidth and winHeight according to View's layout
    winWidth = [self bounds].size.width * [self contentScaleFactor];
    winHeight = [self bounds].size.height * [self contentScaleFactor];

    // call our resize here
    [self resize:winWidth :winHeight];
}

-(BOOL)canBecomeFirstResponder
{
    // code
    return YES;
}

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    // code
}

-(void)onSingleTap:(UITapGestureRecognizer *)gestureRecognizer
{
    // code
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gestureRecognizer
{
    // code
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gestureRecognizer
{
    // code
    [self uninitialise];
    exit(0);
}

-(void) dealloc
{
    // code
    [super dealloc];
}


-(void)onLongPress:(UITapGestureRecognizer *)gestureRecognizer
{
    // code
}

-(VkResult) initialise
{
    // function declarations

    // chnages
    //1. change all YES to YES and all NO to NO
    //2. chnage all user define functions declaration call to objective c method definition syntax
    // 3. chnage all function declarator in objective c method definition to objective c method definition syntax
    // 4. remove all function definitions and replace with objective c method definition syntax

    // conceptual chnages function
    // 1. createVulkanInstance()
    // 2. fillExtensionNames()
    // 3. getSupportedSurface()
    // 4. fillDeviceExtensionNames()
    // 5. creteSwapchain()
    // 6. createImagesAndImageViews()
    // 7. createVertexBuffer()
    // 8. createUniformBuffer()
    // 9. createShaders()
    // 10. createPipeline


    // variable declarations
    VkResult vkresult = VK_SUCCESS;

    // code
    vkresult = [self createVulkanInstance];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : createVulkanInstance() function failed\n");
        return(vkresult);
    }
    else
    {
        printf( "initialise() : createVulkanInstance() succeeded\n");
    }

    // variable declarations

    // create Vulkan Presentation Surface
    vkresult = [self getSupportedSurface];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : getSupportedSurface() function failed\n");
        return(vkresult);
    }
    else
    {
        printf( "initialise() : getSupportedSurface() succeeded\n");
    }

    // select required physical device and its queue family index
    vkresult = [self getPhysicalDevice];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : getPhysicalDevice() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "initialise() : getPhysicalDevice() succeeded\n");
    }

    // print vulkan info
    vkresult = [self printVkInfo];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : printVkInfo() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "initialise() : printVkInfo() succeeded\n");
    }

    vkresult = [self createVulkanDevice];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : createVulkanDevice() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "initialise() : createVulkanDevice() succeeded\n");
    }

    // get device queue
    [self getDeviceQueue];

    // createSwapchain
    vkresult = [self createSwapchain:VK_FALSE];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : createSwapchain() function failed (%d)\n", vkresult);
        return VK_ERROR_INITIALIZATION_FAILED; // Hardcoded return value
    }
    else
    {
        printf( "initialise() : createSwapchain() succeeded\n");
    }

    vkresult = [self createImagesAndImageViews];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : createImagesAndImageViews() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "initialise() : createImagesAndImageViews() succeeded\n");
    }

    vkresult = [self createCommandPool];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : createCommandPool() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "initialise() : createCommandPool() succeeded\n");
    }

    vkresult = [self createCommandBuffers];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : createCommandBuffers() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "initialise() : createCommandBuffers() succeeded\n");
    }

    // craete VertexBuffer
    vkresult = [self createVertexBuffer];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : createVertexBuffer() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "initialise() : createVertexBuffer() succeeded\n");
    }

    // createUniform Buffer
    vkresult = [self createUniformBuffer];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : createUniformBuffer() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "initialise() : createUniformBuffer() succeeded\n");
    }



    vkresult = [self createShaders];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : createShaders() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "initialise() : createShaders() succeeded\n");
    }

    vkresult = [self createDiscriptorSetLayout];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : createDiscriptorSetLayout() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "initialise() : createDiscriptorSetLayout() succeeded\n");
    }

    vkresult = [self createPiplineLayout];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : createPiplineLayout() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "initialise() : createPiplineLayout() succeeded\n");
    }

    // create descriptorpool
    vkresult = [self createDescriptorpool];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : createDescriptorpool() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "initialise() : createDescriptorpool() succeeded\n");
    }

    // create descriptorset
    vkresult = [self createDescriptorSet];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : createDescriptorset() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "initialise() : createDescriptorset() succeeded\n");
    }



    vkresult = [self createRenderPass];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : createRenderPass() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "initialise() : createRenderPass() succeeded\n");
    }

    vkresult = [self createPipline];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : createPipline() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "initialise() : createPipline() succeeded\n");
    }

    vkresult = [self createframeBuffers];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : createframeBuffer() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "initialise() : createframeBuffer() succeeded\n");
    }

    // craete semaphores
    vkresult = [self createSemaphores];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : createSemaphores() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "initialise() : createSemaphores() succeeded\n");
    }

    //create Fences
    vkresult = [self createFences];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : createFences() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "initialise() : createFences() succeeded\n");
    }

    // initialise clear color values
    memset((void*)&vkClearColorValue, 0, sizeof(VkClearColorValue));

    vkClearColorValue.float32[0] = 0.0f;
    vkClearColorValue.float32[1] = 0.0f;
    vkClearColorValue.float32[2] = 0.0f;
    vkClearColorValue.float32[3] = 1.0f;  // analogse to glclear color

    // build commmand buffers
    vkresult = [self buildCommandBuffers];
    if (vkresult != VK_SUCCESS)
    {
        printf( "initialise() : buildCommandBuffers() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "initialise() : buildCommandBuffers() succeeded\n");
    }


    // initialisation is completed

    bInitialised = YES;


    printf( "******************************************* Initialise comment *****************************\n");

    printf( "Initialised()  :  Initialisation() Complete Successfully\n");

    return(vkresult);
}


-(VkResult) resize : (int)width : (int)heigth
{

    // variable declarations
    VkResult vkresult = VK_SUCCESS;

    // code
    if (heigth <= 0)
        heigth = 1;

    // check the bInitialised variable
    if (bInitialised == NO)
    {
        printf( "resize() : Initialisation yet not completed or failed\n");
        vkresult = VK_ERROR_INITIALIZATION_FAILED;
        return vkresult;
    }

    // as recreation of swapchain is needed we are going to repeate many steps of initialise again hence set bInitialised  =  NO again
    bInitialised = NO;

    // set global winwidth and winheight variables
    winWidth = width;
    winHeight = heigth;

    // wait for device to complete in hand task
        if (vkDevice)
        {
            vkDeviceWaitIdle(vkDevice);
            printf( " resize() : \n vkDeviceWaitIdle() is Done\n");
        }

        // check presence of swapchain
        if (vkSwapchainKHR == VK_NULL_HANDLE)
        {
            printf( "\n resize() : swapchain is aleady null cannot proceed\n");
            vkresult = VK_ERROR_INITIALIZATION_FAILED;
            return vkresult;

        }

        //Framebuffer free
        for (uint32_t i = 0; i < swapchainImageCount; i++)
        {
            vkDestroyFramebuffer(vkDevice, vkFramebuffer_Array[i], NULL);

        }

        // destory frame buffer
        if (vkFramebuffer_Array)
        {
            free(vkFramebuffer_Array);
            vkFramebuffer_Array = NULL;
            printf( " resize() : \nFree commandbuffers freed\n");

        }

        // destroy commandbuffer
        for (uint32_t i = 0; i < swapchainImageCount; i++)
        {
            vkFreeCommandBuffers(vkDevice, vkcommandpool, 1, &vkCommandBuffer_Array[i]);
            //vkDestroyImageView(vkDevice, swapchainImageView_array[i], NULL);
            printf( "\n resize() : Free commandbuffers freed\n");
        }

        if (vkCommandBuffer_Array)
        {
            free(vkCommandBuffer_Array);
            vkCommandBuffer_Array = NULL;
            printf( "\n resize() : commandbuffers Array frred\n");
        }

        // destroy pipline
        if (vkPipeline)
        {
            vkDestroyPipeline(vkDevice, vkPipeline, NULL);
            vkPipeline = VK_NULL_HANDLE;
            printf( "\n resize() : Free vkPipeline: Pipeline freed successfully.\n");
        }

        // destroy pipline layout
        if (vkPipelineLayout)
        {
            vkDestroyPipelineLayout(vkDevice, vkPipelineLayout, NULL);
            vkPipelineLayout = VK_NULL_HANDLE;
            printf( "\n resize() : Free vkPipelineLayout freed\n");
        }

        // destroy render pass
        if (vkRenderpass)
        {
            vkDestroyRenderPass(vkDevice, vkRenderpass, NULL);
            vkRenderpass = VK_NULL_HANDLE;
            printf( "\n resize() : Free vkRenderpass freed\n");
        }

        // destroy ImageView
        for (uint32_t i = 0; i < swapchainImageCount; i++)
        {
            vkDestroyImageView(vkDevice, SwapchainImageView_Array[i], NULL);
            printf( "\nresize() : Free swapchainImage_array images freed\n");
        }
        if (SwapchainImageView_Array)
        {
            free(SwapchainImageView_Array);
            SwapchainImageView_Array = NULL;
        }

        /*for (uint32_t i = 0; i < swapchainImageCount; i++)
            {
                vkDestroyImage(vkDevice, SwapchainImage_Array[i], NULL);
                printf( "\nFree SwapchainImage_Array images freed\n");
            }*/

        if (SwapchainImage_Array)
        {
            free(SwapchainImage_Array);
            SwapchainImage_Array = NULL;
        }

        //destroy swapchain
        if (vkSwapchainKHR)
        {
            vkDestroySwapchainKHR(vkDevice, vkSwapchainKHR, NULL);
            vkSwapchainKHR = NULL;        //Bhanda swachha
            printf( "\n resize() : vkSwapchainKHR is Freed\n");

        }


        ////// RECREATE FIR RESIZE \\\\\\\

        // create swapchain
        vkresult = [self createSwapchain:VK_FALSE];
        if (vkresult != VK_SUCCESS)
        {
            printf( " resize() : createSwapchain() function failed (%d)\n", vkresult);
            return VK_ERROR_INITIALIZATION_FAILED; // Hardcoded return value
        }

        vkresult = [self createImagesAndImageViews];
        if (vkresult != VK_SUCCESS)
        {
            printf( " resize() : createImagesAndImageViews() function failed (%d)\n", vkresult);
            return(vkresult);
        }

        vkresult = [self createRenderPass];
        if (vkresult != VK_SUCCESS)
        {
            printf( " resize() : createRenderPass() function failed (%d)\n", vkresult);
            return(vkresult);
        }

        vkresult = [self createPiplineLayout];
        if (vkresult != VK_SUCCESS)
        {
            printf( " resize() : createPiplineLayout() function failed (%d)\n", vkresult);
            return(vkresult);
        }

        vkresult = [self createPipline];
        if (vkresult != VK_SUCCESS)
        {
            printf( " resize() : createPipline() function failed (%d)\n", vkresult);
            return(vkresult);
        }

        vkresult = [self createframeBuffers];
        if (vkresult != VK_SUCCESS)
        {
            printf( " resize() : createframeBuffer() function failed (%d)\n", vkresult);
            return(vkresult);
        }

        vkresult =  [self createCommandBuffers];
        if (vkresult != VK_SUCCESS)
        {
            printf( " resize() : createCommandBuffers() function failed (%d)\n", vkresult);
            return(vkresult);
        }

        // build commmand buffers
        vkresult = [self buildCommandBuffers];
        if (vkresult != VK_SUCCESS)
        {
            printf( " resize() : buildCommandBuffers() function failed (%d)\n", vkresult);
            return(vkresult);
        }


    bInitialised = YES;

    return(vkresult);
}


-(VkResult) render
{
     
    // variable declarations
    VkResult vkresult = VK_SUCCESS;

    // code

    // if control comes here before initilisation gets completed return NO

    if (bInitialised == NO)
    {
        printf( "render(): initliasation yet not completed\n");
        return (VkResult)VK_FALSE;
    }

    // acquire index of next swapchain image
    vkresult = vkAcquireNextImageKHR(vkDevice, vkSwapchainKHR, UINT64_MAX, vkSemaphore_backbuffer, VK_NULL_HANDLE, &currentImageIndex);
    if (vkresult != VK_SUCCESS && vkresult != VK_SUBOPTIMAL_KHR && vkresult != VK_ERROR_OUT_OF_DATE_KHR)
    {
        printf( "render() : vkAcquireNextImageKHR failed with error: %d\n", vkresult);
        return(vkresult);
    }

    // use fence to allow host to wait for completion of execution previous commmand buffer

    vkresult = vkWaitForFences(vkDevice, 1, &vkFence_Array[currentImageIndex], VK_TRUE, UINT64_MAX);
    if (vkresult != VK_SUCCESS)
    {
        printf( "render() : vkWaitForFences failed with error: %d\n", vkresult);
        return(vkresult);
    }

    // now ready the fences for next command buffer
    vkresult = vkResetFences(vkDevice, 1, &vkFence_Array[currentImageIndex]);
    if (vkresult != VK_SUCCESS)
    {
        printf( "render() : vkResetFences failed with error: %d\n", vkresult);
        return(vkresult);
    }

    //one of the memnber of the submit info structure requires array of pipline stages we have only one of completion of color attachment outputs still we need one member array
    const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    // declare and memset and initliase VkSubmitInfo structure
    VkSubmitInfo vksubmitInfo;
    memset((void*)&vksubmitInfo, 0, sizeof(VkSubmitInfo));

    vksubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    vksubmitInfo.pNext = NULL;
    vksubmitInfo.pWaitDstStageMask = &waitDstStageMask;
    vksubmitInfo.waitSemaphoreCount = 1;
    vksubmitInfo.pWaitSemaphores = &vkSemaphore_backbuffer;
    vksubmitInfo.commandBufferCount = 1;
    vksubmitInfo.pCommandBuffers = &vkCommandBuffer_Array[currentImageIndex];
    vksubmitInfo.signalSemaphoreCount = 1;
    vksubmitInfo.pSignalSemaphores = &vkSemaphore_rendercomplete;


    // now submit above work to the queue
    vkresult = vkQueueSubmit(vkQueue, 1, &vksubmitInfo, vkFence_Array[currentImageIndex]);
    if (vkresult != VK_SUCCESS)
    {
        printf( "render() : vkQueueSubmit failed with error: %d\n", vkresult);
        return(vkresult);
    }

    // we are going to present rendered image after declaring and initalising vkPresentInfoKHR structure
    VkPresentInfoKHR vkPresentInfoKHR;
    memset((void*)&vkPresentInfoKHR, 0, sizeof(VkPresentInfoKHR));

    vkPresentInfoKHR.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    vkPresentInfoKHR.pNext = NULL;
    vkPresentInfoKHR.swapchainCount = 1;
    vkPresentInfoKHR.pSwapchains = &vkSwapchainKHR;
    vkPresentInfoKHR.pImageIndices = &currentImageIndex;
    vkPresentInfoKHR.waitSemaphoreCount = 1;
    vkPresentInfoKHR.pWaitSemaphores = &vkSemaphore_rendercomplete;


    // now present the queue
    vkresult = vkQueuePresentKHR(vkQueue, &vkPresentInfoKHR);
    if (vkresult != VK_SUCCESS)
    {
        if (vkresult != VK_SUCCESS && vkresult != VK_SUBOPTIMAL_KHR && vkresult != VK_ERROR_OUT_OF_DATE_KHR)
        {
            printf( "render() : vkQueuePresentKHR failed with error: %d\n", vkresult);
            return(vkresult);
        }
    }


    vkresult = [self updateUniformbuffer];
    if(vkresult != VK_SUCCESS)
    {
        printf( "render() : updateUniformbuffer() failed with error: %d\n", vkresult);
        return(vkresult);
    }

    // here there will be your drawing code
    vkDeviceWaitIdle(vkDevice);


    return(vkresult);

}


-(void)update
{
    // code
}



-(void)uninitialise
{

    // Code
    // if window is in full screen mode then first exit from full screen mode


    //No need to destroy/uninitialize vkQueue


    //Vulkan related any destruction *HAS TO BE AFTER VkDevice*
    //because any resources related to vulkan device ae all done so resource freeing

    //Destroy vulkan device
    if (vkDevice)
    {
        vkDeviceWaitIdle(vkDevice);
        printf( "\n vkDeviceWaitIdle() is Done\n");


        //Free swapchain Images

        for (uint32_t i = 0; i < swapchainImageCount; i++)
        {
            vkDestroyFence(vkDevice, vkFence_Array[i], NULL);
            printf( "vkDestroyFence vkFence_array i = %d Completedestroyed!\n", i);
        }
        if (vkFence_Array)
        {
            free(vkFence_Array);
            vkFence_Array = NULL;
            printf( "\nFree vkFence_array freed\n");
        }

        if (vkSemaphore_rendercomplete)
        {
            vkDestroySemaphore(vkDevice, vkSemaphore_rendercomplete, NULL);
            vkSemaphore_rendercomplete = VK_NULL_HANDLE;
            printf( "\nFree vkSemaphore_RenderComplete freed\n");


        }
        if (vkSemaphore_backbuffer)
        {
            vkDestroySemaphore(vkDevice, vkSemaphore_backbuffer, NULL);
            vkSemaphore_backbuffer = VK_NULL_HANDLE;
            printf( "\nFree vkSemaphore_backbuffer freed\n");

        }

        //Framebuffer free
        for (uint32_t i = 0; i < swapchainImageCount; i++)
        {
            vkDestroyFramebuffer(vkDevice, vkFramebuffer_Array[i], NULL);

        }

        if (vkFramebuffer_Array)
        {
            free(vkFramebuffer_Array);
            vkFramebuffer_Array = NULL;
            printf( "\nFree commandbuffers freed\n");

        }

        if (vkDescriptorSetLayout)
        {
            vkDestroyDescriptorSetLayout(vkDevice, vkDescriptorSetLayout, NULL);
            vkDescriptorSetLayout = VK_NULL_HANDLE;
            printf( "\nFree vkDescriptorSetLayout freed\n");
        }

        if (vkPipelineLayout)
        {
            vkDestroyPipelineLayout(vkDevice, vkPipelineLayout, NULL);
            vkPipelineLayout = VK_NULL_HANDLE;
            printf( "\nFree vkPipelineLayout freed\n");
        }

        if (vkPipeline)
        {
            vkDestroyPipeline(vkDevice, vkPipeline, NULL);
            vkPipeline = VK_NULL_HANDLE;
            printf( "\nFree vkPipeline: Pipeline freed successfully.\n");
        }


        if (vkRenderpass)
        {
            vkDestroyRenderPass(vkDevice, vkRenderpass, NULL);
            vkRenderpass = VK_NULL_HANDLE;
            printf( "\nFree vkRenderpass freed\n");
        }

        // destroy descriptorpool
        // when descriptor pull is destroyed descriptor set created by that pull get destroy imlicitly
        if (vkDescriptorPool)
        {
            vkDestroyDescriptorPool(vkDevice, vkDescriptorPool, NULL);
            vkDescriptorPool = VK_NULL_HANDLE;
            vkDescriptorSet = VK_NULL_HANDLE;
            printf( "\nFreed vkDescriptorPool and vkDescriptorSet\n");
        }


        // destroy shader modules
        if (vkShaderModule_fragment_shader)
        {
            vkDestroyShaderModule(vkDevice, vkShaderModule_fragment_shader, NULL);
            vkShaderModule_fragment_shader = NULL;
            printf( "\nFree vkShaderModule_fragment_shader freed\n");
        }

        if (vkShaderModule_vertex_shader)
        {
            vkDestroyShaderModule(vkDevice, vkShaderModule_vertex_shader, NULL);
            vkShaderModule_vertex_shader = NULL;
            printf( "\nFree vkShaderModule_vertex_shader freed\n");
        }

        // Destroy uniform buffer
        if (uniformData.vkBuffer)
        {
            vkDestroyBuffer(vkDevice, uniformData.vkBuffer, NULL);
            uniformData.vkBuffer = VK_NULL_HANDLE;
            printf( "\nFreed uniformData.vkBuffer \n");
        }

        if (uniformData.vkDeviceMemory)
        {
            vkFreeMemory(vkDevice, uniformData.vkDeviceMemory, NULL);
            uniformData.vkDeviceMemory = VK_NULL_HANDLE;
            printf( "\nFreed uniformData.vkDeviceMemory \n");
        }

        // free color buffer
        if (vertexData_Color.vkDeviceMemory)
        {
            vkFreeMemory(vkDevice, vertexData_Color.vkDeviceMemory, NULL);
            vertexData_Color.vkDeviceMemory = VK_NULL_HANDLE;
            printf( "\nFree vertexData_Color.vkDeviceMemory freed\n");

        }

        if (vertexData_Color.vkBuffer)
        {
            vkDestroyBuffer(vkDevice, vertexData_Color.vkBuffer, NULL);
            vertexData_Color.vkBuffer = VK_NULL_HANDLE;
            printf( "\nFreevertexData_Color.vkBuffer freed\n");

        }


        if (vertexData_Position.vkDeviceMemory)
        {
            vkFreeMemory(vkDevice, vertexData_Position.vkDeviceMemory, NULL);
            vertexData_Position.vkDeviceMemory = VK_NULL_HANDLE;
            printf( "\nFree vertexData_Position.vkDeviceMemory freed\n");

        }

        if (vertexData_Position.vkBuffer)
        {
            vkDestroyBuffer(vkDevice, vertexData_Position.vkBuffer, NULL);
            vertexData_Position.vkBuffer = VK_NULL_HANDLE;
            printf( "\nFree vertexData_Position.vkBuffer freed\n");

        }


        for (uint32_t i = 0; i < swapchainImageCount; i++)
        {
            vkFreeCommandBuffers(vkDevice, vkcommandpool, 1, &vkCommandBuffer_Array[i]);
            //vkDestroyImageView(vkDevice, swapchainImageView_array[i], NULL);
            printf( "\nFree commandbuffers freed\n");
        }
        if (vkCommandBuffer_Array)
        {
            free(vkCommandBuffer_Array);
            vkCommandBuffer_Array = NULL;
        }

        vkDestroyCommandPool(vkDevice, vkcommandpool, NULL);
        //vkCommandPool = VK_NULL_HANDLE;
        printf( "\n vkCommandPool is Freed\n");


        for (uint32_t i = 0; i < swapchainImageCount; i++)
        {
            vkDestroyImageView(vkDevice, SwapchainImageView_Array[i], NULL);
            printf( "\nFree swapchainImage_array images freed\n");
        }
        if (SwapchainImageView_Array)
        {
            free(SwapchainImageView_Array);
            SwapchainImageView_Array = NULL;
        }


        /*for (uint32_t i = 0; i < swapchainImageCount; i++)
        {
            vkDestroyImage(vkDevice, SwapchainImage_Array[i], NULL);
            printf( "\nFree SwapchainImage_Array images freed\n");
        }*/

        if (SwapchainImage_Array)
        {
            free(SwapchainImage_Array);
            SwapchainImage_Array = NULL;
        }

        //Destroy Fences

        //Command buffer free
        //actual array free

        //destroy     swapchain
        if (vkSwapchainKHR)
        {
            vkDestroySwapchainKHR(vkDevice, vkSwapchainKHR, NULL);
            vkSwapchainKHR = NULL;        //Bhanda swachha
            printf( "\n vkSwapchainKHR is Freed\n");

        }

        vkDestroyDevice(vkDevice, NULL);
        vkDevice = VK_NULL_HANDLE;
        printf( "\n vkDestroyDevice() is Done\n");

    }
    //No need to Destroy selected physical device


    if (vkSurfaceKHR)
    {
        vkDestroySurfaceKHR(vkInstance, vkSurfaceKHR, NULL);
        vkSurfaceKHR = VK_NULL_HANDLE;
        printf( "\nvkDestroySurfaceKHR Done\n");
    }

    if (vkDebugUtilsMessengerEXT_handle && vkDestroyDebugUtilsMessengerEXT_fnptr)
    {
        vkDestroyDebugUtilsMessengerEXT_fnptr(vkInstance,
                                              vkDebugUtilsMessengerEXT_handle,
                                              NULL);

        vkDebugUtilsMessengerEXT_handle = VK_NULL_HANDLE;
        vkDestroyDebugUtilsMessengerEXT_fnptr = NULL;

        printf("vkDestroyDebugUtilsMessengerEXT_fnptr Done\n");
    }

    //uninitialize/destroy vulkan instance
    if (vkInstance)
    {
        vkDestroyInstance(vkInstance, NULL);
        vkInstance = VK_NULL_HANDLE;
        printf( "\nvkDestroyInstance Done\n");
    }

}

/////////////////////////////////////////////////////.   Vulkan related global function definitions   ./////////////////////////////////////////////////////

-(VkResult)createVulkanInstance
{
    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    // Fill instance extensions
    vkresult = [self fillExtensionNames];
    if (vkresult != VK_SUCCESS)
    {
        printf("createVulkanInstance() : fillExtensionNames() failed.\n");
        return vkresult;
    }
    printf("createVulkanInstance() : fillExtensionNames() succeeded.\n");

    // Fill validation layers
    if (bValidation == YES)
    {
        vkresult = [self fillValidationLayerNames];
        if (vkresult != VK_SUCCESS)
        {
            printf("createVulkanInstance() : fillValidationLayerNames() failed (%d).\n", vkresult);
            return vkresult;
        }

        printf("createVulkanInstance() : fillValidationLayerNames() succeeded.\n");
    }

    // Application info
    VkApplicationInfo vkApplicationInfo;
    memset(&vkApplicationInfo, 0, sizeof(VkApplicationInfo));

    vkApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vkApplicationInfo.pApplicationName = gpszAppName;
    vkApplicationInfo.applicationVersion = VK_MAKE_API_VERSION(0,1,0,0);
    vkApplicationInfo.pEngineName = gpszAppName;
    vkApplicationInfo.engineVersion = VK_MAKE_API_VERSION(0,1,0,0);
    vkApplicationInfo.apiVersion = VK_API_VERSION_1_2;

    // Instance create info
    VkInstanceCreateInfo vkInstanceCreateInfo;
    memset(&vkInstanceCreateInfo, 0, sizeof(VkInstanceCreateInfo));

    vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vkInstanceCreateInfo.pApplicationInfo = &vkApplicationInfo;

    // Enable portability flag only if extension exists
    if (vulkanPortabilityEnumerationExtensionFound == VK_TRUE)
    {
        vkInstanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }

    vkInstanceCreateInfo.enabledExtensionCount = enabledInstanceExtensionCount;
    vkInstanceCreateInfo.ppEnabledExtensionNames = enabledInstanceExtensionNames_array;

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

    // Create instance
    vkresult = vkCreateInstance(&vkInstanceCreateInfo, NULL, &vkInstance);

    if (vkresult != VK_SUCCESS)
    {
        printf("createVulkanInstance() : vkCreateInstance() failed. VkResult = %d\n", vkresult);
        return vkresult;
    }

    printf("createVulkanInstance() : vkCreateInstance() succeeded.\n");

    // Create debug messenger
    if (bValidation == YES)
    {
        vkresult = [self createValidationCallbackFunction];
        if (vkresult != VK_SUCCESS)
        {
            printf("createVulkanInstance() : createValidationCallbackFunction() failed (%d).\n", vkresult);
            return vkresult;
        }

        printf("createVulkanInstance() : createValidationCallbackFunction() succeeded.\n");
    }

    return VK_SUCCESS;
}

-(VkResult)fillExtensionNames
{
    VkResult vkresult = VK_SUCCESS;

    // Reset enabled extension count
    enabledInstanceExtensionCount = 0;
    vulkanPortabilityEnumerationExtensionFound = VK_FALSE;

    uint32_t instanceExtensionCount = 0;

    vkresult = vkEnumerateInstanceExtensionProperties(NULL,
                                                      &instanceExtensionCount,
                                                      NULL);

    if (vkresult != VK_SUCCESS)
    {
        printf("fillExtensionNames() : First vkEnumerateInstanceExtensionProperties() failed (%d)\n", vkresult);
        return vkresult;
    }

    printf("fillExtensionNames() : Extension Count = %u\n", instanceExtensionCount);

    VkExtensionProperties *vkExtensionProperties_array =
        (VkExtensionProperties *)malloc(sizeof(VkExtensionProperties) * instanceExtensionCount);

    if (vkExtensionProperties_array == NULL)
    {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    vkresult = vkEnumerateInstanceExtensionProperties(NULL,
                                                      &instanceExtensionCount,
                                                      vkExtensionProperties_array);

    if (vkresult != VK_SUCCESS)
    {
        free(vkExtensionProperties_array);
        return vkresult;
    }

    VkBool32 surfaceFound = VK_FALSE;
    VkBool32 metalSurfaceFound = VK_FALSE;
    VkBool32 debugUtilsFound = VK_FALSE;

    for (uint32_t i = 0; i < instanceExtensionCount; i++)
    {
        printf("fillExtensionNames() : Vulkan Extension = %s\n",
               vkExtensionProperties_array[i].extensionName);

        if (strcmp(vkExtensionProperties_array[i].extensionName,
                   VK_KHR_SURFACE_EXTENSION_NAME) == 0)
        {
            surfaceFound = VK_TRUE;
            enabledInstanceExtensionNames_array[enabledInstanceExtensionCount++] =
                VK_KHR_SURFACE_EXTENSION_NAME;
        }

        else if (strcmp(vkExtensionProperties_array[i].extensionName,
                        VK_EXT_METAL_SURFACE_EXTENSION_NAME) == 0)
        {
            metalSurfaceFound = VK_TRUE;
            enabledInstanceExtensionNames_array[enabledInstanceExtensionCount++] =
                VK_EXT_METAL_SURFACE_EXTENSION_NAME;
        }

        else if (strcmp(vkExtensionProperties_array[i].extensionName,
                        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0)
        {
            vulkanPortabilityEnumerationExtensionFound = VK_TRUE;

            enabledInstanceExtensionNames_array[enabledInstanceExtensionCount++] =
                VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
        }

        else if (strcmp(vkExtensionProperties_array[i].extensionName,
                        VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
        {
            debugUtilsFound = VK_TRUE;

            if (bValidation == YES)
            {
                enabledInstanceExtensionNames_array[enabledInstanceExtensionCount++] =
                    VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
            }
        }
    }

    free(vkExtensionProperties_array);

    if (surfaceFound == VK_FALSE)
    {
        printf("fillExtensionNames() : %s not found\n",
               VK_KHR_SURFACE_EXTENSION_NAME);

        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (metalSurfaceFound == VK_FALSE)
    {
        printf("fillExtensionNames() : %s not found\n",
               VK_EXT_METAL_SURFACE_EXTENSION_NAME);

        return VK_ERROR_INITIALIZATION_FAILED;
    }

    if (vulkanPortabilityEnumerationExtensionFound)
    {
        printf("fillExtensionNames() : %s found\n",
               VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    }
    else
    {
        printf("fillExtensionNames() : %s not supported (continuing)\n",
               VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    }

    if (bValidation == YES)
    {
        if (debugUtilsFound == VK_FALSE)
        {
            printf("fillExtensionNames() : %s not found\n",
                   VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

            return VK_ERROR_INITIALIZATION_FAILED;
        }

        printf("fillExtensionNames() : %s found\n",
               VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    printf("Enabled Instance Extensions:\n");

    for (uint32_t i = 0; i < enabledInstanceExtensionCount; i++)
    {
        printf("%u : %s\n",
               i,
               enabledInstanceExtensionNames_array[i]);
    }

    return VK_SUCCESS;
}


-(VkResult)fillValidationLayerNames
{
    // Variable declarations
    VkResult vkresult = VK_SUCCESS;

    enabledValidationLayerCount = 0;

    uint32_t validationLayerCount = 0;

    // Get validation layer count
    vkresult = vkEnumerateInstanceLayerProperties(&validationLayerCount, NULL);
    if (vkresult != VK_SUCCESS)
    {
        printf("fillValidationLayerNames() : First vkEnumerateInstanceLayerProperties() failed (%d)\n", vkresult);
        return vkresult;
    }

    printf("fillValidationLayerNames() : Layer Count = %u\n", validationLayerCount);

    if (validationLayerCount == 0)
    {
        printf("fillValidationLayerNames() : No validation layers available. Validation disabled.\n");
        bValidation = NO;
        return VK_SUCCESS;
    }

    // Allocate memory
    VkLayerProperties *vkLayerProperties_Array =
        (VkLayerProperties *)malloc(sizeof(VkLayerProperties) * validationLayerCount);

    if (vkLayerProperties_Array == NULL)
    {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    // Enumerate validation layers
    vkresult = vkEnumerateInstanceLayerProperties(&validationLayerCount,
                                                  vkLayerProperties_Array);

    if (vkresult != VK_SUCCESS)
    {
        free(vkLayerProperties_Array);
        return vkresult;
    }

    VkBool32 validationLayerFound = VK_FALSE;

    for (uint32_t i = 0; i < validationLayerCount; i++)
    {
        printf("fillValidationLayerNames() : Vulkan Layer name = %s\n",
               vkLayerProperties_Array[i].layerName);

        if (strcmp(vkLayerProperties_Array[i].layerName,
                   "VK_LAYER_KHRONOS_validation") == 0)
        {
            validationLayerFound = VK_TRUE;
        }
    }

    free(vkLayerProperties_Array);

    if (validationLayerFound == VK_TRUE)
    {
        enabledValidationLayerNames_Array[0] = "VK_LAYER_KHRONOS_validation";
        enabledValidationLayerCount = 1;

        printf("fillValidationLayerNames() : VK_LAYER_KHRONOS_validation found.\n");
    }
    else
    {
        printf("fillValidationLayerNames() : VK_LAYER_KHRONOS_validation not found.\n");
        printf("fillValidationLayerNames() : Validation disabled.\n");

        bValidation = NO;
        enabledValidationLayerCount = 0;
    }

    for (uint32_t i = 0; i < enabledValidationLayerCount; i++)
    {
        printf("fillValidationLayerNames() : Enabled Layer = %s\n",
               enabledValidationLayerNames_Array[i]);
    }

    return VK_SUCCESS;
}


-(VkResult)createValidationCallbackFunction
{
    // Function declaration
    VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallBack(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);

    // Variable declarations
    VkResult vkresult = VK_SUCCESS;

    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT_fnptr = NULL;

    // Get vkCreateDebugUtilsMessengerEXT function pointer
    vkCreateDebugUtilsMessengerEXT_fnptr =
        (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT");

    if (vkCreateDebugUtilsMessengerEXT_fnptr == NULL)
    {
        printf("createValidationCallbackFunction() : Failed to get vkCreateDebugUtilsMessengerEXT function pointer.\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    else
    {
        printf("createValidationCallbackFunction() : Successfully got vkCreateDebugUtilsMessengerEXT function pointer.\n");
    }

    // Get vkDestroyDebugUtilsMessengerEXT function pointer
    vkDestroyDebugUtilsMessengerEXT_fnptr =
        (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugUtilsMessengerEXT");

    if (vkDestroyDebugUtilsMessengerEXT_fnptr == NULL)
    {
        printf("createValidationCallbackFunction() : Failed to get vkDestroyDebugUtilsMessengerEXT function pointer.\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    else
    {
        printf("createValidationCallbackFunction() : Successfully got vkDestroyDebugUtilsMessengerEXT function pointer.\n");
    }

    // Fill create info
    VkDebugUtilsMessengerCreateInfoEXT vkDebugUtilsMessengerCreateInfoEXT;
    memset(&vkDebugUtilsMessengerCreateInfoEXT, 0, sizeof(VkDebugUtilsMessengerCreateInfoEXT));

    vkDebugUtilsMessengerCreateInfoEXT.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    vkDebugUtilsMessengerCreateInfoEXT.pNext = NULL;
    vkDebugUtilsMessengerCreateInfoEXT.flags = 0;

    vkDebugUtilsMessengerCreateInfoEXT.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    vkDebugUtilsMessengerCreateInfoEXT.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    vkDebugUtilsMessengerCreateInfoEXT.pfnUserCallback =
        debugUtilsMessengerCallBack;

    vkDebugUtilsMessengerCreateInfoEXT.pUserData = NULL;

    // Create Debug Messenger
    vkresult = vkCreateDebugUtilsMessengerEXT_fnptr(
        vkInstance,
        &vkDebugUtilsMessengerCreateInfoEXT,
        NULL,
        &vkDebugUtilsMessengerEXT_handle);

    if (vkresult != VK_SUCCESS)
    {
        printf("createValidationCallbackFunction() : vkCreateDebugUtilsMessengerEXT() failed with error code %d\n", vkresult);
        return vkresult;
    }
    else
    {
        printf("createValidationCallbackFunction() : vkCreateDebugUtilsMessengerEXT() succeeded.\n");
    }

    printf("createValidationCallbackFunction() : ******************************** EXIT ********************************\n");

    return vkresult;
}

-(VkResult)getSupportedSurface
{
    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    // Metal Surface Create Info
    VkMetalSurfaceCreateInfoEXT vkMetalSurfaceCreateInfoEXT;
    memset((void *)&vkMetalSurfaceCreateInfoEXT, 0, sizeof(VkMetalSurfaceCreateInfoEXT));

    vkMetalSurfaceCreateInfoEXT.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
    vkMetalSurfaceCreateInfoEXT.pNext = NULL;
    vkMetalSurfaceCreateInfoEXT.flags = 0;
    vkMetalSurfaceCreateInfoEXT.pLayer = (CAMetalLayer *)[self layer];

    // Get function pointer
    PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT_fnptr = NULL;

    vkCreateMetalSurfaceEXT_fnptr =
        (PFN_vkCreateMetalSurfaceEXT)
        vkGetInstanceProcAddr(vkInstance, "vkCreateMetalSurfaceEXT");

    if (vkCreateMetalSurfaceEXT_fnptr == NULL)
    {
        printf("getSupportedSurface() : Failed to get vkCreateMetalSurfaceEXT function pointer.\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    else
    {
        printf("getSupportedSurface() : Successfully got vkCreateMetalSurfaceEXT function pointer.\n");
    }

    // Create Metal surface
    vkresult = vkCreateMetalSurfaceEXT_fnptr(
                    vkInstance,
                    &vkMetalSurfaceCreateInfoEXT,
                    NULL,
                    &vkSurfaceKHR);

    if (vkresult != VK_SUCCESS)
    {
        printf("getSupportedSurface() : vkCreateMetalSurfaceEXT() failed with error code %d.\n", vkresult);
        return vkresult;
    }
    else
    {
        printf("getSupportedSurface() : vkCreateMetalSurfaceEXT() succeeded.\n");
    }

    return vkresult;
}

-(VkResult) getPhysicalDevice
{
    VkResult vkresult = VK_SUCCESS;

    vkresult = vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, NULL);
    if (vkresult != VK_SUCCESS)
    {
        printf( "getPhysicalDevice() 1st call : vkEnumeratePhysicalDevices Failed (%d)\n", vkresult);
        return vkresult;
    }
    else if (physicalDeviceCount == 0)
    {
        printf( "getPhysicalDevice() 1st call : vkEnumeratePhysicalDevices resulted in zero physical Devices \n");
        return vkresult;
    }
    else
    {
        printf( "getPhysicalDevice() 1st call : vkEnumeratePhysicalDevices succeeded \n");
    }

    vkPhysicalDevice_Array = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * physicalDeviceCount);
    if (vkPhysicalDevice_Array == NULL)
    {
        printf( "Memory allocation failed for vkPhysicalDevice_Array\n");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    vkresult = vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, vkPhysicalDevice_Array);
    if (vkresult != VK_SUCCESS)
    {
        printf( "getPhysicalDevice() 2nd call : vkEnumeratePhysicalDevices Failed (%d)\n", vkresult);
        free(vkPhysicalDevice_Array);  // Free on error
        return vkresult;
    }
    else
    {
        printf( "getPhysicalDevice() 2nd call  : vkEnumeratePhysicalDevices succeeded \n");
    }

    VkBool32 bFound = VK_FALSE;
    for (uint32_t i = 0; i < physicalDeviceCount; i++)
    {
        uint32_t queueCount = UINT32_MAX;

        vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice_Array[i], &queueCount, NULL);

        VkQueueFamilyProperties* vkQueueFamilyProperties_Array = NULL;
        vkQueueFamilyProperties_Array = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * queueCount);
        if (vkQueueFamilyProperties_Array == NULL)  // Check memory allocation
        {
            printf( "Memory allocation failed for vkQueueFamilyProperties_Array\n");
            free(vkPhysicalDevice_Array);  // Free previous allocations
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }

        vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice_Array[i], &queueCount, vkQueueFamilyProperties_Array);

        VkBool32* isQueueSurface_Array = NULL;

        // Allocate memory for queue surface support
        isQueueSurface_Array = (VkBool32*)malloc(sizeof(VkBool32) * queueCount);
        if (isQueueSurface_Array == NULL)  // Check memory allocation
        {
            printf( "Memory allocation failed for isQueueSurface_Array\n");
            free(vkQueueFamilyProperties_Array);  // Free previous allocations
            free(vkPhysicalDevice_Array);
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }

        for (uint32_t j = 0; j < queueCount; j++)
        {
            vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice_Array[i], j, vkSurfaceKHR, &isQueueSurface_Array[j]);
        }

        for (uint32_t j = 0; j < queueCount; j++)
        {
            if (vkQueueFamilyProperties_Array[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                if (isQueueSurface_Array[j] == VK_TRUE)
                {
                    vkPhysicalDevice_selected = vkPhysicalDevice_Array[i];
                    graphicsQueueFamilyIndex_Selected = j;
                    bFound = VK_TRUE;
                    break;  // Exit the loop when a suitable queue is found
                }
            }
        }

        if (isQueueSurface_Array)
        {
            free(isQueueSurface_Array);
            isQueueSurface_Array = NULL;
        }

        if (vkQueueFamilyProperties_Array)
        {
            free(vkQueueFamilyProperties_Array);
            vkQueueFamilyProperties_Array = NULL;
        }

        if (bFound == VK_TRUE)
        {
            break;
        }
    }

    if (bFound == VK_TRUE)
    {
        printf( "getPhysicalDevice succeeded to select the required physical device with graphics enabled\n");
    }
    else
    {
        printf( "getPhysicalDevice() Failed to find a graphics supported physical device\n");
        vkresult = VK_ERROR_INITIALIZATION_FAILED;

        if (vkPhysicalDevice_Array)
        {
            free(vkPhysicalDevice_Array);
            vkPhysicalDevice_Array = NULL;
        }

        return vkresult;
    }

    memset((void*)&vkPhysicalDeviceMemoryProperties, 0, sizeof(VkPhysicalDeviceMemoryProperties));
    vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice_selected, &vkPhysicalDeviceMemoryProperties);

    VkPhysicalDeviceFeatures vkPhysicalDeviceFeatures;
    memset((void*)&vkPhysicalDeviceFeatures, 0, sizeof(VkPhysicalDeviceFeatures));

    // Retrieve features of the selected physical device
    vkGetPhysicalDeviceFeatures(vkPhysicalDevice_selected, &vkPhysicalDeviceFeatures);

    // Check for tessellation shader support
    if (vkPhysicalDeviceFeatures.tessellationShader)
    {
        printf( "getPhysicalDevice supports tessellation shader\n");
    }
    else
    {
        printf( "getPhysicalDevice does not support tessellation shader\n");
    }

    // Check for geometry shader support
    if (vkPhysicalDeviceFeatures.geometryShader)
    {
        printf( "getPhysicalDevice supports geometry shader\n");
    }
    else
    {
        printf( "getPhysicalDevice does not support geometry shader\n");
    }

    return vkresult;
}

-(VkResult) printVkInfo
{
    VkResult vkresult = VK_SUCCESS;

    // Header for Vulkan Information
    printf( "********************** VULKAN INFORMATION *****************************\n");

    for (uint32_t i = 0; i < physicalDeviceCount; i++)
    {
        VkPhysicalDeviceProperties vkPhysicalDeviceProperties;
        memset(&vkPhysicalDeviceProperties, 0, sizeof(VkPhysicalDeviceProperties));

        // Correct function name
        vkGetPhysicalDeviceProperties(vkPhysicalDevice_Array[i], &vkPhysicalDeviceProperties);

        uint32_t majorVersion = VK_API_VERSION_MAJOR(vkPhysicalDeviceProperties.apiVersion);
        uint32_t minorVersion = VK_API_VERSION_MINOR(vkPhysicalDeviceProperties.apiVersion);
        uint32_t patchVersion = VK_API_VERSION_PATCH(vkPhysicalDeviceProperties.apiVersion);

        // API VERSION
        printf( "apiVersion    = %d.%d.%d\n", majorVersion, minorVersion, patchVersion);

        // DEVICE NAME
        printf( "deviceName    = %s\n", vkPhysicalDeviceProperties.deviceName);

        // DEVICE TYPE
        printf( "deviceType    = ");
        switch (vkPhysicalDeviceProperties.deviceType)
        {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            printf( "integratedGPU (iGPU)\n");  // Correct spelling
            break;

        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            printf( "discreteGPU (dGPU)\n");
            break;

        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            printf( "virtualGPU (vGPU)\n");
            break;

        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            printf( "CPU\n");
            break;

        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            printf( "Other\n");
            break;

        default:
            printf( "UNKNOWN\n");
        }

        // Vendor ID (Hexadecimal)
        printf( "vendorID      = 0x%04x\n", vkPhysicalDeviceProperties.vendorID);

        // Device ID
        printf( "deviceID      = 0x%04x\n", vkPhysicalDeviceProperties.deviceID);

        printf( "--------------------------------------------------------------------\n");
    }

    // Free allocated memory if necessary
    if (vkPhysicalDevice_Array)
    {
        free(vkPhysicalDevice_Array);
        printf( "printVkInfo(): Physical Device memory freed.\n");
        vkPhysicalDevice_Array = NULL;
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
        printf( "fillDeviceExtensionNames(): vkEnumerateDeviceExtensionProperties() 1st call failed with error code %d\n", vkresult);
        return vkresult;
    }
    printf( "fillDeviceExtensionNames(): vkEnumerateDeviceExtensionProperties() 1st call succeeded. Device extension count: %d\n", deviceExtensionCount);

    // Step 2: Allocate memory for extension properties
    VkExtensionProperties* vkExtensionProperties_array = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * deviceExtensionCount);
    if (!vkExtensionProperties_array)
    {
        printf( "fillDeviceExtensionNames(): Memory allocation failed for vkExtensionProperties_array\n");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    // Query extension properties
    vkresult = vkEnumerateDeviceExtensionProperties(vkPhysicalDevice_selected, NULL, &deviceExtensionCount, vkExtensionProperties_array);
    if (vkresult != VK_SUCCESS)
    {
        printf( "fillDeviceExtensionNames(): vkEnumerateDeviceExtensionProperties() 2nd call failed with error code %d\n", vkresult);
        free(vkExtensionProperties_array);
        return vkresult;
    }
    printf( "fillDeviceExtensionNames(): vkEnumerateDeviceExtensionProperties() 2nd call succeeded\n");

    // Step 3: Allocate memory for extension name strings
    char** DeviceExtensionNames_array = (char**)malloc(sizeof(char*) * deviceExtensionCount);
    if (!DeviceExtensionNames_array)
    {
        printf( "fillDeviceExtensionNames(): Memory allocation failed for DeviceExtensionNames_array\n");
        free(vkExtensionProperties_array);
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (uint32_t i = 0; i < deviceExtensionCount; i++)
    {
        DeviceExtensionNames_array[i] = (char*)malloc(strlen(vkExtensionProperties_array[i].extensionName) + 1);
        if (!DeviceExtensionNames_array[i])
        {
            printf( "fillDeviceExtensionNames(): Memory allocation failed for DeviceExtensionNames_array[%d]\n", i);
            // Free previously allocated memory before returning
            for (uint32_t j = 0; j < i; j++)
                free(DeviceExtensionNames_array[j]);
            free(DeviceExtensionNames_array);
            free(vkExtensionProperties_array);
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }

        strcpy(DeviceExtensionNames_array[i], vkExtensionProperties_array[i].extensionName);
        printf( "fillDeviceExtensionNames(): Vulkan Device Extension = %s\n", DeviceExtensionNames_array[i]);
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
        printf( "fillDeviceExtensionNames(): VK_KHR_SWAPCHAIN_EXTENSION_NAME not found\n");
        return vkresult;
    }
    printf( "fillDeviceExtensionNames(): VK_KHR_SWAPCHAIN_EXTENSION_NAME found\n");

    if (vulkanPortabilitySubsetExtensionFound == VK_FALSE)
    {
        vkresult = VK_ERROR_INITIALIZATION_FAILED; // Return hardcoded failure
        printf( "fillDeviceExtensionNames(): VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME not found\n");
        return vkresult;
    }
    printf( "fillDeviceExtensionNames(): VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME found\n");


    // Step 7: Print enabled extension names
    for (uint32_t i = 0; i < enableDeviceExtensionCount; i++)
    {
        printf( "fillDeviceExtensionNames(): Enabled Vulkan Device Extension = %s\n", enabledDeviceExtensionNames_array[i]);
    }

    return vkresult;
}

-(VkResult) createVulkanDevice
{
    // variable declaration
    VkResult vkresult = VK_SUCCESS;

    // fill device extension names
    vkresult = [self fillDeviceExtensionNames];
    if (vkresult != VK_SUCCESS)
    {
        printf( "createVulkanDevice() : fillDeviceExtensionNames function failed\n");
        return(vkresult);
    }
    else
    {
        printf( "createVulkanDevice() : fillDeviceExtensionNames() succeeded\n");
    }

    float queuePriority[1];
    queuePriority[0] = 1.0f;

    // newly added code
    VkDeviceQueueCreateInfo vkDeviceQueueCreateInfo;
    memset((void*)&vkDeviceQueueCreateInfo, 0, sizeof(VkDeviceQueueCreateInfo));

    vkDeviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    vkDeviceQueueCreateInfo.pNext = NULL;
    vkDeviceQueueCreateInfo.flags = 0;
    vkDeviceQueueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex_Selected;
    vkDeviceQueueCreateInfo.queueCount = 1;
    vkDeviceQueueCreateInfo.pQueuePriorities = queuePriority;


    // initialise vkDeviceCreateInfo sttucture
    VkDeviceCreateInfo vkDeviceCreateInfo;
    memset((void*)&vkDeviceCreateInfo, 0, sizeof(VkDeviceCreateInfo));

    vkDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    vkDeviceCreateInfo.pNext = NULL;
    vkDeviceCreateInfo.flags = 0;
    vkDeviceCreateInfo.enabledExtensionCount = enableDeviceExtensionCount;
    vkDeviceCreateInfo.ppEnabledExtensionNames = enabledDeviceExtensionNames_array;
    vkDeviceCreateInfo.enabledLayerCount = 0;
    vkDeviceCreateInfo.ppEnabledLayerNames = NULL;
    vkDeviceCreateInfo.pEnabledFeatures = NULL;
    vkDeviceCreateInfo.queueCreateInfoCount = 1;
    vkDeviceCreateInfo.pQueueCreateInfos = &vkDeviceQueueCreateInfo;


    vkresult = vkCreateDevice(vkPhysicalDevice_selected, &vkDeviceCreateInfo, NULL, &vkDevice);
    if (vkresult != VK_SUCCESS)
    {
        printf( "createVulkanDevice() : vkCreateDevice function failed\n");
        return(vkresult);
    }
    else
    {
        printf( "createVulkanDevice() : vkCreateDevice() succeeded\n");
    }

    return vkresult;
}


-(void) getDeviceQueue
{
    vkGetDeviceQueue(vkDevice, graphicsQueueFamilyIndex_Selected, 0, &vkQueue);

    if (vkQueue == VK_NULL_HANDLE)
    {
        printf( "getDeviceQueue() : vkGetDeviceQueue returned NULL for vkQueue. Check if the device was created with the correct queue family.\n");
        return;
    }
    else
    {
        printf( "getDeviceQueue() : vkGetDeviceQueue() succeeded\n");
    }
}

-(VkResult) getPhysicalDeviceSurfaceFormatAndColorSpace
{
    // Variable declaration
    VkResult vkresult = VK_SUCCESS;
    uint32_t formatCount = 0;

    // First call to get the number of supported surface formats
    vkresult = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice_selected, vkSurfaceKHR, &formatCount, NULL);
    if (vkresult != VK_SUCCESS)
    {
        printf( "getPhysicalDeviceSurfaceFormatAndColorSpace(): vkGetPhysicalDeviceSurfaceFormatsKHR() failed\n");
        return vkresult;
    }
    else
    {
        printf( "getPhysicalDeviceSurfaceFormatAndColorSpace() : vkGetPhysicalDeviceSurfaceFormatsKHR() succeeded\n");
    }

    // Check if no surface formats are found
    if (formatCount == 0)
    {
        printf( "getPhysicalDeviceSurfaceFormatAndColorSpace(): No surface formats found\n");
        return VK_ERROR_INITIALIZATION_FAILED; // Hardcoded failure
    }

    printf( "getPhysicalDeviceSurfaceFormatAndColorSpace(): Found %u surface formats\n", formatCount);

    // Allocate memory for surface format array
    VkSurfaceFormatKHR* vkSurfaceFormatKHR_Array = (VkSurfaceFormatKHR*)malloc(formatCount * sizeof(VkSurfaceFormatKHR));

    // Second call to fetch the actual supported surface formats
    vkresult = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice_selected, vkSurfaceKHR, &formatCount, vkSurfaceFormatKHR_Array);
    if (vkresult != VK_SUCCESS)
    {
        printf( "getPhysicalDeviceSurfaceFormatAndColorSpace(): Second call to vkGetPhysicalDeviceSurfaceFormatsKHR() failed\n");
        free(vkSurfaceFormatKHR_Array);
        return vkresult;
    }
    else
    {
        printf( "getPhysicalDeviceSurfaceFormatAndColorSpace() Second call: vkGetPhysicalDeviceSurfaceFormatsKHR succeeded\n");
    }

    // Decide the color format
    if (formatCount == 1 && vkSurfaceFormatKHR_Array[0].format == VK_FORMAT_UNDEFINED)
    {
        vkFormat_color = VK_FORMAT_B8G8R8A8_UNORM;
    }
    else
    {
        vkFormat_color = vkSurfaceFormatKHR_Array[0].format;
    }

    // Assign the color space
    vkColorSpaceKHR = vkSurfaceFormatKHR_Array[0].colorSpace;

    // Free allocated memory
    if (vkSurfaceFormatKHR_Array)
    {
        free(vkSurfaceFormatKHR_Array);
        vkSurfaceFormatKHR_Array = NULL;
        printf( "getPhysicalDeviceSurfaceFormatAndColorSpace(): vkSurfaceFormatKHR_Array Memory freed\n");
    }

    return vkresult;
}


-(VkResult) getPhysicalDevicePresentMode
{
    // Variable declaration
    VkResult vkresult = VK_SUCCESS;
    uint32_t presentModeCount = 0;

    // First call to get the number of supported present modes
    vkresult = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice_selected, vkSurfaceKHR, &presentModeCount, NULL);
    if (vkresult != VK_SUCCESS)
    {
        printf( "getPhysicalDevicePresentMode() 1st call: vkGetPhysicalDeviceSurfacePresentModesKHR() failed\n");
        return vkresult;
    }
    else
    {
        printf( "getPhysicalDevicePresentMode() 1st call : vkGetPhysicalDeviceSurfacePresentModesKHR() succeeded\n");
    }

    // Check if no present modes are found
    if (presentModeCount == 0)
    {
        printf( "getPhysicalDevicePresentMode(): No present modes found\n");
        return VK_ERROR_INITIALIZATION_FAILED; // Hardcoded failure
    }

    printf( "getPhysicalDevicePresentMode(): Found %u present modes\n", presentModeCount);

    // Allocate memory for present mode array
    VkPresentModeKHR* vkPresentModeKHR_Array = (VkPresentModeKHR*)malloc(presentModeCount * sizeof(VkPresentModeKHR));
    if (!vkPresentModeKHR_Array)  // Ensure memory allocation was successful
    {
        printf( "getPhysicalDevicePresentMode(): Memory allocation failed\n");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    // Second call to fetch the actual supported present modes
    vkresult = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice_selected, vkSurfaceKHR, &presentModeCount, vkPresentModeKHR_Array);
    if (vkresult != VK_SUCCESS)
    {
        printf( "getPhysicalDevicePresentMode() 2nd call: vkGetPhysicalDeviceSurfacePresentModesKHR() failed\n");
        free(vkPresentModeKHR_Array);
        return vkresult;
    }
    else
    {
        printf( "getPhysicalDevicePresentMode() 2nd call: vkGetPhysicalDeviceSurfacePresentModesKHR() succeeded\n");
    }

    // Decide the presentation mode
    vkPresentModeKHR = VK_PRESENT_MODE_FIFO_KHR; // Default mode (FIFO is always available)

    for (uint32_t i = 0; i < presentModeCount; i++)
    {
        if (vkPresentModeKHR_Array[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            vkPresentModeKHR = VK_PRESENT_MODE_MAILBOX_KHR;
            break; // Select MAILBOX if available
        }
    }

    // Print which mode was selected
    if (vkPresentModeKHR == VK_PRESENT_MODE_MAILBOX_KHR)
    {
        printf( "getPhysicalDevicePresentMode(): Selected Present Mode = MAILBOX_KHR\n");
    }
    else
    {
        printf( "getPhysicalDevicePresentMode(): Selected Present Mode = FIFO_KHR\n");
    }

    // Free allocated memory
    if (vkPresentModeKHR_Array)
    {
        free(vkPresentModeKHR_Array);
        vkPresentModeKHR_Array = NULL;
        printf( "getPhysicalDevicePresentMode(): Memory freed, function completed successfully\n");
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
        printf( "createSwapchain() : getPhysicalDeviceSurfaceFormatAndColorSpace() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "createSwapchain() : getPhysicalDeviceSurfaceFormatAndColorSpace() succeeded\n");
    }

    // step 2 : get physical device surface capabilities
    VkSurfaceCapabilitiesKHR vkSurfaceCapabilitiesKHR;
    memset((void*)&vkSurfaceCapabilitiesKHR, 0, sizeof(VkSurfaceCapabilitiesKHR));

    vkresult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice_selected, vkSurfaceKHR, &vkSurfaceCapabilitiesKHR);

    if (vkresult != VK_SUCCESS)
    {
        printf( "createSwapchain() : vkGetPhysicalDeviceSurfaceCapabilitiesKHR() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "createSwapchain() : vkGetPhysicalDeviceSurfaceCapabilitiesKHR() succeeded\n");
    }

    // step 3 : find out desired number of swapchain images
    uint32_t testingNoofSwapchainImages = vkSurfaceCapabilitiesKHR.minImageCount + 1;
    uint32_t desiredNoofSwapchainImages = 0;

    if (vkSurfaceCapabilitiesKHR.maxImageCount > 0 && vkSurfaceCapabilitiesKHR.maxImageCount < testingNoofSwapchainImages)
    {
        desiredNoofSwapchainImages = vkSurfaceCapabilitiesKHR.maxImageCount;
        printf( "\nmaxImageCount\n");
    }
    else
    {
        desiredNoofSwapchainImages = vkSurfaceCapabilitiesKHR.minImageCount;
        printf( "\nminImageCount\n");
    }

    printf( "\n\n\n%d %d %d !!!\n", vkSurfaceCapabilitiesKHR.maxImageCount, vkSurfaceCapabilitiesKHR.minImageCount, desiredNoofSwapchainImages);



    // step 4: choose size of the swapchain image
    memset((void*)&vkExtent2D_Swapchain, 0, sizeof(vkExtent2D_Swapchain));

    if (vkSurfaceCapabilitiesKHR.currentExtent.width != UINT32_MAX)
    {
        vkExtent2D_Swapchain.width = vkSurfaceCapabilitiesKHR.currentExtent.width;
        vkExtent2D_Swapchain.height = vkSurfaceCapabilitiesKHR.currentExtent.height;

        printf( "createSwapchain() : swapchain image width = %d height = %d\n", vkExtent2D_Swapchain.width, vkExtent2D_Swapchain.height);
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

        printf( "createSwapchain() : swapchain image width = %d height = %d\n", vkExtent2D_Swapchain.width, vkExtent2D_Swapchain.height);
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
        printf( "createSwapchain() : getPhysicalDevicePresentMode() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "createSwapchain() : getPhysicalDevicePresentMode() succeeded\n");
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
        printf( "createSwapchain() : vkCreateSwapchainKHR() function failed (%d)\n", vkresult);
        return(vkresult);
    }
    else
    {
        printf( "createSwapchain() : vkCreateSwapchainKHR() succeeded\n");
    }

    return vkresult;
}

-(VkResult)createImagesAndImageViews
{
    // variable declaration
    VkResult vkresult = VK_SUCCESS;

    // get swapchain image count
    vkresult = vkGetSwapchainImagesKHR(vkDevice, vkSwapchainKHR, &swapchainImageCount, NULL);
    if (vkresult != VK_SUCCESS)
    {
        printf( "createImagesAndImageViews() 1st call : vkGetSwapchainImagesKHR() function failed (%d)\n", vkresult);
        return vkresult;
    }
    else if (swapchainImageCount == 0)
    {
        printf( "createImagesAndImageViews() 1st call : swapchain image count is zero, returning hardcoded error value\n");
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    else
    {
        printf( "createImagesAndImageViews() 1st call : this func is giving the swapchain image count = %d\n", swapchainImageCount);
    }

    // allocate the swapchain image array
    SwapchainImage_Array = (VkImage*)malloc(sizeof(VkImage) * swapchainImageCount);

    // fill this array with swapchain images
    vkresult = vkGetSwapchainImagesKHR(vkDevice, vkSwapchainKHR, &swapchainImageCount, SwapchainImage_Array);
    if (vkresult != VK_SUCCESS)
    {
        printf( "createImagesAndImageViews() 2nd call : vkGetSwapchainImagesKHR() function failed (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createImagesAndImageViews() 2nd call : vkGetSwapchainImagesKHR() succeeded\n");
    }

    // allocate array of swapchain image views
    SwapchainImageView_Array = (VkImageView*)malloc(sizeof(VkImageView) * swapchainImageCount);

    // initialize VkImageViewCreateInfo Structure
    VkImageViewCreateInfo vkImageViewCreateInfo;
    memset((void*)&vkImageViewCreateInfo, 0, sizeof(VkImageViewCreateInfo));

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

    // now fill image view array using above struct
    for (uint32_t i = 0; i < swapchainImageCount; i++)
    {
        vkImageViewCreateInfo.image = SwapchainImage_Array[i];

        vkresult = vkCreateImageView(vkDevice,
                                     &vkImageViewCreateInfo,
                                     NULL,
                                     &SwapchainImageView_Array[i]);

        if (vkresult != VK_SUCCESS)
        {
            printf(
                    "createImagesAndImageViews() : vkCreateImageView() function failed for iteration (%d).(%d)\n",
                    i,
                    vkresult);
            return vkresult;
        }
        else
        {
            printf(
                    "createImagesAndImageViews() : vkCreateImageView() succeeded for iteration (%d)\n",
                    i);
        }
    }

    return vkresult;
}

-(VkResult) createCommandPool
{
    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    // vkCommandPool creating info structure
    VkCommandPoolCreateInfo vkCommandPoolCreateInfo;
    memset(&vkCommandPoolCreateInfo, 0, sizeof(VkCommandPoolCreateInfo));

    vkCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    vkCommandPoolCreateInfo.pNext = NULL;
    vkCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    vkCommandPoolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex_Selected;

    // Create the command pool
    vkresult = vkCreateCommandPool(vkDevice, &vkCommandPoolCreateInfo, NULL, &vkcommandpool);

    if (vkresult != VK_SUCCESS)
    {
        printf( "createCommandPool() : vkCreateCommandPool() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createCommandPool() : vkCreateCommandPool() succeeded.\n");
    }

    return vkresult;
}

-(VkResult) createCommandBuffers
{
    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    // Command buffer allocation structure initialization
    VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo;
    memset(&vkCommandBufferAllocateInfo, 0, sizeof(VkCommandBufferAllocateInfo));

    vkCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    vkCommandBufferAllocateInfo.pNext = NULL;
    vkCommandBufferAllocateInfo.commandPool = vkcommandpool;
    vkCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    vkCommandBufferAllocateInfo.commandBufferCount = swapchainImageCount;

    // Allocate memory for command buffer array
    vkCommandBuffer_Array = (VkCommandBuffer*)malloc(sizeof(VkCommandBuffer) * swapchainImageCount);

    // Allocate command buffers
    vkresult = vkAllocateCommandBuffers(vkDevice, &vkCommandBufferAllocateInfo, vkCommandBuffer_Array);

    if (vkresult != VK_SUCCESS)
    {
        printf( "createCommandBuffers() : vkAllocateCommandBuffers() function failed. Error Code: (%d)\n", vkresult);
        free(vkCommandBuffer_Array);
        return vkresult;
    }
    else
    {
        printf( "createCommandBuffers() : vkAllocateCommandBuffers() succeeded.\n");
    }

    return vkresult;
}

-(VkResult)createVertexBuffer
{

    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    float traingle_Position[] =
    {
        0.0f,1.0f,0.0f,
        -1.0f,-1.0f,0.0f,
        1.0f,-1.0f,0.0f
    };

    float triangle_color[] =
    {
        1.0f,0.0f,0.0f,
        0.0f,1.0f,0.0f,
        0.0f,0.0f,1.0f
    };

    for (int i = 0; i < 9; ++i)
    {
        printf("Color[%d] = %f\n", i, triangle_color[i]);
    }

    // VERTEX POSITION BUFFER
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
        printf( "createVertexBuffer() : vkCreateBuffer() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createVertexBuffer() : vkCreateBuffer() succeeded.\n");
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
        printf( "createVertexBuffer() : Failed to find suitable memory type for vertex buffer\n");
        return vkresult; // Return hardcoded error code
    }
    else
    {
        printf( "createVertexBuffer() : Suitable memory type found for vertex buffer at index %d\n", vkMemoryAllocateInfo.memoryTypeIndex);
    }

    vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, NULL, &vertexData_Position.vkDeviceMemory);
    if (vkresult != VK_SUCCESS)
    {
        printf( "createVertexBuffer() : vkAllocateMemory() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createVertexBuffer() : vkAllocateMemory() succeeded.\n");
    }

    vkresult = vkBindBufferMemory(vkDevice, vertexData_Position.vkBuffer, vertexData_Position.vkDeviceMemory, 0);
    if (vkresult != VK_SUCCESS)
    {
        printf( "createVertexBuffer() : vkBindBufferMemory() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createVertexBuffer() : vkBindBufferMemory() succeeded.\n");
    }

    void* data = NULL;

    vkresult = vkMapMemory(vkDevice, vertexData_Position.vkDeviceMemory, 0, vkMemoryAllocateInfo.allocationSize, 0, &data);
    if (vkresult != VK_SUCCESS)
    {
        printf( "createVertexBuffer() : vkMapMemory() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createVertexBuffer() : vkMapMemory() succeeded.\n");
    }

    // actual memory mapped

    memcpy(data, traingle_Position, sizeof(traingle_Position));

    vkUnmapMemory(vkDevice, vertexData_Position.vkDeviceMemory);

    // VERTEX COLOR BUFFER
    memset((void*)&vertexData_Color, 0, sizeof(VertexData));

    memset((void*)&vkBufferCreateInfo, 0, sizeof(VkBufferCreateInfo));

    vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vkBufferCreateInfo.pNext = NULL;
    vkBufferCreateInfo.flags = 0;
    vkBufferCreateInfo.size = sizeof(triangle_color);
    vkBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    vkresult = vkCreateBuffer(vkDevice, &vkBufferCreateInfo, NULL, &vertexData_Color.vkBuffer);
    if (vkresult != VK_SUCCESS)
    {
        printf( "createVertexBuffer() : vkCreateBuffer() function failed for vertex color buffer Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createVertexBuffer() : vkCreateBuffer() succeeded for vertex color buffer\n");
    }

    memset((void*)&vkMemoryRequirements, 0, sizeof(VkMemoryRequirements));

    vkGetBufferMemoryRequirements(vkDevice, vertexData_Color.vkBuffer, &vkMemoryRequirements);

    memset((void*)&vkMemoryAllocateInfo, 0, sizeof(VkMemoryAllocateInfo));

    vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    vkMemoryAllocateInfo.pNext = NULL;
    vkMemoryAllocateInfo.allocationSize = vkMemoryRequirements.size;
    vkMemoryAllocateInfo.memoryTypeIndex = 0; // initial value before entering into loop

    for (uint32_t i = 0; i < vkPhysicalDeviceMemoryProperties.memoryTypeCount; i++)
    {
        if ((vkMemoryRequirements.memoryTypeBits & 1) == 1)
        {
            if (vkPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
            {
                vkMemoryAllocateInfo.memoryTypeIndex = i;
                break;
            }
        }

        vkMemoryRequirements.memoryTypeBits >>= 1;

    }

    vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, NULL, &vertexData_Color.vkDeviceMemory);
    if (vkresult != VK_SUCCESS)
    {
        printf( "createVertexBuffer() : vkAllocateMemory() function failed failed for vertex color buffer Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createVertexBuffer() : vkAllocateMemory() succeeded for vertex color buffer\n");
    }

    vkresult = vkBindBufferMemory(vkDevice, vertexData_Color.vkBuffer, vertexData_Color.vkDeviceMemory, 0);
    if (vkresult != VK_SUCCESS)
    {
        printf( "createVertexBuffer() : vkBindBufferMemory() function failed failed for vertex color buffer Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createVertexBuffer() : vkBindBufferMemory() succeeded  for vertex color buffer\n");
    }

    data = NULL;

    vkresult = vkMapMemory(vkDevice, vertexData_Color.vkDeviceMemory, 0, vkMemoryAllocateInfo.allocationSize, 0, &data);
    if (vkresult != VK_SUCCESS)
    {
        printf( "createVertexBuffer() : vkMapMemory() function failed for vertex color buffer Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createVertexBuffer() : vkMapMemory() succeeded for vertex color buffer\n");
    }

    // actual memory mapped

    memcpy(data, triangle_color, sizeof(triangle_color));

    vkUnmapMemory(vkDevice, vertexData_Color.vkDeviceMemory);


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
        printf( "createUniformBuffer() : vkCreateBuffer() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createUniformBuffer() : vkCreateBuffer() succeeded.\n");
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
        printf( "createUniformBuffer() : Failed to find suitable memory type for uniform buffer\n");
        return vkresult; // Return hardcoded error code
    }
    else
    {
        printf( "createUniformBuffer() : Suitable memory type found for uniform buffer at index %d\n", vkMemoryAllocateInfo.memoryTypeIndex);
    }

    vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, NULL, &uniformData.vkDeviceMemory);
    if (vkresult != VK_SUCCESS)
    {
        printf( "createUniformBuffer() : vkAllocateMemory() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createUniformBuffer() : vkAllocateMemory() succeeded.\n");
    }

    vkresult = vkBindBufferMemory(vkDevice, uniformData.vkBuffer, uniformData.vkDeviceMemory, 0);
    if (vkresult != VK_SUCCESS)
    {
        printf( "createUniformBuffer() : vkBindBufferMemory() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createUniformBuffer() : vkBindBufferMemory() succeeded.\n");
    }

    // call updateUnifomBuffer
    vkresult = [self updateUniformbuffer];
    if (vkresult != VK_SUCCESS)
    {
        printf( "createUniformBuffer() : updateUniformbuffer() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createUniformBuffer() : updateUniformbuffer() succeeded.\n");
    }


    return vkresult;
}


-(VkResult)updateUniformbuffer
{
    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    // code
    MyUniformData myUniformData;
    memset((void*)&myUniformData, 0, sizeof(MyUniformData));

    // update matrices
    myUniformData.modelMatrix = glm::mat4(1.0);
    myUniformData.modelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, -3.0f));
    myUniformData.viewMatrix = glm::mat4(1.0);
    glm::mat4 perspectiveProjectionMatrix = glm::mat4(1.0);

    perspectiveProjectionMatrix = glm::perspective(glm::radians(45.0f), float(winWidth) / float(winHeight), 0.1f, 100.0f);

    perspectiveProjectionMatrix[1][1] = perspectiveProjectionMatrix[1][1] * (-1.0f);

    myUniformData.projectionMatrix = perspectiveProjectionMatrix;

    // map unifrom buffer

    void* data = NULL;

    vkresult = vkMapMemory(vkDevice, uniformData.vkDeviceMemory, 0, sizeof(MyUniformData), 0, &data);
    if (vkresult != VK_SUCCESS)
    {
        printf( "updateUniformbuffer() : vkMapMemory() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }

    // actual memory mapped
    memcpy(data, &myUniformData, sizeof(myUniformData));

    vkUnmapMemory(vkDevice, uniformData.vkDeviceMemory);

    return vkresult;
}


-(VkResult) createShaders
{
    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    //code

    // for vertex shaders
    NSBundle* appBundle = [NSBundle mainBundle];

    const char* szfileName = "Shader.vert.spv";

    NSString *shaderfileNameWithPath =
        [[appBundle resourcePath] stringByAppendingPathComponent:@(szfileName)];

    const char* pszshaderFileNameWithPath = [shaderfileNameWithPath cStringUsingEncoding:NSUTF8StringEncoding];
    
    FILE* fp = NULL;

    size_t size;

    fp = fopen(pszshaderFileNameWithPath, "rb");

    if (fp == NULL)
    {
        printf( "createShaders() : createShader failed to open vertexshader.spv file\n");
        vkresult = VK_ERROR_INITIALIZATION_FAILED;
        return vkresult;
    }
    else
    {
        printf( "createShaders() : createShader succeeded to open vertexshader.spv file\n");
    }

    fseek(fp, 0L, SEEK_END);

    size = ftell(fp);

    if (size == 0)
    {
        printf( "createShaders() : createShader failed and give file size of vertex shader 0\n");
        vkresult = VK_ERROR_INITIALIZATION_FAILED;
        return vkresult;
    }

    fseek(fp, 0L, SEEK_SET);

    char* shaderData = (char*)malloc(sizeof(char) * size);

    size_t retVal = fread(shaderData, size, 1, fp);

    if (retVal != 1)
    {
        printf( "createShaders() : createShader failed to read vertexshader.spv file\n");
        vkresult = VK_ERROR_INITIALIZATION_FAILED;
        return vkresult;
    }
    else
    {
        printf( "createShaders() : createShader succeeded to read vertexshader.spv file\n");
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
        printf( "createShaders() : vkCreateShaderModule() function failed.for vertex shader Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createShaders() : vkCreateShaderModule() succeeded fro vertex shader.\n");
    }


    if (shaderData)
    {
        free(shaderData);
        shaderData = NULL;
    }

    printf( "Vertexshader Module sucessfully created\n");

    // for fragmnt shader

    // for fragment shader

    szfileName = "Shader.frag.spv";

    size = 0;

    shaderfileNameWithPath =
        [[appBundle resourcePath] stringByAppendingPathComponent:@(szfileName)];

    pszshaderFileNameWithPath = [shaderfileNameWithPath cStringUsingEncoding:NSUTF8StringEncoding];

    fp = fopen(pszshaderFileNameWithPath, "rb");

    if (fp == NULL)
    {
        printf( "createShaders() : createShader failed to open fragmentshader.spv file\n");
        vkresult = VK_ERROR_INITIALIZATION_FAILED;
        return vkresult;
    }
    else
    {
        printf( "createShaders() : createShader succeeded to open fragmentshader.spv file\n");
    }

    fseek(fp, 0L, SEEK_END);

    size = ftell(fp);

    if (size == 0)
    {
        printf( "createShaders() : createShader failed and give file size of fragment shader 0\n");
        vkresult = VK_ERROR_INITIALIZATION_FAILED;
        return vkresult;
    }

    fseek(fp, 0L, SEEK_SET);

    shaderData = (char*)malloc(sizeof(char) * size);

    retVal = fread(shaderData, size, 1, fp);

    if (retVal != 1)
    {
        printf( "createShaders() : createShader failed to read fragmentshader.spv file\n");
        vkresult = VK_ERROR_INITIALIZATION_FAILED;
        return vkresult;
    }
    else
    {
        printf( "createShaders() : createShader succeeded to read fragmentshader.spv file\n");
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
        printf(
                "createShaders() : vkCreateShaderModule() function failed for fragment shader. Error Code: (%d)\n",
                vkresult);
        return vkresult;
    }
    else
    {
        printf(
                "createShaders() : vkCreateShaderModule() succeeded for fragment shader.\n");
    }

    if (shaderData)
    {
        free(shaderData);
        shaderData = NULL;
    }

    printf( "Fragment Shader Module successfully created\n");

    return vkresult;
}


-(VkResult)createDiscriptorSetLayout
{
    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    // initialise descriptorsetBinding
    VkDescriptorSetLayoutBinding vkdescriptorSetLayoutBinding;
    memset((void*)&vkdescriptorSetLayoutBinding, 0, sizeof(VkDescriptorSetLayoutBinding));

    vkdescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    vkdescriptorSetLayoutBinding.binding = 0;  // this 0 is related with the binding  = 0 of vertex shader
    vkdescriptorSetLayoutBinding.descriptorCount = 1;
    vkdescriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    vkdescriptorSetLayoutBinding.pImmutableSamplers = NULL;


    VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo;
    memset((void*)&vkDescriptorSetLayoutCreateInfo, 0, sizeof(VkDescriptorSetLayoutCreateInfo));

;    vkDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    vkDescriptorSetLayoutCreateInfo.pNext = NULL;
    vkDescriptorSetLayoutCreateInfo.flags = 0;
    vkDescriptorSetLayoutCreateInfo.bindingCount = 1;
    vkDescriptorSetLayoutCreateInfo.pBindings =&vkdescriptorSetLayoutBinding;  // pbinding array is actually array VkDiscriptorSetLayoutBinding having 5 members  1) uint32_t binding : an integer value where you want to bind descriptor set
                                                                                                                                        // 2) VkDiscriptorSetType DiscriptorType : which type of descriptor
                                                                                                                                        // 3) uint32_t discriptorCount : How many descriptor
                                                                                                                                        // 4) VkShaderStageFalgs stageFalgs : konty shader mdhe vaprycha aahe?
                                                                                                                                        // 5) const VkSamplaar* pImmutableSamplers : jevha aaplyakde sampler aahe permanant

    vkresult = vkCreateDescriptorSetLayout(vkDevice, &vkDescriptorSetLayoutCreateInfo, NULL, &vkDescriptorSetLayout);
    if (vkresult != VK_SUCCESS)
    {
        printf( "createDescriptorSetLayout() : vkCreateDescriptorSetLayout() failed Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createDescriptorSetLayout() : vkCreateDescriptorSetLayout() succeeded.\n");
    }



    return vkresult;

}

-(VkResult)createPiplineLayout
{
    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    VkPipelineLayoutCreateInfo vkPipelineLayoutCreateInfo;
    memset((void*)&vkPipelineLayoutCreateInfo, 0, sizeof(VkPipelineLayoutCreateInfo));

    vkPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    vkPipelineLayoutCreateInfo.pNext = NULL;
    vkPipelineLayoutCreateInfo.flags = 0;

    // Set descriptor set layouts
    vkPipelineLayoutCreateInfo.setLayoutCount = 1;
    vkPipelineLayoutCreateInfo.pSetLayouts = &vkDescriptorSetLayout;
    vkPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    vkPipelineLayoutCreateInfo.pPushConstantRanges = NULL;

    // Create the pipeline layout
    vkresult = vkCreatePipelineLayout(vkDevice, &vkPipelineLayoutCreateInfo, NULL, &vkPipelineLayout);
    if (vkresult != VK_SUCCESS)
    {
        printf( "createPiplineLayout() : vkCreatePipelineLayout() failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createPiplineLayout() : vkCreatePipelineLayout() succeeded.\n");
    }

    return vkresult;
}

-(VkResult)createDescriptorpool
{
    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    VkDescriptorPoolSize vkdescriptorPoolSize;
    memset((void*)&vkdescriptorPoolSize, 0, sizeof(VkDescriptorPoolSize));

    vkdescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    vkdescriptorPoolSize.descriptorCount = 1;

    // create the pool
    VkDescriptorPoolCreateInfo vkdescriptorPoolCreateInfo;
    memset((void*)&vkdescriptorPoolCreateInfo, 0, sizeof(VkDescriptorPoolCreateInfo));

    vkdescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    vkdescriptorPoolCreateInfo.pNext = NULL;
    vkdescriptorPoolCreateInfo.flags = 0;
    vkdescriptorPoolCreateInfo.poolSizeCount = 1;
    vkdescriptorPoolCreateInfo.pPoolSizes = &vkdescriptorPoolSize;
    vkdescriptorPoolCreateInfo.maxSets = 1;

    vkresult = vkCreateDescriptorPool(vkDevice, &vkdescriptorPoolCreateInfo, NULL, &vkDescriptorPool);
    if (vkresult != VK_SUCCESS)
    {
        printf( "createDescriptorpool() : vkCreateDescriptorPool() failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createDescriptorpool() : vkCreateDescriptorPool() succeeded.\n");
    }


    return vkresult;
}

-(VkResult)createDescriptorSet
{
    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    // code

    // initialise descriptor set alloc info

    VkDescriptorSetAllocateInfo vkDescriptorSetAllocateInfo;
    memset((void*)&vkDescriptorSetAllocateInfo, 0, sizeof(VkDescriptorSetAllocateInfo));

    vkDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    vkDescriptorSetAllocateInfo.pNext = NULL;
    vkDescriptorSetAllocateInfo.descriptorPool = vkDescriptorPool;
    vkDescriptorSetAllocateInfo.descriptorSetCount = 1;
    vkDescriptorSetAllocateInfo.pSetLayouts = &vkDescriptorSetLayout;

    vkresult = vkAllocateDescriptorSets(vkDevice, &vkDescriptorSetAllocateInfo, &vkDescriptorSet);
    if (vkresult != VK_SUCCESS)
    {
        printf( "createDescriptorSet() : vkCreateDescriptorPool() failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createDescriptorSet() : vkCreateDescriptorPool() succeeded.\n");
    }
    
    // describe whether we want image as uniform or buffer as unuform
    VkDescriptorBufferInfo vkdescriptorBufferInfo;
    memset((void*)&vkdescriptorBufferInfo, 0, sizeof(VkDescriptorBufferInfo));

    vkdescriptorBufferInfo.buffer = uniformData.vkBuffer;
    vkdescriptorBufferInfo.offset = 0;
    vkdescriptorBufferInfo.range = sizeof(MyUniformData);

    // now upadte descriptor set directly to the shader


    VkWriteDescriptorSet vkWriteDescriptorSet;
    memset((void*)&vkWriteDescriptorSet, 0, sizeof(VkWriteDescriptorSet));

    vkWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    vkWriteDescriptorSet.dstSet = vkDescriptorSet;
    vkWriteDescriptorSet.dstBinding = 0; // Matches layout(binding = 0) in shader
    vkWriteDescriptorSet.dstArrayElement = 0;
    vkWriteDescriptorSet.descriptorCount = 1;
    vkWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    vkWriteDescriptorSet.pBufferInfo = &vkdescriptorBufferInfo;
    vkWriteDescriptorSet.pImageInfo = NULL;
    vkWriteDescriptorSet.pTexelBufferView = NULL;

    vkUpdateDescriptorSets(vkDevice, 1, &vkWriteDescriptorSet, 0, NULL);

    printf( "\nvkUpdateDescriptorSets() succeeded.\n");


    return vkresult;
}

-(VkResult)createRenderPass
{
    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    VkAttachmentDescription vkAttachmentDescription_array[1];

    memset((void*)vkAttachmentDescription_array, 0, sizeof(VkAttachmentDescription) * _ARRAYSIZE(vkAttachmentDescription_array));
    vkAttachmentDescription_array[0].flags = 0;
    vkAttachmentDescription_array[0].format = vkFormat_color;
    vkAttachmentDescription_array[0].samples = VK_SAMPLE_COUNT_1_BIT;   //No multi sampling so 1 bit is enough
    vkAttachmentDescription_array[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    vkAttachmentDescription_array[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;                //color attachment related
    vkAttachmentDescription_array[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;            //This is for both Depth and Stencil although it is for stencil
    vkAttachmentDescription_array[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    vkAttachmentDescription_array[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;                                                    //Image data when in and when out
    vkAttachmentDescription_array[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;




    //Declare and initialize vkAttachmentReference structure
    VkAttachmentReference vkAttachmentRederence;
    memset((void*)&vkAttachmentRederence, 0, sizeof(VkAttachmentReference));
    vkAttachmentRederence.attachment = 0;            //This means above given array 0th Ataachment reference, O means it is the index number
    vkAttachmentRederence.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;        //Tis means this attachment i can use it color attachment so keep it optimal


    //Step 3 : Declare and Initialize vkSubpassDescription

    VkSubpassDescription vkSubpassDesciption;
    memset((void*)&vkSubpassDesciption, 0, sizeof(VkSubpassDescription));
    vkSubpassDesciption.flags = 0;
    vkSubpassDesciption.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    vkSubpassDesciption.inputAttachmentCount = 0;
    vkSubpassDesciption.pInputAttachments = NULL;
    vkSubpassDesciption.colorAttachmentCount = _ARRAYSIZE(vkAttachmentDescription_array);
    vkSubpassDesciption.pColorAttachments = &vkAttachmentRederence;
    vkSubpassDesciption.pResolveAttachments = NULL;
    vkSubpassDesciption.pDepthStencilAttachment = NULL;
    vkSubpassDesciption.preserveAttachmentCount = 0;
    vkSubpassDesciption.pPreserveAttachments = NULL;

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
    vkRenderPassCreateInfo.pDependencies = NULL;

    // Create render pass
    vkresult = vkCreateRenderPass(vkDevice, &vkRenderPassCreateInfo, NULL, &vkRenderpass);
    if (vkresult != VK_SUCCESS)
    {
        printf( "createRenderPass() : vkCreateRenderPass() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createRenderPass() : vkCreateRenderPass() succeeded.\n");
    }

    return vkresult;
}

-(VkResult)createPipline
{
    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    // vertex input state
    VkVertexInputBindingDescription vkVertexInputBindingDescription_Array[2];
    memset((void*)vkVertexInputBindingDescription_Array, 0, sizeof(VkVertexInputBindingDescription) * _ARRAYSIZE(vkVertexInputBindingDescription_Array));

    // for position
    vkVertexInputBindingDescription_Array[0].binding = 0; // corresponding to location = 0 in vertex shader
    vkVertexInputBindingDescription_Array[0].stride = sizeof(float) * 3;
    vkVertexInputBindingDescription_Array[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    // for color
    vkVertexInputBindingDescription_Array[1].binding = 1; // corresponding to location  = 1 in vertex shader
    vkVertexInputBindingDescription_Array[1].stride = sizeof(float) * 3;
    vkVertexInputBindingDescription_Array[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription vkVertexInputAttributeDescription_Array[2];
    memset((void*)vkVertexInputAttributeDescription_Array, 0, sizeof(VkVertexInputAttributeDescription) * _ARRAYSIZE(vkVertexInputAttributeDescription_Array));

    // for position
    vkVertexInputAttributeDescription_Array[0].binding = 0;
    vkVertexInputAttributeDescription_Array[0].location = 0;
    vkVertexInputAttributeDescription_Array[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vkVertexInputAttributeDescription_Array[0].offset = 0;

    // for color
    vkVertexInputAttributeDescription_Array[1].binding = 1;
    vkVertexInputAttributeDescription_Array[1].location = 1;
    vkVertexInputAttributeDescription_Array[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vkVertexInputAttributeDescription_Array[1].offset = 0;

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
    vkPipelineColorBlendAttachmentState_Array[0].colorWriteMask =VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;



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

    // as we dont have depth yet we can skip this



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
        printf( "createPipline() : vkCreatePipelineCache() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createPipline() : vkCreatePipelineCache() succeeded.\n");
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
    vkGraphicsPipelineCreateInfo.pDepthStencilState = NULL;
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
        printf( "createPipeline() : vkCreateGraphicsPipelines() function failed. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createPipeline() : vkCreateGraphicsPipelines() succeeded.\n");
    }

    // we have done with pipline cache so destroy it

    if (vkPipelineCache != VK_NULL_HANDLE)
    {
        vkDestroyPipelineCache(vkDevice, vkPipelineCache, NULL);
        vkPipelineCache = VK_NULL_HANDLE;
        printf( "createPipeline() : Free vkPipelineCache freed\n");
    }

    return vkresult;

}


-(VkResult)createframeBuffers
{
    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    // Declare array of VkImageView
    VkImageView vkImageView_Attchment_Array[1];
    memset((void*)vkImageView_Attchment_Array, 0, sizeof(VkImageView) * _ARRAYSIZE(vkImageView_Attchment_Array));

    VkFramebufferCreateInfo vkFramebufferCreateInfo;
    memset((void*)&vkFramebufferCreateInfo, 0, sizeof(VkFramebufferCreateInfo));

    vkFramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    vkFramebufferCreateInfo.pNext = NULL;
    vkFramebufferCreateInfo.flags = 0;
    vkFramebufferCreateInfo.renderPass = vkRenderpass;
    vkFramebufferCreateInfo.attachmentCount = _ARRAYSIZE(vkImageView_Attchment_Array);
    vkFramebufferCreateInfo.pAttachments = vkImageView_Attchment_Array;
    vkFramebufferCreateInfo.width = vkExtent2D_Swapchain.width;
    vkFramebufferCreateInfo.height = vkExtent2D_Swapchain.height;
    vkFramebufferCreateInfo.layers = 1;

    // Allocate framebuffer array
    vkFramebuffer_Array = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * swapchainImageCount);
    

    for (uint32_t i = 0; i < swapchainImageCount; i++)
    {
        vkImageView_Attchment_Array[0] = SwapchainImageView_Array[i];

        vkresult = vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &vkFramebuffer_Array[i]);
        if (vkresult != VK_SUCCESS)
        {
            printf( "createframeBuffers() : vkCreateFramebuffer() function failed. Error Code: (%d)\n", vkresult);
            return vkresult;
        }
        else
        {
            printf( "createframeBuffers() : vkCreateFramebuffer() succeeded.\n");
        }
    }

    return vkresult;
}

-(VkResult)createSemaphores
{
    // code

    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    VkSemaphoreCreateInfo vkSemaphoreCreateInfo;
    memset((void*)&vkSemaphoreCreateInfo, 0, sizeof(VkSemaphoreCreateInfo));

    vkSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    vkSemaphoreCreateInfo.pNext = NULL;
    vkSemaphoreCreateInfo.flags = 0; // must be zero

    vkresult = vkCreateSemaphore(vkDevice, &vkSemaphoreCreateInfo, NULL, &vkSemaphore_backbuffer);
    if (vkresult != VK_SUCCESS)
    {
        printf( "createSemaphores() : vkCreateSemaphore() function failed for backbuffer. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createSemaphores() : vkCreateSemaphore() succeeded for backbuffer.\n");
    }

    vkresult = vkCreateSemaphore(vkDevice, &vkSemaphoreCreateInfo, NULL, &vkSemaphore_rendercomplete);
    if (vkresult != VK_SUCCESS)
    {
        printf( "createSemaphores() : vkCreateSemaphore() function failed for rendercomplete. Error Code: (%d)\n", vkresult);
        return vkresult;
    }
    else
    {
        printf( "createSemaphores() : vkCreateSemaphore() succeeded for rendercomplete.\n");
    }

    return vkresult;
}



-(VkResult)createFences
{
    // code

    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    VkFenceCreateInfo vkFenceCreateInfo;
    memset(&vkFenceCreateInfo, 0, sizeof(VkFenceCreateInfo));

    vkFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    vkFenceCreateInfo.pNext = NULL;
    vkFenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    vkFence_Array = (VkFence*)malloc(sizeof(VkFence) * swapchainImageCount);

    for (uint32_t i = 0; i < swapchainImageCount; i++)
    {
        vkresult = vkCreateFence(vkDevice, &vkFenceCreateInfo, NULL, &vkFence_Array[i]);
        if (vkresult != VK_SUCCESS)
        {
            printf( "createFences() : vkCreateFence() function failed. Error Code: (%d)\n", vkresult);
            return vkresult;
        }
        else
        {
            printf( "createFences() : vkCreateFence() succeeded.\n");
        }
    }

    return vkresult;
}

-(VkResult)buildCommandBuffers
{
    // Variable declaration
    VkResult vkresult = VK_SUCCESS;

    // loop per swapchain image
    for (uint32_t i = 0; i < swapchainImageCount; i++)
    {
        // reset command buffers
        vkresult = vkResetCommandBuffer(vkCommandBuffer_Array[i], 0);
        if (vkresult != VK_SUCCESS)
        {
            printf( "buildCommandBuffers() : vkResetCommandBuffer() failed at index [%d]. Error Code: (%d)\n", i, vkresult);
            return vkresult;
        }
        else
        {
            printf( "buildCommandBuffers() : vkResetCommandBuffer() succeeded at index [%d].\n", i);
        }

        VkCommandBufferBeginInfo vkCommandBufferBeginInfo;
        memset((void*)&vkCommandBufferBeginInfo, 0, sizeof(VkCommandBufferBeginInfo));

        vkCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkCommandBufferBeginInfo.pNext = NULL;
        vkCommandBufferBeginInfo.flags = 0;

        vkresult = vkBeginCommandBuffer(vkCommandBuffer_Array[i], &vkCommandBufferBeginInfo);
        if (vkresult != VK_SUCCESS)
        {
            printf( "buildCommandBuffers() : VkBeginCommandBuffer() failed at index [%d]. Error Code: (%d)\n", i, vkresult);
            return vkresult;
        }
        else
        {
            printf( "buildCommandBuffers() : VkBeginCommandBuffer() succeeded at index [%d].\n", i);
        }

        // set clear values
        VkClearValue vkClearValue_Array[1];
        memset((void*)vkClearValue_Array, 0, sizeof(VkClearValue) * _ARRAYSIZE(vkClearValue_Array));

        vkClearValue_Array[0].color = vkClearColorValue;


        VkRenderPassBeginInfo vkRenderPassBeginInfo;
        memset((void*)&vkRenderPassBeginInfo, 0, sizeof(VkRenderPassBeginInfo));

        vkRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        vkRenderPassBeginInfo.pNext = NULL;
        vkRenderPassBeginInfo.renderPass = vkRenderpass;
        vkRenderPassBeginInfo.renderArea.offset.x = 0;
        vkRenderPassBeginInfo.renderArea.offset.y = 0;
        vkRenderPassBeginInfo.renderArea.extent.width = vkExtent2D_Swapchain.width;
        vkRenderPassBeginInfo.renderArea.extent.height = vkExtent2D_Swapchain.height;
        vkRenderPassBeginInfo.clearValueCount = _ARRAYSIZE(vkClearValue_Array);
        vkRenderPassBeginInfo.pClearValues = vkClearValue_Array;
        vkRenderPassBeginInfo.framebuffer = vkFramebuffer_Array[i];

        // Begin the render pass
        vkCmdBeginRenderPass(vkCommandBuffer_Array[i], &vkRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        // bind with the pipline
        vkCmdBindPipeline(vkCommandBuffer_Array[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);

        // bind descriptor set to pipline
        vkCmdBindDescriptorSets(
            vkCommandBuffer_Array[i],
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            vkPipelineLayout, // must match the one used to create pipeline
            0,              // firstSet
            1,              // descriptorSetCount
            &vkDescriptorSet,
            0,              // dynamicOffsetCount
            NULL            // pDynamicOffsets
        );


        // bind vertex position vertex buffer
        VkDeviceSize vkDeviceSize_Offest_Position[1];
        memset((void*)vkDeviceSize_Offest_Position, 0, sizeof(VkDeviceSize) * _ARRAYSIZE(vkDeviceSize_Offest_Position));

        vkCmdBindVertexBuffers(vkCommandBuffer_Array[i], 0, 1, &vertexData_Position.vkBuffer, vkDeviceSize_Offest_Position);

        // bind vertex color vertex buffer
        VkDeviceSize vkDeviceSize_Offest_Color[1];
        memset((void*)vkDeviceSize_Offest_Color, 0, sizeof(VkDeviceSize) * _ARRAYSIZE(vkDeviceSize_Offest_Color));

        vkCmdBindVertexBuffers(vkCommandBuffer_Array[i], 1, 1, &vertexData_Color.vkBuffer, vkDeviceSize_Offest_Color);

        // Here we should call Vulkan drawing functions
        vkCmdDraw(vkCommandBuffer_Array[i], 3, 1, 0, 0);

        // End the render pass
        vkCmdEndRenderPass(vkCommandBuffer_Array[i]);

        // End command buffer recording
        vkresult = vkEndCommandBuffer(vkCommandBuffer_Array[i]);
        if (vkresult != VK_SUCCESS)
        {
            printf( "buildCommandBuffers() : vkEndCommandBuffer() failed at index [%d]. Error Code: (%d)\n", i, vkresult);
            return vkresult;
        }
        else
        {
            printf( "buildCommandBuffers() : vkEndCommandBuffer() succeeded at index [%d].\n", i);
        }

    }

    return vkresult;

}

VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallBack(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    // Suppress unused parameter warnings
    (void)messageSeverity;
    (void)messageType;
    (void)pUserData;

    printf("PRS_Validation : vkDebugUtilsMessengerCallback() : %s\n",
           pCallbackData->pMessage);

    // Return VK_FALSE to let Vulkan continue execution.
    return VK_FALSE;
}

@end

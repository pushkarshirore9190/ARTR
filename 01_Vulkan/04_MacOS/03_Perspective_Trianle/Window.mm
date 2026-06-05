#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#import <QuartzCore/CVDisplayLink.h> // this is for coreVideo display link
#import<QuartzCore/CAMetalLayer.h> // Metal based core animation layer

// vulkan related moltenvk header files
#include<MoltenVK/mvk_vulkan.h>

// Vulkan related header files
#include <vulkan/vulkan.h>  // VK_USE_PLATFORM_MACOS_MVK dont redefine this macro as it is already defined in mvk_vulkan.h

// glm related macros and header files
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TWO_ONE
#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"

// macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define _ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

// global function declarations
CVReturn displayLinkCallback(CVDisplayLinkRef, const CVTimeStamp*, const CVTimeStamp* , CVOptionFlags , CVOptionFlags* , void*);


// global variable declarations
int winWidth = WIN_WIDTH;
int winHeight = WIN_HEIGHT;

BOOL bActiveWindow = NO;
BOOL bFullScreen = NO;
BOOL bWindowMinimized = NO;

char gszLogFileName[] = "Log.txt";
FILE *gpFile = NULL;

NSView *gpView = nil;

// vulkun related global variables
uint32_t enabledInstanceExtensionCount = 0;

const char* enabledInstanceExtensionNames_array[4];  // VK_KHR_SURFACE_EXTENSION_NAME and VK_KHR_MACOS_SURFACE_EXTENSION_NAME and VK_EXT_DEBUG_REPORT_EXTENSION_NAME and VK_PORTABILITY_ENUMERATION_EXTENSION_NAME

// instance extension related variables
VkInstance vkInstance = VK_NULL_HANDLE;


uint32_t enableDeviceExtensionCount = 0;

const char* enabledDeviceExtensionNames_array[2];  // VK_KHR_SWAPCHAIN_EXTENSION_NAME and VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME


// Vulakn Presentation Surface
VkSurfaceKHR  vkSurfaceKHR = VK_NULL_HANDLE;

// Vulkan Physical Device Related Global Variables
VkPhysicalDevice vkPhysicalDevice_selected = VK_NULL_HANDLE;

uint32_t graphicsQueueFamilyIndex_Selected = UINT_MAX;

VkPhysicalDeviceMemoryProperties vkPhysicalDeviceMemoryProperties;

// get from getPhysicalDevice()

uint32_t physicalDeviceCount = 0;

VkPhysicalDevice* vkPhysicalDevice_Array = NULL;


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

// for color images
uint32_t swapchainImageCount = UINT32_MAX; 
VkImage* SwapchainImage_Array = NULL;
VkImageView* SwapchainImageView_Array = NULL;

// for depth image
VkFormat vkFormat_Depth = VK_FORMAT_UNDEFINED;

VkImage vkImage_Depth = VK_NULL_HANDLE;

VkDeviceMemory vkDeviceMemory_Depth = VK_NULL_HANDLE;

VkImageView vkImageView_Depth = VK_NULL_HANDLE;

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

VkClearDepthStencilValue vkClearDepthStencilValue;

BOOL bInitialised = NO;

uint32_t currentImageIndex = UINT32_MAX;

// Validation
BOOL bValidation = YES;

uint32_t enabledValidationLayerCount = 0;

const char* enabledValidationLayerNames_Array[1];  // for VK_LAYER_CRONOS_validation

VkDebugReportCallbackEXT vkDebugReportCallbackEXT = VK_NULL_HANDLE;

PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT_fnptr = NULL;

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



// forward interface declarations
@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface View : NSView
@end

// entry point function
int main(int argc, char* argv[])
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

    NSApplication *app = [NSApplication sharedApplication];
    [app setActivationPolicy:NSApplicationActivationPolicyRegular];

    AppDelegate *delegate = [[AppDelegate alloc] init];
    [app setDelegate:delegate];

    [app run];

    [delegate release];
    [pool release];

    return 0;
}

// app delegate implementation
@implementation AppDelegate
{
    NSWindow *window;
    View *view;
}

-(void)applicationDidFinishLaunching:(NSNotification*)notification
{
    NSBundle* appBundle = [NSBundle mainBundle];
    NSString* appDirPath = [appBundle bundlePath];
    NSString* parentDirPath = [appDirPath stringByDeletingLastPathComponent];

    NSString* logFileNameWithPath = [NSString stringWithFormat:@"%@/log.txt", parentDirPath];
    const char* pszLogFileNameWithPath = [logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];

    gpFile = fopen(pszLogFileNameWithPath, "w");
    if (gpFile == NULL)
    {
        printf("Log file not created");
        [NSApp terminate:self];
        return;
    }

    fprintf(gpFile, "Program started successfully\n");

    NSRect win_rect = NSMakeRect(0.0, 0.0, 800.0, 600.0);

    window = [[NSWindow alloc] initWithContentRect:win_rect
                                         styleMask:NSWindowStyleMaskTitled |
                                                   NSWindowStyleMaskClosable |
                                                   NSWindowStyleMaskMiniaturizable |
                                                   NSWindowStyleMaskResizable
                                           backing:NSBackingStoreBuffered
                                             defer:NO];

    [window setTitle:@"PRS : Cocoa window"];
    [window center];
    [window setBackgroundColor:[NSColor blackColor]];

    view = [[View alloc] initWithFrame:win_rect];
    [window setContentView:view];

    [window setDelegate:self];

    [window makeKeyAndOrderFront:self];
    [NSApp activateIgnoringOtherApps:YES];
}

-(void)applicationWillTerminate:(NSNotification*)notification
{
}

-(void)dealloc
{
    [view release];
    [window release];
    [super dealloc];
}
@end

// View implementation
@implementation View
    {
        @private
            CVDisplayLinkRef displayLink;
    }

-(id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if(self)
    {
        // convert our view into CAMetalLayered backing view
        [self setWantsLayer:YES];

        int result = [self initialise];

        if (result == -1)
            fprintf(gpFile, "Initialisation failed\n");
        else
            fprintf(gpFile, "Initialisation successful\n");

        // create a display link capable of being used with all active displays
        CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);

        // set the display link as our rendering output callback
        CVDisplayLinkSetOutputCallback(displayLink, &displayLinkCallback, self);

        // activate the display link
        CVDisplayLinkStart(displayLink);

    }
    return self;
}

-(void)windowDidBecomeKey:(NSNotification*)notification
{
    bActiveWindow = YES;
}

-(void)windowDidResignKey:(NSNotification*)notification
{

    bActiveWindow = NO;
}

-(NSSize)windowWillResize:(NSWindow*)sender toSize:(NSSize)frameSize
{
    CVDisplayLinkStop(displayLink);
    
    if(bWindowMinimized == NO)
    {
        [self Resize:frameSize.width :frameSize.height];
    }
    return frameSize;
}

-(void)windowDidResize : (NSNotification*)notification 
{
    if(bWindowMinimized == NO)
    {
        CVDisplayLinkStart(displayLink);
    }
}

-(void)windowWillMiniaturize:(NSNotification*)notification
{
    // stop the display link
    CVDisplayLinkStop(displayLink);

    bWindowMinimized = YES;
}

-(void)windowDidDeminiaturize:(NSNotification*)notification
{
    // start the display link again
    CVDisplayLinkStart(displayLink);

    bWindowMinimized = NO;
}

-(void)windowWillClose:(NSNotification*)notification
{
    [self uninitialise];
    [NSApp terminate:self];
}

-(CVReturn)getFrameForTime:(const CVTimeStamp*)pOutputTime
{
    // code
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

    // render the sence
    [self drawView];

    [pool release];

    return kCVReturnSuccess;
    
}

-(void)drawRect:(NSRect)dirtyRect
{
    [self drawView];
}

-(void)drawView
{
    [self display];
    [self update];
}

+(Class)layerClass
{
    //code
    return [CAMetalLayer class];
}

// continusoly demand the updated layer which is updated by redering
-(BOOL)wantsUpdateLayer
{
    // code
    return YES;
}

// to have the result of set wantsUpdateLayer follwing function should be return YES if resizing is done

-(CALayer*)makeBackingLayer
{
    CALayer *layer = [[[self class] layerClass] layer];

    CGSize viewSize = [self convertSizeToBacking:CGSizeMake(1.0, 1.0)];

    [layer setContentsScale:MIN(viewSize.width, viewSize.height)];

    return layer;
}

-(BOOL)acceptsFirstResponder
{
    [[self window] makeFirstResponder:self];
    return YES;
}

-(void)keyDown:(NSEvent*)event
{
    int key = (int)[[event characters] characterAtIndex:0];

    switch(key)
    {
        case 27:
            if(bFullScreen == YES)
            {
                [[self window] toggleFullScreen:self];
                bFullScreen = NO;
            }
            [[self window] performClose:self];
            break;

        case 'F':
        case 'f':
            [[self window] toggleFullScreen:nil];
            bFullScreen = !bFullScreen;
            break;
    }
}

-(void)dealloc
{
    if(displayLink)
    {
        CVDisplayLinkStop(displayLink);
        CVDisplayLinkRelease(displayLink);
        displayLink = NULL;
    }
    [super dealloc];
}

// user methods
-(VkResult) initialise    
{
	// function declarations

    // chnages
    //1. change all TRUE to YES and all FALSE to NO
    //2. chnage all user define functions declaration call to objective c method definition syntax
    // 3. chnage all function declarator in objective c method definition to objective c method definition syntax
    // 4. remove all function definitions and replace with objective c method definition syntax

    // conceptual chnages function
    // 1. createVulkanInstance()
    // 2. fillInstanceExtensionNames()
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
	vkresult = createVulkanInstance();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createVulkanInstance() function failed\n");
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createVulkanInstance() succeeded\n");
	}

	// variable declarations

	// create Vulkan Presentation Surface
	vkresult = getSupportedSurface();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : getSupportedSurface() function failed\n");
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : getSupportedSurface() succeeded\n");
	}

	// select required physical device and its queue family index
	vkresult = getPhysicalDevice();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : getPhysicalDevice() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : getPhysicalDevice() succeeded\n");
	}

	// print vulkan info
	vkresult = printVkInfo();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : printVkInfo() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : printVkInfo() succeeded\n");
	}

	vkresult = createVulkanDevice();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createVulkanDevice() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createVulkanDevice() succeeded\n");
	}

	// get device queue
	getDeviceQueue();

	// createSwapchain
	vkresult = createSwapchain(VK_FALSE);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createSwapchain() function failed (%d)\n", vkresult);
		return VK_ERROR_INITIALIZATION_FAILED; // Hardcoded return value
	}
	else
	{
		fprintf(gpFile, "initialise() : createSwapchain() succeeded\n");
	}

	vkresult = createImagesAndImageViews();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createImagesAndImageViews() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createImagesAndImageViews() succeeded\n");
	}

	vkresult = createCommandPool();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createCommandPool() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createCommandPool() succeeded\n");
	}

	vkresult = createCommandBuffers();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createCommandBuffers() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createCommandBuffers() succeeded\n");
	}

	// craete VertexBuffer
	vkresult = createVertexBuffer();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createVertexBuffer() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createVertexBuffer() succeeded\n");
	}

	// createUniform Buffer
	vkresult = createUniformBuffer();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createUniformBuffer() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createUniformBuffer() succeeded\n");
	}



	vkresult = createShaders();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createShaders() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createShaders() succeeded\n");
	}

	vkresult = createDiscriptorSetLayout();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createDiscriptorSetLayout() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createDiscriptorSetLayout() succeeded\n");
	}

	vkresult = createPiplineLayout();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createPiplineLayout() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createPiplineLayout() succeeded\n");
	}

	// create descriptorpool
	vkresult = createDescriptorpool();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createDescriptorpool() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createDescriptorpool() succeeded\n");
	}

	// create descriptorset
	vkresult = createDescriptorSet();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createDescriptorset() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createDescriptorset() succeeded\n");
	}



	vkresult = createRenderPass();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createRenderPass() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createRenderPass() succeeded\n");
	}

	vkresult = createPipline();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createPipline() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createPipline() succeeded\n");
	}

	vkresult = createframeBuffers();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createframeBuffer() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createframeBuffer() succeeded\n");
	}

	// craete semaphores
	vkresult = createSemaphores();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createSemaphores() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createSemaphores() succeeded\n");
	}

	//create Fences
	vkresult = createFences();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createFences() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createFences() succeeded\n");
	}

	// initialise clear color values
	memset((void*)&vkClearColorValue, 0, sizeof(VkClearColorValue));

	vkClearColorValue.float32[0] = 0.0f;
	vkClearColorValue.float32[1] = 0.0f;
	vkClearColorValue.float32[2] = 1.0f;
	vkClearColorValue.float32[3] = 1.0f;  // analogse to glclear color


	memset((void*)&vkClearDepthStencilValue, 0, sizeof(VkClearDepthStencilValue));

	// set defalut clear depth
	vkClearDepthStencilValue.depth = 1.0f;
	vkClearDepthStencilValue.stencil = 0;

	// build commmand buffers
	vkresult = buildCommandBuffers();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : buildCommandBuffers() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : buildCommandBuffers() succeeded\n");
	}

	// initialisation is completed

	bInitialised = TRUE;


	fprintf(gpFile, "******************************************* Initialise comment *****************************\n");

	fprintf(gpFile, "Initialised()  :  Initialisation() Complete Successfully\n");

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
	if (bInitialised == FALSE)
	{
		fprintf(gpFile, "resize() : Initialisation yet not completed or failed\n");
		vkresult = VK_ERROR_INITIALIZATION_FAILED;
		return vkresult;
	}

	// as recreation of swapchain is needed we are going to repeate many steps of initialise again hence set bInitialised  =  FALSE again
	bInitialised = FALSE;

	// set global winwidth and winheight variables
	winWidth = width;
	winHeight = heigth;

	// wait for device to complete in hand task
	if (vkDevice)
	{
		vkDeviceWaitIdle(vkDevice);
		fprintf(gpFile, " resize() : \n vkDeviceWaitIdle() is Done\n");
	}

	// check presence of swapchain
	if (vkSwapchainKHR == VK_NULL_HANDLE)
	{
		fprintf(gpFile, "\n resize() : swapchain is aleady null cannot proceed\n");
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
		fprintf(gpFile, " resize() : \nFree commandbuffers freed\n");

	}

	// destroy commandbuffer
	for (uint32_t i = 0; i < swapchainImageCount; i++)
	{
		vkFreeCommandBuffers(vkDevice, vkcommandpool, 1, &vkCommandBuffer_Array[i]);
		//vkDestroyImageView(vkDevice, swapchainImageView_array[i], NULL);
		fprintf(gpFile, "\n resize() : Free commandbuffers freed\n");
	}

	if (vkCommandBuffer_Array)
	{
		free(vkCommandBuffer_Array);
		vkCommandBuffer_Array = NULL;
		fprintf(gpFile, "\n resize() : commandbuffers Array frred\n");
	}

	// destroy pipline
	if (vkPipeline)
	{
		vkDestroyPipeline(vkDevice, vkPipeline, NULL);
		vkPipeline = VK_NULL_HANDLE;
		fprintf(gpFile, "\n resize() : Free vkPipeline: Pipeline freed successfully.\n");
	}

	// destroy pipline layout
	if (vkPipelineLayout)
	{
		vkDestroyPipelineLayout(vkDevice, vkPipelineLayout, NULL);
		vkPipelineLayout = VK_NULL_HANDLE;
		fprintf(gpFile, "\n resize() : Free vkPipelineLayout freed\n");
	}

	// destroy render pass
	if (vkRenderpass)
	{
		vkDestroyRenderPass(vkDevice, vkRenderpass, NULL);
		vkRenderpass = VK_NULL_HANDLE;
		fprintf(gpFile, "\n resize() : Free vkRenderpass freed\n");
	}

	// Destroy depth image view
	if (vkImageView_Depth)
	{
		vkDestroyImageView(vkDevice, vkImageView_Depth, NULL);
		vkImageView_Depth = VK_NULL_HANDLE;
	}

	// Free memory for depth image
	if (vkDeviceMemory_Depth)
	{
		vkFreeMemory(vkDevice, vkDeviceMemory_Depth, NULL);
		vkDeviceMemory_Depth = VK_NULL_HANDLE;
	}

	// destroy depth image
	if (vkImage_Depth)
	{
		vkDestroyImage(vkDevice, vkImage_Depth, NULL);
		vkImage_Depth = VK_NULL_HANDLE;
	}

	// destroy ImageView
	for (uint32_t i = 0; i < swapchainImageCount; i++)
	{
		vkDestroyImageView(vkDevice, SwapchainImageView_Array[i], NULL);
		fprintf(gpFile, "\nresize() : Free swapchainImage_array images freed\n");
	}
	if (SwapchainImageView_Array)
	{
		free(SwapchainImageView_Array);
		SwapchainImageView_Array = NULL;
	}

	/*for (uint32_t i = 0; i < swapchainImageCount; i++)
		{
			vkDestroyImage(vkDevice, SwapchainImage_Array[i], NULL);
			fprintf(gpFile, "\nFree SwapchainImage_Array images freed\n");
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
		vkSwapchainKHR = NULL;		//Bhanda swachha
		fprintf(gpFile, "\n resize() : vkSwapchainKHR is Freed\n");

	}


	////// RECREATE FIR RESIZE \\\\\\\

	// create swapchain
	vkresult = createSwapchain(VK_FALSE);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, " resize() : createSwapchain() function failed (%d)\n", vkresult);
		return VK_ERROR_INITIALIZATION_FAILED; // Hardcoded return value
	}

	vkresult = createImagesAndImageViews();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, " resize() : createImagesAndImageViews() function failed (%d)\n", vkresult);
		return(vkresult);
	}

	vkresult = createRenderPass();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, " resize() : createRenderPass() function failed (%d)\n", vkresult);
		return(vkresult);
	}

	vkresult = createPiplineLayout();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, " resize() : createPiplineLayout() function failed (%d)\n", vkresult);
		return(vkresult);
	}

	vkresult = createPipline();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, " resize() : createPipline() function failed (%d)\n", vkresult);
		return(vkresult);
	}

	vkresult = createframeBuffers();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, " resize() : createframeBuffer() function failed (%d)\n", vkresult);
		return(vkresult);
	}

	vkresult = createCommandBuffers();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, " resize() : createCommandBuffers() function failed (%d)\n", vkresult);
		return(vkresult);
	}


	// build commmand buffers
	vkresult = buildCommandBuffers();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, " resize() : buildCommandBuffers() function failed (%d)\n", vkresult);
		return(vkresult);
	}


	bInitialised = TRUE;

	return(vkresult);
}


-(VkResult) display
{
	 
	// variable declarations
	VkResult vkresult = VK_SUCCESS;

	// code

	// if control comes here before initilisation gets completed return false

	if (bInitialised == FALSE)
	{
		fprintf(gpFile, "display(): initliasation yet not completed\n");
		return (VkResult)VK_FALSE;
	}

	// acquire index of next swapchain image
	vkresult = vkAcquireNextImageKHR(vkDevice, vkSwapchainKHR, UINT64_MAX, vkSemaphore_backbuffer, VK_NULL_HANDLE, &currentImageIndex);
	if (vkresult != VK_SUCCESS && vkresult != VK_SUBOPTIMAL_KHR && vkresult != VK_ERROR_OUT_OF_DATE_KHR)
	{
		fprintf(gpFile, "display() : vkAcquireNextImageKHR failed with error: %d\n", vkresult);
		return(vkresult);
	}

	// use fence to allow host to wait for completion of execution previous commmand buffer

	vkresult = vkWaitForFences(vkDevice, 1, &vkFence_Array[currentImageIndex], VK_TRUE, UINT64_MAX);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "display() : vkWaitForFences failed with error: %d\n", vkresult);
		return(vkresult);
	}

	// now ready the fences for next command buffer
	vkresult = vkResetFences(vkDevice, 1, &vkFence_Array[currentImageIndex]);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "display() : vkResetFences failed with error: %d\n", vkresult);
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
		fprintf(gpFile, "display() : vkQueueSubmit failed with error: %d\n", vkresult);
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
            fprintf(gpFile, "display() : vkQueuePresentKHR failed with error: %d\n", vkresult);
            return(vkresult);
        }
	}

    vkDeviceWaitIdle(vkDevice);

	vkresult = updateUniformbuffer();
	if(vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "display() : updateUniformbuffer() failed with error: %d\n", vkresult);
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
    if(displayLink)
    {
        CVDisplayLinkStop(displayLink);
        CVDisplayLinkRelease(displayLink);
        displayLink = NULL;
    }
    // if window is in full screen mode then first exit from full screen mode

	 if(bFullScreen == YES)
    {
        [[self window] toggleFullScreen:nil];
        bFullScreen = NO;
    }


	//No need to destroy/uninitialize vkQueue


	//Vulkan related any destruction *HAS TO BE AFTER VkDevice*
	//because any resources related to vulkan device ae all done so resource freeing 

	//Destroy vulkan device
	if (vkDevice)
	{
		vkDeviceWaitIdle(vkDevice);
		fprintf(gpFile, "\n vkDeviceWaitIdle() is Done\n");


		//Free swapchain Images

		for (uint32_t i = 0; i < swapchainImageCount; i++)
		{
			vkDestroyFence(vkDevice, vkFence_Array[i], NULL);
			fprintf(gpFile, "vkDestroyFence vkFence_array i = %d Completedestroyed!\n", i);
		}
		if (vkFence_Array)
		{
			free(vkFence_Array);
			vkFence_Array = NULL;
			fprintf(gpFile, "\nFree vkFence_array freed\n");
		}

		if (vkSemaphore_rendercomplete)
		{
			vkDestroySemaphore(vkDevice, vkSemaphore_rendercomplete, NULL);
			vkSemaphore_rendercomplete = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFree vkSemaphore_RenderComplete freed\n");


		}
		if (vkSemaphore_backbuffer)
		{
			vkDestroySemaphore(vkDevice, vkSemaphore_backbuffer, NULL);
			vkSemaphore_backbuffer = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFree vkSemaphore_backbuffer freed\n");

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
			fprintf(gpFile, "\nFree commandbuffers freed\n");

		}

		if (vkDescriptorSetLayout)
		{
			vkDestroyDescriptorSetLayout(vkDevice, vkDescriptorSetLayout, NULL);
			vkDescriptorSetLayout = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFree vkDescriptorSetLayout freed\n");
		}

		if (vkPipelineLayout)
		{
			vkDestroyPipelineLayout(vkDevice, vkPipelineLayout, NULL);
			vkPipelineLayout = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFree vkPipelineLayout freed\n");
		}

		if (vkPipeline)
		{
			vkDestroyPipeline(vkDevice, vkPipeline, NULL);
			vkPipeline = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFree vkPipeline: Pipeline freed successfully.\n");
		}


		if (vkRenderpass)
		{
			vkDestroyRenderPass(vkDevice, vkRenderpass, NULL);
			vkRenderpass = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFree vkRenderpass freed\n");
		}

		// destroy descriptorpool
		// when descriptor pull is destroyed descriptor set created by that pull get destroy imlicitly
		if (vkDescriptorPool)
		{
			vkDestroyDescriptorPool(vkDevice, vkDescriptorPool, NULL);
			vkDescriptorPool = VK_NULL_HANDLE;
			vkDescriptorSet = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFreed vkDescriptorPool and vkDescriptorSet\n");
		}


		// destroy shader modules
		if (vkShaderModule_fragment_shader)
		{
			vkDestroyShaderModule(vkDevice, vkShaderModule_fragment_shader, NULL);
			vkShaderModule_fragment_shader = NULL;
			fprintf(gpFile, "\nFree vkShaderModule_fragment_shader freed\n");
		}

		if (vkShaderModule_vertex_shader)
		{
			vkDestroyShaderModule(vkDevice, vkShaderModule_vertex_shader, NULL);
			vkShaderModule_vertex_shader = NULL;
			fprintf(gpFile, "\nFree vkShaderModule_vertex_shader freed\n");
		}

		// Destroy uniform buffer
		if (uniformData.vkBuffer)
		{
			vkDestroyBuffer(vkDevice, uniformData.vkBuffer, NULL);
			uniformData.vkBuffer = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFreed uniformData.vkBuffer \n");
		}

		if (uniformData.vkDeviceMemory)
		{
			vkFreeMemory(vkDevice, uniformData.vkDeviceMemory, NULL);
			uniformData.vkDeviceMemory = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFreed uniformData.vkDeviceMemory \n");
		}




		if (vertexData_Position.vkDeviceMemory)
		{
			vkFreeMemory(vkDevice, vertexData_Position.vkDeviceMemory, NULL);
			vertexData_Position.vkDeviceMemory = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFree vertexData_Position.vkDeviceMemory freed\n");

		}

		if (vertexData_Position.vkBuffer)
		{
			vkDestroyBuffer(vkDevice, vertexData_Position.vkBuffer, NULL);
			vertexData_Position.vkBuffer = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFree vertexData_Position.vkBuffer freed\n");

		}


		for (uint32_t i = 0; i < swapchainImageCount; i++)
		{
			vkFreeCommandBuffers(vkDevice, vkcommandpool, 1, &vkCommandBuffer_Array[i]);
			//vkDestroyImageView(vkDevice, swapchainImageView_array[i], NULL);
			fprintf(gpFile, "\nFree commandbuffers freed\n");
		}
		if (vkCommandBuffer_Array)
		{
			free(vkCommandBuffer_Array);
			vkCommandBuffer_Array = NULL;
		}

		vkDestroyCommandPool(vkDevice, vkcommandpool, NULL);
		//vkCommandPool = VK_NULL_HANDLE;
		fprintf(gpFile, "\n vkCommandPool is Freed\n");

		// Destroy depth image view
		if (vkImageView_Depth)
		{
			vkDestroyImageView(vkDevice, vkImageView_Depth, NULL);
			vkImageView_Depth = VK_NULL_HANDLE;
			fprintf(gpFile, "\n[Depth] VkImageView destroyed.\n");
		}

		// Free memory for depth image
		if (vkDeviceMemory_Depth)
		{
			vkFreeMemory(vkDevice, vkDeviceMemory_Depth, NULL);
			vkDeviceMemory_Depth = VK_NULL_HANDLE;
			fprintf(gpFile, "\n[Depth] VkDeviceMemory freed.\n");
		}

		// destroy depth image
		if (vkImage_Depth)
		{
			vkDestroyImage(vkDevice, vkImage_Depth, NULL);
			vkImage_Depth = VK_NULL_HANDLE;
			fprintf(gpFile, "\n[Depth] vkImage_Depth freed.\n");

		}


		for (uint32_t i = 0; i < swapchainImageCount; i++)
		{
			vkDestroyImageView(vkDevice, SwapchainImageView_Array[i], NULL);
			fprintf(gpFile, "\nFree swapchainImage_array images freed\n");
		}
		if (SwapchainImageView_Array)
		{
			free(SwapchainImageView_Array);
			SwapchainImageView_Array = NULL;
		}


		/*for (uint32_t i = 0; i < swapchainImageCount; i++)
		{
			vkDestroyImage(vkDevice, SwapchainImage_Array[i], NULL);
			fprintf(gpFile, "\nFree SwapchainImage_Array images freed\n");
		}*/

		if (SwapchainImage_Array)
		{
			free(SwapchainImage_Array);
			SwapchainImage_Array = NULL;
		}

		//Destroy Fences

		//Command buffer free
		//actual array free

		//destroy 	swapchain
		if (vkSwapchainKHR)
		{
			vkDestroySwapchainKHR(vkDevice, vkSwapchainKHR, NULL);
			vkSwapchainKHR = NULL;		//Bhanda swachha
			fprintf(gpFile, "\n vkSwapchainKHR is Freed\n");

		}

		vkDestroyDevice(vkDevice, NULL);
		vkDevice = VK_NULL_HANDLE;
		fprintf(gpFile, "\n vkDestroyDevice() is Done\n");

	}
	//No need to Destroy selected physical device


	if (vkSurfaceKHR)
	{
		vkDestroySurfaceKHR(vkInstance, vkSurfaceKHR, NULL);
		vkSurfaceKHR = VK_NULL_HANDLE;
		fprintf(gpFile, "\nvkDestroySurfaceKHR Done\n");
	}

	if (vkDebugReportCallbackEXT && vkDestroyDebugReportCallbackEXT_fnptr)
	{

		vkDestroyDebugReportCallbackEXT_fnptr(vkInstance, vkDebugReportCallbackEXT, NULL);
		vkDebugReportCallbackEXT = VK_NULL_HANDLE;
		vkDestroyDebugReportCallbackEXT_fnptr = NULL;
		fprintf(gpFile, "\nvkDestroyDebugReportCallbackEXT_fnptr Done\n");

	}

	//uninitialize/destroy vulkan instance
	if (vkInstance)
	{
		vkDestroyInstance(vkInstance, NULL);
		vkInstance = VK_NULL_HANDLE;
		fprintf(gpFile, "\nvkDestroyInstance Done\n");
	}


	if (gpFile)
	{
		fprintf(gpFile, "\nUninitialize->Program Terminated Successfully.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}

@end

// global function definitions
CVReturn displayLinkCallback(CVDisplayLinkRef myDisplayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* renderer)
{
    // code
    CVReturn result = [(View*)renderer getFrameForTime:outputTime];

    return result;
}




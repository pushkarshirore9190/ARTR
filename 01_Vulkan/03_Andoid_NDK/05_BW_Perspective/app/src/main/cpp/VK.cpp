
// android header files
#include<android_native_app_glue.h> // everuhting related with pure native activity need this (mainly android_main function and android_app struct)
#include<android/log.h> // for android_log_print function to print log in logcat
#include<math.h> // for sqrt function to calculate distance between two points
#include<memory.h>

// android platform
#define VK_USE_PLATFORM_ANDROID_KHR 
#include<vulkan/vulkan.h>

// glm related macros and header files
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TWO_ONE
#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"

// macros 
#define _ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))


// android callback function declarationa
void engine_handle_cmd(struct android_app*, int32_t);
int32_t engine_handle_input(struct android_app*, AInputEvent*);


// android global variables
typedef struct
{
    struct android_app *app;
    bool bActive;
} Engine;

ANativeWindow* androidNativeWindow = NULL;
AAssetManager* androidAssetManager = NULL;

// Event handling related global variables
long touchStartTime = 0;
long pendingSingleTapTime = 0;

bool bTouchDown = false;
bool bDragging = false;
bool bLongPressDetected = false;
bool bDoubleTabDetected = false;
bool bPendingSingleTap = false;

float touchStartX = 0.0f;
float touchStartY = 0.0f;
float lastTapX = 0.0f;
float lastTapY = 0.0f;

bool bDone = false;
const char* gpszAppName = "ARTR";
int winWidth = 0;
int winHeight = 0;

// vulkun related global variables

uint32_t enabledInstanceExtensionCount = 0;

// android instance extension names array
const char* enabledInstanceExtensionNames_array[3];  // VK_KHR_SURFACE_EXTENSION_NAME and VK_KHR_ANDROID_SURFCACE_EXTENSION_NAME and VK_EXT_DEBUG_REPORT_EXTENSION_NAME


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

bool bInitialised = false;

uint32_t currentImageIndex = UINT32_MAX;

// Validation
bool bValidation = true;

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

void android_main(struct android_app* state)
{
	// Function declarations
	VkResult display(void);
	void update(void);

	// variable declarations
	VkResult vkresult = VK_SUCCESS;

    // full screen hide status bar
    JavaVM* vm = state->activity->vm;

    JNIEnv* env = NULL;

    // attach current thread to JVM and get JNIEnv
    vm->AttachCurrentThread(&env, NULL);

    jobject activityObject = state->activity->clazz;

    jclass activityClass = env->GetObjectClass(activityObject);

    jclass windowClass = env->FindClass("android/view/Window");

    jclass viewClass = env->FindClass("android/view/View");

    // get window method
    jmethodID getWindowMethod = env->GetMethodID(activityClass, "getWindow", "()Landroid/view/Window;");

    jobject windowObject = env->CallObjectMethod(activityObject, getWindowMethod);

    // get decor view method
    jmethodID getDecorViewMethod = env->GetMethodID(windowClass, "getDecorView", "()Landroid/view/View;");

    jobject decorViewObject = env->CallObjectMethod(windowObject, getDecorViewMethod);

    // get 8 viewclass static fields

    const int flag_SYSTEM_UI_FLAG_IMMERSIVE =
    env->GetStaticIntField(viewClass,env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_IMMERSIVE", "I"));

    const int flag_SYSTEM_UI_FLAG_LAYOUT_STABLE = env->GetStaticIntField(viewClass, env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_LAYOUT_STABLE", "I"));
    
    const int flag_SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION = env->GetStaticIntField(viewClass, env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION", "I"));

    const int flag_SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN = env->GetStaticIntField(viewClass, env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN", "I"));

    const int flag_SYSTEM_UI_FLAG_HIDE_NAVIGATION = env->GetStaticIntField(viewClass, env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_HIDE_NAVIGATION", "I"));

    const int flag_SYSTEM_UI_FLAG_FULLSCREEN = env->GetStaticIntField(viewClass, env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_FULLSCREEN", "I"));

    const int flag_SYSTEM_UI_FLAG_LOW_PROFILE = env->GetStaticIntField(viewClass, env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_LOW_PROFILE", "I"));

    const int flag_SYSTEM_UI_FLAG_IMMERSIVE_STICKY = env->GetStaticIntField(viewClass, env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_IMMERSIVE_STICKY", "I"));


    jmethodID setSystemUiVisibilityMethod = env->GetMethodID(viewClass, "setSystemUiVisibility", "(I)V");

    // set system UI visibility
    env->CallVoidMethod(decorViewObject, setSystemUiVisibilityMethod, flag_SYSTEM_UI_FLAG_IMMERSIVE | flag_SYSTEM_UI_FLAG_LAYOUT_STABLE | flag_SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION | flag_SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN | flag_SYSTEM_UI_FLAG_HIDE_NAVIGATION | flag_SYSTEM_UI_FLAG_FULLSCREEN | flag_SYSTEM_UI_FLAG_LOW_PROFILE | flag_SYSTEM_UI_FLAG_IMMERSIVE_STICKY);

    // change to landscape mode
    jclass activityInfoClass = env->FindClass("android/content/pm/ActivityInfo");

    const int flag_SCREEN_ORIENTATION_LANDSCAPE = env->GetStaticIntField(activityInfoClass, env->GetStaticFieldID(activityInfoClass, "SCREEN_ORIENTATION_LANDSCAPE", "I"));

    jmethodID setRequestedOrientationMethod = env->GetMethodID(activityClass, "setRequestedOrientation", "(I)V");

    // Now actually call set orientation method and pass the required flag to set landscape mode
    env->CallVoidMethod(activityObject, setRequestedOrientationMethod, flag_SCREEN_ORIENTATION_LANDSCAPE);

    // detach current thread from JVM
    vm->DetachCurrentThread();

    // code
    Engine engine;
    memset((void*)&engine, 0, sizeof(Engine));

    // initialize state
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;

    engine.app = state;
    __android_log_print(ANDROID_LOG_INFO, "PRS:", "android_main() : Started Successfully\n"); 

    // loop
    while(1)
    {
        int Identifier;
        struct android_poll_source* source = NULL;
        while((Identifier = ALooper_pollOnce(engine.bActive ? 0 : -1, NULL, NULL, (void**)&source)) >= 0)
        {
            // process system events
            if(source != NULL)
            {
                source->process(state, source);
            }

            // check when to exit
            if(state ->destroyRequested != 0)
            {
                return;
            }
    
        }

        // handle long press
        if(bTouchDown == true && bDragging == false && bLongPressDetected == false && bDoubleTabDetected == false)
        {
            struct timespec ts;
            memset((void*)&ts, 0, sizeof(ts));

            clock_gettime(CLOCK_MONOTONIC, &ts);

            long now = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);

            if(now - touchStartTime >= 500) // 500 ms for long press
            {
                bLongPressDetected = true;
                __android_log_print(ANDROID_LOG_INFO, "PRS:", "Long Press Detected\n");
            }

        }

        // handle single tap
        if(bPendingSingleTap == true)
        {
            struct timespec ts;
            memset((void*)&ts, 0, sizeof(ts));

            clock_gettime(CLOCK_MONOTONIC, &ts);

            long now = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);

            if(now - pendingSingleTapTime >= 300) // 300 ms for single tap
            {
                bPendingSingleTap = false;
                __android_log_print(ANDROID_LOG_INFO, "PRS:", "Single Tap Detected\n");
            }
        }

		// game loop
		if(engine.bActive == true && bInitialised == true)
		{
			// display
			vkresult = display();
			if (vkresult != VK_SUCCESS && vkresult != VK_ERROR_OUT_OF_DATE_KHR && vkresult != VK_SUBOPTIMAL_KHR)
			{
				__android_log_print(ANDROID_LOG_INFO, "PRS:", "android_main() : display() function failed (%d)\n", vkresult);
			}
			
			// update
			update();

		} 
    }

}

void engine_handle_cmd(struct android_app* app, int32_t cmd)
{
	// function declarations
	VkResult initialise(void);
	void uninitialise(void);

	// variable declarations
	VkResult vkresult = VK_SUCCESS;
	 
    // code
    Engine* engine = (Engine*)app->userData;
    switch(cmd)
    {
        case APP_CMD_SAVE_STATE:
            engine->bActive = false;
            __android_log_print(ANDROID_LOG_INFO, "PRS:", "APP_CMD_SAVE_STATE : Saving State\n");
            break;

        case APP_CMD_INIT_WINDOW: // like WM_CREATE
            if(app->window != NULL)
            {
                 

                androidNativeWindow = engine->app->window;

				androidAssetManager = app->activity->assetManager;

				winWidth = ANativeWindow_getWidth(androidNativeWindow);
				winHeight = ANativeWindow_getHeight(androidNativeWindow);

				if(bInitialised == false)
				{
					vkresult = initialise();
					if (vkresult != VK_SUCCESS)
					{
						__android_log_print(ANDROID_LOG_INFO, "PRS:", "engine_handle_cmd() : initialise() function failed (%d)\n", vkresult);
						return;
					}
					else
					{
						__android_log_print(ANDROID_LOG_INFO, "PRS:", "engine_handle_cmd() : initialise() succeeded\n");
					}
				}

				engine->bActive = true;
            
                __android_log_print(ANDROID_LOG_INFO, "PRS:", "APP_CMD_INIT_WINDOW : Window is created\n");
            }
            else
            {
                androidNativeWindow = NULL;
            }

            break;

        case APP_CMD_TERM_WINDOW: // like WM_DESTROY
			uninitialise();
            __android_log_print(ANDROID_LOG_INFO, "PRS:", "APP_CMD_TERM_WINDOW : Window Terminated\n");
            break;

        case APP_CMD_GAINED_FOCUS:
            engine->bActive = true;
            __android_log_print(ANDROID_LOG_INFO, "PRS:", "APP_CMD_GAINED_FOCUS : Gained Focus\n");
            break;

        case APP_CMD_LOST_FOCUS:
            engine->bActive = false;
            __android_log_print(ANDROID_LOG_INFO, "PRS:", "APP_CMD_LOST_FOCUS : Lost Focus\n");
            break;
    }
}

int32_t engine_handle_input(struct android_app* app, AInputEvent* event)
{
    Engine *engine = (Engine*)app->userData;
    int32_t eventType = AInputEvent_getType(event);

    switch(eventType)
    {
        case AINPUT_EVENT_TYPE_MOTION:
        {
            int32_t action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;

            switch (action)
            {
                case AMOTION_EVENT_ACTION_DOWN:
                {
                   touchStartX = AMotionEvent_getX(event, 0);
                   touchStartY = AMotionEvent_getY(event, 0);

                   struct timespec ts;
                   memset((void*)&ts, 0, sizeof(struct timespec));

                   clock_gettime(CLOCK_MONOTONIC, &ts);

                   touchStartTime = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
                   
                   bTouchDown = true;
                   bDragging = false;
                   bLongPressDetected = false;
                   bDoubleTabDetected = false; 

                   if(bPendingSingleTap == true)
                   {
                        long timeSinceLastTap = touchStartTime - pendingSingleTapTime;

                        float dX  = touchStartX - lastTapX;
                        float dY = touchStartY - lastTapY;

                        float distance = sqrtf((dX * dX) + (dY * dY));

                        if(timeSinceLastTap <= 300 && distance <= 100.0f)
                        {
                            bDoubleTabDetected = true;
                            __android_log_print(ANDROID_LOG_INFO, "PRS:", "Double Tap Detected\n");
                        }

                        bPendingSingleTap = false;
                   }   
                }
                break;

                case AMOTION_EVENT_ACTION_MOVE:
                {
                    float currentX = AMotionEvent_getX(event, 0);
                    float currentY = AMotionEvent_getY(event, 0);

                    float dX = currentX - touchStartX;
                    float dY = currentY - touchStartY;

                    float distance = sqrtf((dX * dX) + (dY * dY));

                    if(distance > 50.0f)
                    {
                        bDragging = true;
                    }
                }
                break;

                case AMOTION_EVENT_ACTION_UP:
                {
                   struct timespec ts;
                   memset((void*)&ts, 0, sizeof(struct timespec));

                   clock_gettime(CLOCK_MONOTONIC, &ts);

                   long currentTime = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
                   long duration = currentTime - touchStartTime;

                   float EndX = AMotionEvent_getX(event, 0);
                   float EndY = AMotionEvent_getY(event, 0);

                   float dX = EndX - touchStartX;
                   float dY = EndY - touchStartY;

                   float distance = sqrtf((dX * dX) + (dY * dY));

                   bTouchDown = false;

                   if(bDoubleTabDetected == true)
                   {
                   }
                   else if(bLongPressDetected == true)
                   {
                   }
                   else if(bDragging == true && distance > 150.0f)
                   {
                        __android_log_print(ANDROID_LOG_INFO, "PRS:", "Drag Detected\n");
                        ANativeActivity_finish(engine->app->activity);
                   }
                   else if(duration < 300 && distance < 50.0f)
                   {
                        bPendingSingleTap = true;
                        pendingSingleTapTime = currentTime;
                        lastTapX = EndX;
                        lastTapY = EndY;
                   }

                   bDragging = false;
                }
                break;
            }
        }
    }

    return 0;
}


VkResult initialise(void)
{
	// function declarations

	VkResult createVulkanInstance(void);
	VkResult getSupportedSurface(void);
	VkResult getPhysicalDevice(void);
	VkResult printVkInfo(void);
	VkResult createVulkanDevice(void);
	void getDeviceQueue(void);
	VkResult createSwapchain(VkBool32);
	VkResult createImagesAndImageViews(void);
	VkResult createCommandPool(void);
	VkResult createCommandBuffers(void);
	VkResult createVertexBuffer(void);
	VkResult createUniformBuffer(void);
	VkResult createShaders(void);
	VkResult createDiscriptorSetLayout(void);
	VkResult createPiplineLayout(void);
	VkResult createDescriptorpool(void);
	VkResult createDescriptorSet(void);
	VkResult createRenderPass(void);
	VkResult createPipline(void);
	VkResult createframeBuffers(void);
	VkResult createSemaphores(void);
	VkResult createFences(void);
	VkResult buildCommandBuffers(void);


	// variable declarations
	VkResult vkresult = VK_SUCCESS;

	// code
	vkresult = createVulkanInstance();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createVulkanInstance() function failed\n");
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createVulkanInstance() succeeded\n");
	}

	// variable declarations

	// create Vulkan Presentation Surface
	vkresult = getSupportedSurface();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : getSupportedSurface() function failed\n");
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : getSupportedSurface() succeeded\n");
	}

	// select required physical device and its queue family index
	vkresult = getPhysicalDevice();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : getPhysicalDevice() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : getPhysicalDevice() succeeded\n");
	}

	// print vulkan info
	vkresult = printVkInfo();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : printVkInfo() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : printVkInfo() succeeded\n");
	}

	vkresult = createVulkanDevice();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createVulkanDevice() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createVulkanDevice() succeeded\n");
	}

	// get device queue
	getDeviceQueue();

	// createSwapchain
	vkresult = createSwapchain(VK_FALSE);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createSwapchain() function failed (%d)\n", vkresult);
		return VK_ERROR_INITIALIZATION_FAILED; // Hardcoded return value
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createSwapchain() succeeded\n");
	}

	vkresult = createImagesAndImageViews();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createImagesAndImageViews() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createImagesAndImageViews() succeeded\n");
	}

	vkresult = createCommandPool();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createCommandPool() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createCommandPool() succeeded\n");
	}

	vkresult = createCommandBuffers();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createCommandBuffers() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createCommandBuffers() succeeded\n");
	}

	// craete VertexBuffer
	vkresult = createVertexBuffer();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createVertexBuffer() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createVertexBuffer() succeeded\n");
	}

	// createUniform Buffer
	vkresult = createUniformBuffer();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createUniformBuffer() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createUniformBuffer() succeeded\n");
	}



	vkresult = createShaders();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createShaders() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createShaders() succeeded\n");
	}

	vkresult = createDiscriptorSetLayout();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createDiscriptorSetLayout() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createDiscriptorSetLayout() succeeded\n");
	}

	vkresult = createPiplineLayout();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createPiplineLayout() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createPiplineLayout() succeeded\n");
	}

	// create descriptorpool
	vkresult = createDescriptorpool();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createDescriptorpool() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createDescriptorpool() succeeded\n");
	}

	// create descriptorset
	vkresult = createDescriptorSet();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createDescriptorset() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createDescriptorset() succeeded\n");
	}



	vkresult = createRenderPass();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createRenderPass() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createRenderPass() succeeded\n");
	}

	vkresult = createPipline();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createPipline() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createPipline() succeeded\n");
	}

	vkresult = createframeBuffers();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createframeBuffer() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createframeBuffer() succeeded\n");
	}

	// craete semaphores
	vkresult = createSemaphores();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createSemaphores() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createSemaphores() succeeded\n");
	}

	//create Fences
	vkresult = createFences();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createFences() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : createFences() succeeded\n");
	}

	// initialise clear color values
	memset((void*)&vkClearColorValue, 0, sizeof(VkClearColorValue));

	vkClearColorValue.float32[0] = 0.0f;
	vkClearColorValue.float32[1] = 0.0f;
	vkClearColorValue.float32[2] = 1.0f;
	vkClearColorValue.float32[3] = 1.0f;  // analogse to glclear color

	// build commmand buffers
	vkresult = buildCommandBuffers();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : buildCommandBuffers() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "initialise() : buildCommandBuffers() succeeded\n");
	}

	// initialisation is completed

	bInitialised = true;


	__android_log_print(ANDROID_LOG_INFO, "PRS:", "******************************************* Initialise comment *****************************\n");

	__android_log_print(ANDROID_LOG_INFO, "PRS:", "Initialised()  :  Initialisation() Complete Successfully\n");

	return(vkresult);
}



VkResult resize(int width, int heigth)
{
	// function declaration ;
	VkResult createSwapchain(VkBool32);
	VkResult createImagesAndImageViews(void);
	VkResult createCommandBuffers(void);
	VkResult createPiplineLayout(void);
	VkResult createRenderPass(void);
	VkResult createPipline(void);
	VkResult createframeBuffers(void);
	VkResult buildCommandBuffers(void);

	

	// variable declarations
	VkResult vkresult = VK_SUCCESS;

	// code
	if (heigth <= 0)
		heigth = 1;

	// check the bInitialised variable
	if (bInitialised == false)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "resize() : Initialisation yet not completed or failed\n");
		vkresult = VK_ERROR_INITIALIZATION_FAILED;
		return vkresult;
	}

	// as recreation of swapchain is needed we are going to repeate many steps of initialise again hence set bInitialised  =  false again
	bInitialised = false;

	// set global winwidth and winheight variables
	winWidth = width;
	winHeight = heigth;

	// wait for device to complete in hand task
	if (vkDevice)
	{
		vkDeviceWaitIdle(vkDevice);
		__android_log_print(ANDROID_LOG_INFO, "PRS:", " resize() : \n vkDeviceWaitIdle() is Done\n");
	}

	// check presence of swapchain
	if (vkSwapchainKHR == VK_NULL_HANDLE)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "\n resize() : swapchain is aleady null cannot proceed\n");
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", " resize() : \nFree commandbuffers freed\n");

	}

	// destroy commandbuffer
	for (uint32_t i = 0; i < swapchainImageCount; i++)
	{
		vkFreeCommandBuffers(vkDevice, vkcommandpool, 1, &vkCommandBuffer_Array[i]);
		//vkDestroyImageView(vkDevice, swapchainImageView_array[i], NULL);
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "\n resize() : Free commandbuffers freed\n");
	}

	if (vkCommandBuffer_Array)
	{
		free(vkCommandBuffer_Array);
		vkCommandBuffer_Array = NULL;
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "\n resize() : commandbuffers Array frred\n");
	}

	// destroy pipline
	if (vkPipeline)
	{
		vkDestroyPipeline(vkDevice, vkPipeline, NULL);
		vkPipeline = VK_NULL_HANDLE;
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "\n resize() : Free vkPipeline: Pipeline freed successfully.\n");
	}

	// destroy pipline layout
	if (vkPipelineLayout)
	{
		vkDestroyPipelineLayout(vkDevice, vkPipelineLayout, NULL);
		vkPipelineLayout = VK_NULL_HANDLE;
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "\n resize() : Free vkPipelineLayout freed\n");
	}

	// destroy render pass
	if (vkRenderpass)
	{
		vkDestroyRenderPass(vkDevice, vkRenderpass, NULL);
		vkRenderpass = VK_NULL_HANDLE;
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "\n resize() : Free vkRenderpass freed\n");
	}

	// destroy ImageView
	for (uint32_t i = 0; i < swapchainImageCount; i++)
	{
		vkDestroyImageView(vkDevice, SwapchainImageView_Array[i], NULL);
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nresize() : Free swapchainImage_array images freed\n");
	}
	if (SwapchainImageView_Array)
	{
		free(SwapchainImageView_Array);
		SwapchainImageView_Array = NULL;
	}

	/*for (uint32_t i = 0; i < swapchainImageCount; i++)
		{
			vkDestroyImage(vkDevice, SwapchainImage_Array[i], NULL);
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nFree SwapchainImage_Array images freed\n");
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "\n resize() : vkSwapchainKHR is Freed\n");

	}


	////// RECREATE FIR RESIZE \\\\\\\

	// create swapchain
	vkresult = createSwapchain(VK_FALSE);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", " resize() : createSwapchain() function failed (%d)\n", vkresult);
		return VK_ERROR_INITIALIZATION_FAILED; // Hardcoded return value
	}

	vkresult = createImagesAndImageViews();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", " resize() : createImagesAndImageViews() function failed (%d)\n", vkresult);
		return(vkresult);
	}

	vkresult = createRenderPass();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", " resize() : createRenderPass() function failed (%d)\n", vkresult);
		return(vkresult);
	}

	vkresult = createPiplineLayout();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", " resize() : createPiplineLayout() function failed (%d)\n", vkresult);
		return(vkresult);
	}

	vkresult = createPipline();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", " resize() : createPipline() function failed (%d)\n", vkresult);
		return(vkresult);
	}

	vkresult = createframeBuffers();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", " resize() : createframeBuffer() function failed (%d)\n", vkresult);
		return(vkresult);
	}

	vkresult = createCommandBuffers();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", " resize() : createCommandBuffers() function failed (%d)\n", vkresult);
		return(vkresult);
	}


	// build commmand buffers
	vkresult = buildCommandBuffers();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", " resize() : buildCommandBuffers() function failed (%d)\n", vkresult);
		return(vkresult);
	}


	bInitialised = true;

	return(vkresult);
}

VkResult display(void)
{

	// function declaration
	VkResult resize(int, int);
	VkResult updateUniformbuffer(void);
	 
	// variable declarations
	VkResult vkresult = VK_SUCCESS;

	// code

	// if control comes here before initilisation gets completed return false

	if (bInitialised == false)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "display(): initliasation yet not completed\n");
		return (VkResult)VK_FALSE;
	}

	// acquire index of next swapchain image
	vkresult = vkAcquireNextImageKHR(vkDevice, vkSwapchainKHR, UINT64_MAX, vkSemaphore_backbuffer, VK_NULL_HANDLE, &currentImageIndex);
	if (vkresult != VK_SUCCESS && vkresult != VK_SUBOPTIMAL_KHR)
	{
		if (vkresult == VK_ERROR_OUT_OF_DATE_KHR)
		{
			resize(winWidth, winHeight);
		}
		else
		{
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "display() : vkAcquireNextImageKHR failed with error: %d\n", vkresult);
			return(vkresult);
		}
	}

	// use fence to allow host to wait for completion of execution previous commmand buffer

	vkresult = vkWaitForFences(vkDevice, 1, &vkFence_Array[currentImageIndex], VK_TRUE, UINT64_MAX);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "display() : vkWaitForFences failed with error: %d\n", vkresult);
		return(vkresult);
	}

	// now ready the fences for next command buffer
	vkresult = vkResetFences(vkDevice, 1, &vkFence_Array[currentImageIndex]);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "display() : vkResetFences failed with error: %d\n", vkresult);
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "display() : vkQueueSubmit failed with error: %d\n", vkresult);
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
	if (vkresult != VK_SUCCESS && vkresult != VK_SUBOPTIMAL_KHR)
	{
		if (vkresult == VK_ERROR_OUT_OF_DATE_KHR)
		{
			resize(winWidth, winHeight);
		}
		else
		{
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "display() : vkQueuePresentKHR failed with error: %d\n", vkresult);
			return(vkresult);
		}
	}

	vkresult = updateUniformbuffer();
	if(vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "display() : updateUniformbuffer() failed with error: %d\n", vkresult);
		return(vkresult);
	}

	// here there will be your drawing code
	vkDeviceWaitIdle(vkDevice);


	return(vkresult);

}

void update(void)
{
	// code
}


void uninitialise(void)
{
	//No need to destroy/uninitialize vkQueue


	//Vulkan related any destruction *HAS TO BE AFTER VkDevice*
	//because any resources related to vulkan device ae all done so resource freeing 

	//Destroy vulkan device
	if (vkDevice)
	{
		vkDeviceWaitIdle(vkDevice);
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "\n vkDeviceWaitIdle() is Done\n");


		//Free swapchain Images

		for (uint32_t i = 0; i < swapchainImageCount; i++)
		{
			vkDestroyFence(vkDevice, vkFence_Array[i], NULL);
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "vkDestroyFence vkFence_array i = %d Completedestroyed!\n", i);
		}
		if (vkFence_Array)
		{
			free(vkFence_Array);
			vkFence_Array = NULL;
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nFree vkFence_array freed\n");
		}

		if (vkSemaphore_rendercomplete)
		{
			vkDestroySemaphore(vkDevice, vkSemaphore_rendercomplete, NULL);
			vkSemaphore_rendercomplete = VK_NULL_HANDLE;
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nFree vkSemaphore_RenderComplete freed\n");


		}
		if (vkSemaphore_backbuffer)
		{
			vkDestroySemaphore(vkDevice, vkSemaphore_backbuffer, NULL);
			vkSemaphore_backbuffer = VK_NULL_HANDLE;
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nFree vkSemaphore_backbuffer freed\n");

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
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nFree commandbuffers freed\n");

		}

		if (vkDescriptorSetLayout)
		{
			vkDestroyDescriptorSetLayout(vkDevice, vkDescriptorSetLayout, NULL);
			vkDescriptorSetLayout = VK_NULL_HANDLE;
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nFree vkDescriptorSetLayout freed\n");
		}

		if (vkPipelineLayout)
		{
			vkDestroyPipelineLayout(vkDevice, vkPipelineLayout, NULL);
			vkPipelineLayout = VK_NULL_HANDLE;
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nFree vkPipelineLayout freed\n");
		}

		if (vkPipeline)
		{
			vkDestroyPipeline(vkDevice, vkPipeline, NULL);
			vkPipeline = VK_NULL_HANDLE;
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nFree vkPipeline: Pipeline freed successfully.\n");
		}


		if (vkRenderpass)
		{
			vkDestroyRenderPass(vkDevice, vkRenderpass, NULL);
			vkRenderpass = VK_NULL_HANDLE;
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nFree vkRenderpass freed\n");
		}

		// destroy descriptorpool
		// when descriptor pull is destroyed descriptor set created by that pull get destroy imlicitly
		if (vkDescriptorPool)
		{
			vkDestroyDescriptorPool(vkDevice, vkDescriptorPool, NULL);
			vkDescriptorPool = VK_NULL_HANDLE;
			vkDescriptorSet = VK_NULL_HANDLE;
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nFreed vkDescriptorPool and vkDescriptorSet\n");
		}


		// destroy shader modules
		if (vkShaderModule_fragment_shader)
		{
			vkDestroyShaderModule(vkDevice, vkShaderModule_fragment_shader, NULL);
			vkShaderModule_fragment_shader = NULL;
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nFree vkShaderModule_fragment_shader freed\n");
		}

		if (vkShaderModule_vertex_shader)
		{
			vkDestroyShaderModule(vkDevice, vkShaderModule_vertex_shader, NULL);
			vkShaderModule_vertex_shader = NULL;
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nFree vkShaderModule_vertex_shader freed\n");
		}

		// Destroy uniform buffer
		if (uniformData.vkBuffer)
		{
			vkDestroyBuffer(vkDevice, uniformData.vkBuffer, NULL);
			uniformData.vkBuffer = VK_NULL_HANDLE;
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nFreed uniformData.vkBuffer \n");
		}

		if (uniformData.vkDeviceMemory)
		{
			vkFreeMemory(vkDevice, uniformData.vkDeviceMemory, NULL);
			uniformData.vkDeviceMemory = VK_NULL_HANDLE;
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nFreed uniformData.vkDeviceMemory \n");
		}




		if (vertexData_Position.vkDeviceMemory)
		{
			vkFreeMemory(vkDevice, vertexData_Position.vkDeviceMemory, NULL);
			vertexData_Position.vkDeviceMemory = VK_NULL_HANDLE;
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nFree vertexData_Position.vkDeviceMemory freed\n");

		}

		if (vertexData_Position.vkBuffer)
		{
			vkDestroyBuffer(vkDevice, vertexData_Position.vkBuffer, NULL);
			vertexData_Position.vkBuffer = VK_NULL_HANDLE;
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nFree vertexData_Position.vkBuffer freed\n");

		}


		for (uint32_t i = 0; i < swapchainImageCount; i++)
		{
			vkFreeCommandBuffers(vkDevice, vkcommandpool, 1, &vkCommandBuffer_Array[i]);
			//vkDestroyImageView(vkDevice, swapchainImageView_array[i], NULL);
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nFree commandbuffers freed\n");
		}
		if (vkCommandBuffer_Array)
		{
			free(vkCommandBuffer_Array);
			vkCommandBuffer_Array = NULL;
		}

		vkDestroyCommandPool(vkDevice, vkcommandpool, NULL);
		//vkCommandPool = VK_NULL_HANDLE;
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "\n vkCommandPool is Freed\n");


		for (uint32_t i = 0; i < swapchainImageCount; i++)
		{
			vkDestroyImageView(vkDevice, SwapchainImageView_Array[i], NULL);
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nFree swapchainImage_array images freed\n");
		}
		if (SwapchainImageView_Array)
		{
			free(SwapchainImageView_Array);
			SwapchainImageView_Array = NULL;
		}


		/*for (uint32_t i = 0; i < swapchainImageCount; i++)
		{
			vkDestroyImage(vkDevice, SwapchainImage_Array[i], NULL);
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nFree SwapchainImage_Array images freed\n");
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
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "\n vkSwapchainKHR is Freed\n");

		}

		vkDestroyDevice(vkDevice, NULL);
		vkDevice = VK_NULL_HANDLE;
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "\n vkDestroyDevice() is Done\n");

	}
	//No need to Destroy selected physical device


	if (vkSurfaceKHR)
	{
		vkDestroySurfaceKHR(vkInstance, vkSurfaceKHR, NULL);
		vkSurfaceKHR = VK_NULL_HANDLE;
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nvkDestroySurfaceKHR Done\n");
	}

	if (vkDebugReportCallbackEXT && vkDestroyDebugReportCallbackEXT_fnptr)
	{

		vkDestroyDebugReportCallbackEXT_fnptr(vkInstance, vkDebugReportCallbackEXT, NULL);
		vkDebugReportCallbackEXT = VK_NULL_HANDLE;
		vkDestroyDebugReportCallbackEXT_fnptr = NULL;
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nvkDestroyDebugReportCallbackEXT_fnptr Done\n");

	}

	//uninitialize/destroy vulkan instance
	if (vkInstance)
	{
		vkDestroyInstance(vkInstance, NULL);
		vkInstance = VK_NULL_HANDLE;
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nvkDestroyInstance Done\n");
	}

}

//////////////////////////////////////////////////////     DEFINATION OF VULKUN RELATED FUNCTION     //////////////////////////////////////////////////////////

VkResult createVulkanInstance(void)
{
	// function declarations
	VkResult fillExtensionNames(void);

	VkResult fillValidationLayerNames(void);

	VkResult createValidationCallbackfuntion(void);

	// variable declarations
	VkResult vkresult = VK_SUCCESS;

	// code
	vkresult = fillExtensionNames();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVulkanInstance() : fillExtensionNames function failed\n");
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVulkanInstance() : fillExtensionNames() succeeded\n");
	}

	// Fill validation layers
	if (bValidation == true)
	{
		vkresult = fillValidationLayerNames();
		if (vkresult != VK_SUCCESS)
		{
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVulkanInstance() : fillValidationLayerNames() function failed (VkResult: %d)\n", vkresult);
			return vkresult;
		}
		else
		{
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVulkanInstance() : fillValidationLayerNames() succeeded\n");
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
	vkApplicationInfo.apiVersion = VK_API_VERSION_1_3; // android latest


	// step 3 : Initialize struct VkInstanceCreateInfo
	VkInstanceCreateInfo vkInstanceCreateInfo;
	memset((void*)&vkInstanceCreateInfo, 0, sizeof(VkInstanceCreateInfo));

	vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;  // Corrected the structure type
	vkInstanceCreateInfo.pNext = NULL;
	vkInstanceCreateInfo.pApplicationInfo = &vkApplicationInfo;
	vkInstanceCreateInfo.enabledExtensionCount = enabledInstanceExtensionCount;
	vkInstanceCreateInfo.ppEnabledExtensionNames = enabledInstanceExtensionNames_array;  // Corrected the field name

	if (bValidation == true)
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVulkanInstance() : vkCreateInstance() function failed due to incompatible driver (%d)\n", vkresult);
		return vkresult;
	}
	else if (vkresult == VK_ERROR_EXTENSION_NOT_PRESENT)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVulkanInstance() : vkCreateInstance() function failed due to required extension not present (%d)\n", vkresult);
		return vkresult;
	}
	else if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVulkanInstance() : vkCreateInstance() function failed due to unknown reason (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVulkanInstance() : vkCreateInstance() function succeeded\n");
	}

	// do for validationcallback
	if (bValidation == true)
	{
		vkresult = createValidationCallbackfuntion();
		if (vkresult != VK_SUCCESS)
		{
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVulkanInstance() : createValidationCallbackfuntion() function failed (VkResult: %d)\n", vkresult);
			return vkresult;
		}
		else
		{
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVulkanInstance() : createValidationCallbackfuntion() succeeded\n");
		}
	}

	// Return after completing the loop
	return vkresult;
}


VkResult fillExtensionNames(void)
{
	// variable declarations
	VkResult vkresult = VK_SUCCESS;

	// step - 1: How many instance extensions are supported by the Vulkan driver of this version
	uint32_t instanceExtensionCount = 0;

	// Query how many extensions are supported
	vkresult = vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, NULL);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillExtensionNames() : vkEnumerateInstanceExtensionProperties function 1st call failed with error code %d\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillExtensionNames() : vkEnumerateInstanceExtensionProperties() 1st call succeeded. Extension count: %d\n", instanceExtensionCount);
	}

	// step 2: Allocate memory for extensions
	VkExtensionProperties* vkExtensionProperties_array = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * instanceExtensionCount);
	if (!vkExtensionProperties_array)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillExtensionNames() : Memory allocation failed for VkExtensionProperties_array\n");
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	// Query extension properties
	vkresult = vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, vkExtensionProperties_array);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillExtensionNames() : vkEnumerateInstanceExtensionProperties function 2nd call failed with error code %d\n", vkresult);
		free(vkExtensionProperties_array);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillExtensionNames() : vkEnumerateInstanceExtensionProperties() 2nd call succeeded\n");
	}

	// step 3: Fill string array by names of extensions obtained from VkExtension array
	char** InstanceExtensionNames_array = (char**)malloc(sizeof(char*) * instanceExtensionCount);
	if (!InstanceExtensionNames_array)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillExtensionNames() : Memory allocation failed for InstanceExtensionNames_array\n");
		free(vkExtensionProperties_array);
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	for (uint32_t i = 0; i < instanceExtensionCount; i++)
	{
		InstanceExtensionNames_array[i] = (char*)malloc(sizeof(char) * strlen(vkExtensionProperties_array[i].extensionName) + 1);
		memcpy(InstanceExtensionNames_array[i], vkExtensionProperties_array[i].extensionName, strlen(vkExtensionProperties_array[i].extensionName) + 1);
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillExtensionNames() : Vulkan Extension name = %s\n", InstanceExtensionNames_array[i]);
	}

	free(vkExtensionProperties_array);

	// step 4: Check for required extensions
	VkBool32 vulkunSurfaceExtensionFound = VK_FALSE;
	VkBool32 androidSurfaceExtensionFound = VK_FALSE;
	VkBool32 debugReportExtensionFound = VK_FALSE;

	for (uint32_t i = 0; i < instanceExtensionCount; i++)
	{
		if (strcmp(InstanceExtensionNames_array[i], VK_KHR_SURFACE_EXTENSION_NAME) == 0)
		{
			vulkunSurfaceExtensionFound = VK_TRUE;
			enabledInstanceExtensionNames_array[enabledInstanceExtensionCount++] = VK_KHR_SURFACE_EXTENSION_NAME;
		}

		if (strcmp(InstanceExtensionNames_array[i], VK_KHR_ANDROID_SURFACE_EXTENSION_NAME) == 0)
		{
			androidSurfaceExtensionFound = VK_TRUE;
			enabledInstanceExtensionNames_array[enabledInstanceExtensionCount++] = VK_KHR_ANDROID_SURFACE_EXTENSION_NAME;
		}

		if (strcmp(InstanceExtensionNames_array[i], VK_EXT_DEBUG_REPORT_EXTENSION_NAME) == 0)
		{
			debugReportExtensionFound = VK_TRUE;

			if (bValidation == true)
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillExtensionNames() : VK_KHR_SURFACE_EXTENSION_NAME not found\n");
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillExtensionNames() : VK_KHR_SURFACE_EXTENSION_NAME found\n");
	}

	if (androidSurfaceExtensionFound == VK_FALSE)
	{
		vkresult = VK_ERROR_INITIALIZATION_FAILED; // return hardcoded failure
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillExtensionNames() : VK_KHR_WIN32_SURFACE_EXTENSION_NAME not found\n");
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillExtensionNames() : VK_KHR_WIN32_SURFACE_EXTENSION_NAME found\n");
	}


	if (vulkunSurfaceExtensionFound == VK_FALSE)
	{
		if (bValidation == true)
		{
			vkresult = VK_ERROR_INITIALIZATION_FAILED; // return hardcoded failure
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillExtensionNames() : validation is on but required  VK_EXT_DEBUG_REPORT_EXTENSION_NAME not found\n");
			return vkresult;
		}
		else
		{
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillExtensionNames() :  validation is off and VK_EXT_DEBUG_REPORT_EXTENSION_NAME is not suppoerted found\n");
		}
	}
	else
	{
		if (bValidation == true)
		{
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillExtensionNames() : validation is on and  VK_EXT_DEBUG_REPORT_EXTENSION_NAME is also found\n");
		}
		else
		{
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillExtensionNames() : validation is off and VK_EXT_DEBUG_REPORT_EXTENSION_NAME is suppoerted found\n");
		}

	}

	// Step 7: Print only enabled extension names
	for (uint32_t i = 0; i < enabledInstanceExtensionCount; i++)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillExtensionNames() : enabled Vulkan extension name = %s\n", enabledInstanceExtensionNames_array[i]);
	}

	// Return after completing the loop
	return vkresult;
}

VkResult fillValidationLayerNames(void)
{
	// variable declarations
	VkResult vkresult = VK_SUCCESS;

	uint32_t validationLayerCount = 0;

	// IMPORTANT FIX: reset count
	enabledValidationLayerCount = 0;

	vkresult = vkEnumerateInstanceLayerProperties(&validationLayerCount, NULL);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillValidationLayerNames() : vkEnumerateInstanceLayerProperties function 1st call failed with error code %d\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillValidationLayerNames() : vkEnumerateInstanceLayerProperties() 1st call succeeded\n");
	}

	VkLayerProperties* vkLayerProperties_Array = NULL;
	vkLayerProperties_Array = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * validationLayerCount);

	vkresult = vkEnumerateInstanceLayerProperties(&validationLayerCount, vkLayerProperties_Array);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillValidationLayerNames() : vkEnumerateInstanceLayerProperties function 2nd call failed with error code %d\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillValidationLayerNames() : vkEnumerateInstanceLayerProperties() 2nd call succeeded\n");
	}

	char** validationLayerNames_Array = NULL;
	validationLayerNames_Array = (char**)malloc(sizeof(char*) * validationLayerCount);

	for (uint32_t i = 0; i < validationLayerCount; i++)
	{
		// FIXED: correct allocation brackets
		validationLayerNames_Array[i] = (char*)malloc(sizeof(char) * (strlen(vkLayerProperties_Array[i].layerName) + 1));

		memcpy(validationLayerNames_Array[i], vkLayerProperties_Array[i].layerName, strlen(vkLayerProperties_Array[i].layerName) + 1);

		__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillValidationLayerNames() : Vulkan Layer name = %s\n", validationLayerNames_Array[i]);
	}

	free(vkLayerProperties_Array);
	vkLayerProperties_Array = NULL;

	// for required one validation layer
	VkBool32 validationLayerFound = VK_FALSE;
	for (uint32_t i = 0; i < validationLayerCount; i++)
	{
		if (strcmp(validationLayerNames_Array[i], "VK_LAYER_KHRONOS_validation") == 0)
		{
			validationLayerFound = VK_TRUE;
			enabledValidationLayerNames_Array[enabledValidationLayerCount++] = "VK_LAYER_KHRONOS_validation";
		}
	}

	for (uint32_t i = 0; i < validationLayerCount; i++)
	{
		free(validationLayerNames_Array[i]);
	}

	free(validationLayerNames_Array);
	validationLayerNames_Array = NULL;

	if (validationLayerFound == VK_FALSE)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:",
			"fillValidationLayerNames() : VK_LAYER_KHRONOS_validation not found, running without validation\n");

		// already 0, but keeping your style
		enabledValidationLayerCount = 0;

		return VK_SUCCESS;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:",
			"fillValidationLayerNames() : VK_LAYER_KHRONOS_validation found\n");
	}

	// Step 7: Print only enabled layer names
	for (uint32_t i = 0; i < enabledValidationLayerCount; i++)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "fillValidationLayerNames() : enabled Vulkan layer name = %s\n", enabledValidationLayerNames_Array[i]);
	}

	return vkresult;
}

VkResult createValidationCallbackfuntion(void)
{
	// function declaration
	VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallback(VkDebugReportFlagsEXT, VkDebugReportObjectTypeEXT, uint64_t, size_t, int32_t, const char*, const char*, void*);

	// variable declarations
	VkResult vkresult = VK_SUCCESS;

	PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT_fnptr = NULL;

	// get the required function pointers
	vkCreateDebugReportCallbackEXT_fnptr =(PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(vkInstance, "vkCreateDebugReportCallbackEXT");
	if (vkCreateDebugReportCallbackEXT_fnptr == NULL)
	{
		vkresult = VK_ERROR_INITIALIZATION_FAILED; // return hardcoded failure
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createValidationCallbackfuntion() : vkGetInstanceProcAddr() failed to get function pointer failed with error code %d \n vkCreateDebugReportCallbackEXT\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createValidationCallbackfuntion() : vkGetInstanceProcAddr() succeded to get function pointer for \n vkCreateDebugReportCallbackEXT\n");
	}

	vkDestroyDebugReportCallbackEXT_fnptr =(PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugReportCallbackEXT");
	if (vkDestroyDebugReportCallbackEXT_fnptr == NULL)
	{
		vkresult = VK_ERROR_INITIALIZATION_FAILED;

		__android_log_print(
			ANDROID_LOG_ERROR,
			"PRS",
			"createValidationCallbackFunction(): vkGetInstanceProcAddr failed to get vkDestroyDebugReportCallbackEXT, error = %d",
			vkresult
		);

		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createValidationCallbackFunction() : vkGetInstanceProcAddr() succeeded in getting function pointer for vkDestroyDebugReportCallbackEXT\n");
	}

	// get vulkan debug report callback object
	VkDebugReportCallbackCreateInfoEXT vkDebugReportCallbackCreateInfoEXT;
	memset((void*)&vkDebugReportCallbackCreateInfoEXT, 0, sizeof(VkDebugReportCallbackCreateInfoEXT));

	vkDebugReportCallbackCreateInfoEXT.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	vkDebugReportCallbackCreateInfoEXT.pNext = NULL;
	vkDebugReportCallbackCreateInfoEXT.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	vkDebugReportCallbackCreateInfoEXT.pfnCallback = debugReportCallback; 
	vkDebugReportCallbackCreateInfoEXT.pUserData = NULL; 

	vkresult = vkCreateDebugReportCallbackEXT_fnptr(vkInstance,&vkDebugReportCallbackCreateInfoEXT,NULL, &vkDebugReportCallbackEXT);

	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createValidationCallbackfuntion() : vkCreateDebugReportCallbackEXT_fnptr function failed with error code %d\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createValidationCallbackfuntion() : vkCreateDebugReportCallbackEXT_fnptr() succeeded\n");
	}


	__android_log_print(ANDROID_LOG_INFO, "PRS:", "createValidationCallbackfuntion() : ******************************************* EXITING FROM THIS FUNCTIION*********************************\n");
	return vkresult;

}

VkResult getSupportedSurface(void)
{
	VkResult vkresult = VK_SUCCESS;

	VkAndroidSurfaceCreateInfoKHR vkAndroidSurfaceCreateInfoKHR;
	memset((void*)&vkAndroidSurfaceCreateInfoKHR, 0, sizeof(VkAndroidSurfaceCreateInfoKHR));

	vkAndroidSurfaceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
	vkAndroidSurfaceCreateInfoKHR.pNext = NULL;
	vkAndroidSurfaceCreateInfoKHR.flags = 0;
	vkAndroidSurfaceCreateInfoKHR.window = androidNativeWindow;

	vkresult = vkCreateAndroidSurfaceKHR(vkInstance, &vkAndroidSurfaceCreateInfoKHR, NULL, &vkSurfaceKHR);
	if(vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getSupportedSurface() : vkCreateAndroidSurfaceKHR Failed\n");
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getSupportedSurface() : vkCreateAndroidSurfaceKHR succeded \n");
	}

	return vkresult;

}

VkResult getPhysicalDevice(void)
{
	VkResult vkresult = VK_SUCCESS;

	vkresult = vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, NULL);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevice() 1st call : vkEnumeratePhysicalDevices Failed (%d)\n", vkresult);
		return vkresult;
	}
	else if (physicalDeviceCount == 0) 
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevice() 1st call : vkEnumeratePhysicalDevices resulted in zero physical Devices \n");
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevice() 1st call : vkEnumeratePhysicalDevices succeeded \n");
	}

	vkPhysicalDevice_Array = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * physicalDeviceCount);
	if (vkPhysicalDevice_Array == NULL) 
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "Memory allocation failed for vkPhysicalDevice_Array\n");
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	vkresult = vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, vkPhysicalDevice_Array);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevice() 2nd call : vkEnumeratePhysicalDevices Failed (%d)\n", vkresult);
		free(vkPhysicalDevice_Array);  // Free on error
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevice() 2nd call  : vkEnumeratePhysicalDevices succeeded \n");
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
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "Memory allocation failed for vkQueueFamilyProperties_Array\n");
			free(vkPhysicalDevice_Array);  // Free previous allocations
			return VK_ERROR_OUT_OF_HOST_MEMORY;
		}

		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice_Array[i], &queueCount, vkQueueFamilyProperties_Array);

		VkBool32* isQueueSurface_Array = NULL;

		// Allocate memory for queue surface support
		isQueueSurface_Array = (VkBool32*)malloc(sizeof(VkBool32) * queueCount);
		if (isQueueSurface_Array == NULL)  // Check memory allocation
		{
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "Memory allocation failed for isQueueSurface_Array\n");
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevice succeeded to select the required physical device with graphics enabled\n");
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevice() Failed to find a graphics supported physical device\n");
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevice supports tessellation shader\n");
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevice does not support tessellation shader\n");
	}

	// Check for geometry shader support
	if (vkPhysicalDeviceFeatures.geometryShader)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevice supports geometry shader\n");
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevice does not support geometry shader\n");
	}

	return vkresult;
}

VkResult printVkInfo()
{
	VkResult vkresult = VK_SUCCESS;

	// Header for Vulkan Information
	__android_log_print(ANDROID_LOG_INFO, "PRS:", "********************** VULKAN INFORMATION *****************************\n");

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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "apiVersion    = %d.%d.%d\n", majorVersion, minorVersion, patchVersion);

		// DEVICE NAME
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "deviceName    = %s\n", vkPhysicalDeviceProperties.deviceName);

		// DEVICE TYPE
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "deviceType    = ");
		switch (vkPhysicalDeviceProperties.deviceType)
		{
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "integratedGPU (iGPU)\n");  // Correct spelling
			break;

		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "discreteGPU (dGPU)\n");
			break;

		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "virtualGPU (vGPU)\n");
			break;

		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "CPU\n");
			break;

		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "Other\n");
			break;

		default:
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "UNKNOWN\n");
		}

		// Vendor ID (Hexadecimal)
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "vendorID      = 0x%04x\n", vkPhysicalDeviceProperties.vendorID);

		// Device ID
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "deviceID      = 0x%04x\n", vkPhysicalDeviceProperties.deviceID);

		__android_log_print(ANDROID_LOG_INFO, "PRS:", "--------------------------------------------------------------------\n");
	}

	// Free allocated memory if necessary
	if (vkPhysicalDevice_Array)
	{
		free(vkPhysicalDevice_Array);
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "printVkInfo(): Physical Device memory freed.\n");
		vkPhysicalDevice_Array = NULL;
	}

	return vkresult;
}


VkResult fillDeviceExtensionNames(void)
{
    // Variable declarations
    VkResult vkresult = VK_SUCCESS;

    // Step 1: Query how many device extensions are supported
    uint32_t deviceExtensionCount = 0;
    vkresult = vkEnumerateDeviceExtensionProperties(vkPhysicalDevice_selected, NULL, &deviceExtensionCount, NULL);
    if (vkresult != VK_SUCCESS)
    {
        __android_log_print(ANDROID_LOG_INFO, "PRS:", "fillDeviceExtensionNames(): vkEnumerateDeviceExtensionProperties() 1st call failed with error code %d\n", vkresult);
        return vkresult;
    }
    __android_log_print(ANDROID_LOG_INFO, "PRS:", "fillDeviceExtensionNames(): vkEnumerateDeviceExtensionProperties() 1st call succeeded. Device extension count: %d\n", deviceExtensionCount);

    // Step 2: Allocate memory for extension properties
    VkExtensionProperties* vkExtensionProperties_array = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * deviceExtensionCount);
    if (!vkExtensionProperties_array)
    {
        __android_log_print(ANDROID_LOG_INFO, "PRS:", "fillDeviceExtensionNames(): Memory allocation failed for vkExtensionProperties_array\n");
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    // Query extension properties
    vkresult = vkEnumerateDeviceExtensionProperties(vkPhysicalDevice_selected, NULL, &deviceExtensionCount, vkExtensionProperties_array);
    if (vkresult != VK_SUCCESS)
    {
        __android_log_print(ANDROID_LOG_INFO, "PRS:", "fillDeviceExtensionNames(): vkEnumerateDeviceExtensionProperties() 2nd call failed with error code %d\n", vkresult);
        free(vkExtensionProperties_array);
        return vkresult;
    }
    __android_log_print(ANDROID_LOG_INFO, "PRS:", "fillDeviceExtensionNames(): vkEnumerateDeviceExtensionProperties() 2nd call succeeded\n");

    // Step 3: Allocate memory for extension name strings
    char** DeviceExtensionNames_array = (char**)malloc(sizeof(char*) * deviceExtensionCount);
    if (!DeviceExtensionNames_array)
    {
        __android_log_print(ANDROID_LOG_INFO, "PRS:", "fillDeviceExtensionNames(): Memory allocation failed for DeviceExtensionNames_array\n");
        free(vkExtensionProperties_array);
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    for (uint32_t i = 0; i < deviceExtensionCount; i++)
    {
        DeviceExtensionNames_array[i] = (char*)malloc(strlen(vkExtensionProperties_array[i].extensionName) + 1);
        if (!DeviceExtensionNames_array[i])
        {
            __android_log_print(ANDROID_LOG_INFO, "PRS:", "fillDeviceExtensionNames(): Memory allocation failed for DeviceExtensionNames_array[%d]\n", i);
            // Free previously allocated memory before returning
            for (uint32_t j = 0; j < i; j++)
                free(DeviceExtensionNames_array[j]);
            free(DeviceExtensionNames_array);
            free(vkExtensionProperties_array);
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        }

        strcpy(DeviceExtensionNames_array[i], vkExtensionProperties_array[i].extensionName);
        __android_log_print(ANDROID_LOG_INFO, "PRS:", "fillDeviceExtensionNames(): Vulkan Device Extension = %s\n", DeviceExtensionNames_array[i]);
    }

    free(vkExtensionProperties_array); // Free extension properties after copying names

    // Step 4: Check for required extensions
    VkBool32 vulkanSwapchainExtensionFound = VK_FALSE;

    for (uint32_t i = 0; i < deviceExtensionCount; i++)
    {
        if (strcmp(DeviceExtensionNames_array[i], VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
        {
            vulkanSwapchainExtensionFound = VK_TRUE;
            enabledDeviceExtensionNames_array[enableDeviceExtensionCount++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
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
        __android_log_print(ANDROID_LOG_INFO, "PRS:", "fillDeviceExtensionNames(): VK_KHR_SWAPCHAIN_EXTENSION_NAME not found\n");
        return vkresult;
    }
    __android_log_print(ANDROID_LOG_INFO, "PRS:", "fillDeviceExtensionNames(): VK_KHR_SWAPCHAIN_EXTENSION_NAME found\n");

    // Step 7: Print enabled extension names
    for (uint32_t i = 0; i < enableDeviceExtensionCount; i++)
    {
        __android_log_print(ANDROID_LOG_INFO, "PRS:", "fillDeviceExtensionNames(): Enabled Vulkan Device Extension = %s\n", enabledDeviceExtensionNames_array[i]);
    }

    return vkresult;
}

VkResult createVulkanDevice(void)
{
	// function declaration
	VkResult fillDeviceExtensionNames(void);


	// variable declaration
	VkResult vkresult = VK_SUCCESS;

	// fill device extension names
	vkresult = fillDeviceExtensionNames();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVulkanDevice() : fillDeviceExtensionNames function failed\n");
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVulkanDevice() : fillDeviceExtensionNames() succeeded\n");
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVulkanDevice() : vkCreateDevice function failed\n");
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVulkanDevice() : vkCreateDevice() succeeded\n");
	}

	return vkresult;
}


void getDeviceQueue(void)
{
	vkGetDeviceQueue(vkDevice, graphicsQueueFamilyIndex_Selected, 0, &vkQueue);

	if (vkQueue == VK_NULL_HANDLE)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getDeviceQueue() : vkGetDeviceQueue returned NULL for vkQueue. Check if the device was created with the correct queue family.\n");
		return;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getDeviceQueue() : vkGetDeviceQueue() succeeded\n");
	}
}

VkResult getPhysicalDeviceSurfaceFormatAndColorSpace(void)
{
	// Variable declaration
	VkResult vkresult = VK_SUCCESS;
	uint32_t formatCount = 0;

	// First call to get the number of supported surface formats
	vkresult = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice_selected, vkSurfaceKHR, &formatCount, NULL);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDeviceSurfaceFormatAndColorSpace(): vkGetPhysicalDeviceSurfaceFormatsKHR() failed\n");
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDeviceSurfaceFormatAndColorSpace() : vkGetPhysicalDeviceSurfaceFormatsKHR() succeeded\n");
	}

	// Check if no surface formats are found
	if (formatCount == 0)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDeviceSurfaceFormatAndColorSpace(): No surface formats found\n");
		return VK_ERROR_INITIALIZATION_FAILED; // Hardcoded failure
	}

	__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDeviceSurfaceFormatAndColorSpace(): Found %u surface formats\n", formatCount);

	// Allocate memory for surface format array
	VkSurfaceFormatKHR* vkSurfaceFormatKHR_Array = (VkSurfaceFormatKHR*)malloc(formatCount * sizeof(VkSurfaceFormatKHR));

	// Second call to fetch the actual supported surface formats
	vkresult = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice_selected, vkSurfaceKHR, &formatCount, vkSurfaceFormatKHR_Array);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDeviceSurfaceFormatAndColorSpace(): Second call to vkGetPhysicalDeviceSurfaceFormatsKHR() failed\n");
		free(vkSurfaceFormatKHR_Array);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDeviceSurfaceFormatAndColorSpace() Second call: vkGetPhysicalDeviceSurfaceFormatsKHR succeeded\n");
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDeviceSurfaceFormatAndColorSpace(): vkSurfaceFormatKHR_Array Memory freed\n");
	}

	return vkresult;
}


VkResult getPhysicalDevicePresentMode(void)
{
	// Variable declaration
	VkResult vkresult = VK_SUCCESS;
	uint32_t presentModeCount = 0;

	// First call to get the number of supported present modes
	vkresult = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice_selected, vkSurfaceKHR, &presentModeCount, NULL);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevicePresentMode() 1st call: vkGetPhysicalDeviceSurfacePresentModesKHR() failed\n");
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevicePresentMode() 1st call : vkGetPhysicalDeviceSurfacePresentModesKHR() succeeded\n");
	}

	// Check if no present modes are found
	if (presentModeCount == 0)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevicePresentMode(): No present modes found\n");
		return VK_ERROR_INITIALIZATION_FAILED; // Hardcoded failure
	}

	__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevicePresentMode(): Found %u present modes\n", presentModeCount);

	// Allocate memory for present mode array
	VkPresentModeKHR* vkPresentModeKHR_Array = (VkPresentModeKHR*)malloc(presentModeCount * sizeof(VkPresentModeKHR));
	if (!vkPresentModeKHR_Array)  // Ensure memory allocation was successful
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevicePresentMode(): Memory allocation failed\n");
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	// Second call to fetch the actual supported present modes
	vkresult = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice_selected, vkSurfaceKHR, &presentModeCount, vkPresentModeKHR_Array);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevicePresentMode() 2nd call: vkGetPhysicalDeviceSurfacePresentModesKHR() failed\n");
		free(vkPresentModeKHR_Array);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevicePresentMode() 2nd call: vkGetPhysicalDeviceSurfacePresentModesKHR() succeeded\n");
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevicePresentMode(): Selected Present Mode = MAILBOX_KHR\n");
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevicePresentMode(): Selected Present Mode = FIFO_KHR\n");
	}

	// Free allocated memory
	if (vkPresentModeKHR_Array)
	{
		free(vkPresentModeKHR_Array);
		vkPresentModeKHR_Array = NULL;
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "getPhysicalDevicePresentMode(): Memory freed, function completed successfully\n");
	}

	return vkresult;
}

VkResult createSwapchain(VkBool32 vsync)
{
	// function declarations
	VkResult getPhysicalDeviceSurfaceFormatAndColorSpace(void);
	VkResult getPhysicalDevicePresentMode(void);

	// variable declaration
	VkResult vkresult = VK_SUCCESS;

	// code
	vkresult = getPhysicalDeviceSurfaceFormatAndColorSpace();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createSwapchain() : getPhysicalDeviceSurfaceFormatAndColorSpace() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createSwapchain() : getPhysicalDeviceSurfaceFormatAndColorSpace() succeeded\n");
	}

	// step 2 : get physical device surface capabilities
	VkSurfaceCapabilitiesKHR vkSurfaceCapabilitiesKHR;
	memset((void*)&vkSurfaceCapabilitiesKHR, 0, sizeof(VkSurfaceCapabilitiesKHR));

	vkresult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice_selected, vkSurfaceKHR, &vkSurfaceCapabilitiesKHR);

	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createSwapchain() : vkGetPhysicalDeviceSurfaceCapabilitiesKHR() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createSwapchain() : vkGetPhysicalDeviceSurfaceCapabilitiesKHR() succeeded\n");
	}

	// step 3 : find out desired number of swapchain images
	uint32_t testingNoofSwapchainImages = vkSurfaceCapabilitiesKHR.minImageCount + 1;
	uint32_t desiredNoofSwapchainImages = 0;

	if (vkSurfaceCapabilitiesKHR.maxImageCount > 0 && vkSurfaceCapabilitiesKHR.maxImageCount < testingNoofSwapchainImages)
	{
		desiredNoofSwapchainImages = vkSurfaceCapabilitiesKHR.maxImageCount;
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nmaxImageCount\n");
	}
	else
	{
		desiredNoofSwapchainImages = vkSurfaceCapabilitiesKHR.minImageCount;
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nminImageCount\n");
	}

	__android_log_print(ANDROID_LOG_INFO, "PRS:", "\n\n\n%d %d %d !!!\n", vkSurfaceCapabilitiesKHR.maxImageCount, vkSurfaceCapabilitiesKHR.minImageCount, desiredNoofSwapchainImages);



	// step 4: choose size of the swapchain image
	memset((void*)&vkExtent2D_Swapchain, 0, sizeof(vkExtent2D_Swapchain));

	if (vkSurfaceCapabilitiesKHR.currentExtent.width != UINT32_MAX)
	{
		vkExtent2D_Swapchain.width = vkSurfaceCapabilitiesKHR.currentExtent.width;
		vkExtent2D_Swapchain.height = vkSurfaceCapabilitiesKHR.currentExtent.height;

		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createSwapchain() : swapchain image width = %d height = %d\n", vkExtent2D_Swapchain.width, vkExtent2D_Swapchain.height);
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

		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createSwapchain() : swapchain image width = %d height = %d\n", vkExtent2D_Swapchain.width, vkExtent2D_Swapchain.height);
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
	vkresult = getPhysicalDevicePresentMode();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createSwapchain() : getPhysicalDevicePresentMode() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createSwapchain() : getPhysicalDevicePresentMode() succeeded\n");
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
	vkSwapchainCreateInfoKHR.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
	vkSwapchainCreateInfoKHR.presentMode = vkPresentModeKHR;
	vkSwapchainCreateInfoKHR.clipped = VK_TRUE;

	// step 9 : call the function
	vkresult = vkCreateSwapchainKHR(vkDevice, &vkSwapchainCreateInfoKHR, NULL, &vkSwapchainKHR);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createSwapchain() : vkCreateSwapchainKHR() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createSwapchain() : vkCreateSwapchainKHR() succeeded\n");
	}

	return vkresult;
}

VkResult createImagesAndImageViews(void)
{
	// variable declaration
	VkResult vkresult = VK_SUCCESS;

	// get swapchain image count
	vkresult = vkGetSwapchainImagesKHR(vkDevice, vkSwapchainKHR, &swapchainImageCount, NULL);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createImagesAndImageViews() 1st call : vkGetSwapchainImagesKHR() function failed (%d)\n", vkresult);
		return vkresult;
	}

	else if (swapchainImageCount == 0)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createImagesAndImageViews() 1st call : swapchain image count is zero, returning hardcoded error value\n");
		return VK_ERROR_INITIALIZATION_FAILED;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createImagesAndImageViews() 1st call : this func is giving the swapchain image count = %d\n", swapchainImageCount);
	}

	//// allocate the swapchain image array
	SwapchainImage_Array = (VkImage*)malloc(sizeof(VkImage) * swapchainImageCount);

	//// fill this array with swapchain images
	vkresult = vkGetSwapchainImagesKHR(vkDevice, vkSwapchainKHR, &swapchainImageCount, SwapchainImage_Array);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createImagesAndImageViews() 2nd call : vkGetSwapchainImagesKHR() function failed (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createImagesAndImageViews() 2nd call : vkGetSwapchainImagesKHR() succeeded\n");
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
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "createImagesAndImageViews() : vkCreateImageView() function failed for iteration (%d).(%d)\n", i, vkresult);
			return vkresult;
		}
		else
		{
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "createImagesAndImageViews() : vkCreateImageView() succeeded for iteration (%d)\n", i);
		}
	}

	return vkresult;
}

VkResult createCommandPool(void)
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createCommandPool() : vkCreateCommandPool() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createCommandPool() : vkCreateCommandPool() succeeded.\n");
	}

	return vkresult;
}


VkResult createCommandBuffers(void)
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createCommandBuffers() : vkAllocateCommandBuffers() function failed. Error Code: (%d)\n", vkresult);
		free(vkCommandBuffer_Array);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createCommandBuffers() : vkAllocateCommandBuffers() succeeded.\n");
	}

	return vkresult;
}

VkResult createVertexBuffer(void)
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVertexBuffer() : vkCreateBuffer() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVertexBuffer() : vkCreateBuffer() succeeded.\n");
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

	vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, NULL, &vertexData_Position.vkDeviceMemory);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVertexBuffer() : vkAllocateMemory() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVertexBuffer() : vkAllocateMemory() succeeded.\n");
	}

	vkresult = vkBindBufferMemory(vkDevice, vertexData_Position.vkBuffer, vertexData_Position.vkDeviceMemory, 0);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVertexBuffer() : vkBindBufferMemory() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVertexBuffer() : vkBindBufferMemory() succeeded.\n");
	}

	void* data = NULL;

	vkresult = vkMapMemory(vkDevice, vertexData_Position.vkDeviceMemory, 0, vkMemoryAllocateInfo.allocationSize, 0, &data);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVertexBuffer() : vkMapMemory() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createVertexBuffer() : vkMapMemory() succeeded.\n");
	}

	// actual memory mapped

	memcpy(data, traingle_Position, sizeof(traingle_Position));

	vkUnmapMemory(vkDevice, vertexData_Position.vkDeviceMemory);


	return vkresult;

}

VkResult createUniformBuffer(void)
{
	// funcition declarations
	VkResult updateUniformbuffer(void);

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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createUniformBuffer() : vkCreateBuffer() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createUniformBuffer() : vkCreateBuffer() succeeded.\n");
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

	vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, NULL, &uniformData.vkDeviceMemory);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createUniformBuffer() : vkAllocateMemory() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createUniformBuffer() : vkAllocateMemory() succeeded.\n");
	}

	vkresult = vkBindBufferMemory(vkDevice, uniformData.vkBuffer, uniformData.vkDeviceMemory, 0);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createUniformBuffer() : vkBindBufferMemory() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createUniformBuffer() : vkBindBufferMemory() succeeded.\n");
	}

	// call updateUnifomBuffer
	vkresult = updateUniformbuffer();
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createUniformBuffer() : updateUniformbuffer() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createUniformBuffer() : updateUniformbuffer() succeeded.\n");
	}


	return vkresult;
}

VkResult updateUniformbuffer(void)
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "updateUniformbuffer() : vkMapMemory() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}

	// actual memory mapped
	memcpy(data, &myUniformData, sizeof(myUniformData));

	vkUnmapMemory(vkDevice, uniformData.vkDeviceMemory);

	return vkresult;
}


VkResult createShaders(void)
{
	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	//code

	// for vertex shaders
	const char* szfileName = "Shader.vert.spv";

	size_t size;

	AAsset* asset = NULL;

	asset = AAssetManager_open(androidAssetManager, szfileName, AASSET_MODE_STREAMING);

	if(asset == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createShaders() : createShader failed to open vertexshader.spv file\n");
		vkresult = VK_ERROR_INITIALIZATION_FAILED;
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createShaders() : createShader succeeded to open vertexshader.spv file\n");
	}

	size = AAsset_getLength(asset);

	if (size == 0)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createShaders() : createShader failed and give file size of vertex shader 0\n");
		vkresult = VK_ERROR_INITIALIZATION_FAILED;
		return vkresult;
	}

	char* shaderData = (char*)malloc(sizeof(char) * size);

	int retVal = AAsset_read(asset, shaderData, size);

	if(retVal != size)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createShaders() : createShader failed to read vertexshader.spv file\n");
		vkresult = VK_ERROR_INITIALIZATION_FAILED;
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createShaders() : createShader succeeded to read vertexshader.spv file\n");
	}

	AAsset_close(asset);

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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createShaders() : vkCreateShaderModule() function failed.for vertex shader Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createShaders() : vkCreateShaderModule() succeeded fro vertex shader.\n");
	}


	if (shaderData)
	{
		free(shaderData);
		shaderData = NULL;
	}

	__android_log_print(ANDROID_LOG_INFO, "PRS:", "Vertexshader Module sucessfully created\n");

	// for fragmnt shader

	szfileName = "Shader.frag.spv";
	
	asset = NULL;

	asset = AAssetManager_open(androidAssetManager, szfileName, AASSET_MODE_STREAMING);

	if(asset == NULL)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createShaders() : createShader failed to open fragmentShader.spv file\n");
		vkresult = VK_ERROR_INITIALIZATION_FAILED;
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createShaders() : createShader succeeded to open fragmentShader.spv file\n");
	}

	size = AAsset_getLength(asset);

	if (size == 0)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createShaders() : createShader failed and give file size of fagment shader 0\n");
		vkresult = VK_ERROR_INITIALIZATION_FAILED;
		return vkresult;
	}

	shaderData = (char*)malloc(sizeof(char) * size);

	retVal = AAsset_read(asset, shaderData, size);

	if(retVal != size)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createShaders() : createShader failed to read fragmentShader.spv file\n");
		vkresult = VK_ERROR_INITIALIZATION_FAILED;
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createShaders() : createShader succeeded to read fragmentShader.spv file\n");
	}

	AAsset_close(asset);

	memset((void*)&vkShaderModuleCreateInfo, 0, sizeof(VkShaderModuleCreateInfo));

	vkShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vkShaderModuleCreateInfo.pNext = NULL;
	vkShaderModuleCreateInfo.flags = 0; // future use can be haapend but now zero
	vkShaderModuleCreateInfo.codeSize = size;
	vkShaderModuleCreateInfo.pCode = (uint32_t*)shaderData;

	vkresult = vkCreateShaderModule(vkDevice, &vkShaderModuleCreateInfo, NULL, &vkShaderModule_fragment_shader);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createShaders() : vkCreateShaderModule() function failed. for fragment shader Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createShaders() : vkCreateShaderModule() succeeded for fragment shader.\n");
	}


	if (shaderData)
	{
		free(shaderData);
		shaderData = NULL;
	}

	__android_log_print(ANDROID_LOG_INFO, "PRS:", "fragment Module sucessfully created\n");

	return vkresult;
}

VkResult createDiscriptorSetLayout(void)
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

;	vkDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createDescriptorSetLayout() : vkCreateDescriptorSetLayout() failed Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createDescriptorSetLayout() : vkCreateDescriptorSetLayout() succeeded.\n");
	}



	return vkresult;

}

VkResult createPiplineLayout(void)
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createPiplineLayout() : vkCreatePipelineLayout() failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createPiplineLayout() : vkCreatePipelineLayout() succeeded.\n");
	}

	return vkresult;
}

VkResult createDescriptorpool(void)
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createDescriptorpool() : vkCreateDescriptorPool() failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createDescriptorpool() : vkCreateDescriptorPool() succeeded.\n");
	}


	return vkresult;
}

VkResult createDescriptorSet(void)
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createDescriptorSet() : vkCreateDescriptorPool() failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createDescriptorSet() : vkCreateDescriptorPool() succeeded.\n");
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

	__android_log_print(ANDROID_LOG_INFO, "PRS:", "\nvkUpdateDescriptorSets() succeeded.\n");


	return vkresult;
}

VkResult createRenderPass(void)
{
	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	VkAttachmentDescription vkAttachmentDescription_array[1];

	memset((void*)vkAttachmentDescription_array, 0, sizeof(VkAttachmentDescription) * _ARRAYSIZE(vkAttachmentDescription_array));
	vkAttachmentDescription_array[0].flags = 0;
	vkAttachmentDescription_array[0].format = vkFormat_color;
	vkAttachmentDescription_array[0].samples = VK_SAMPLE_COUNT_1_BIT;   //No multi sampling so 1 bit is enough
	vkAttachmentDescription_array[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	vkAttachmentDescription_array[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;				//color attachment related
	vkAttachmentDescription_array[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;			//This is for both Depth and Stencil although it is for stencil
	vkAttachmentDescription_array[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	vkAttachmentDescription_array[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;													//Image data when in and when out
	vkAttachmentDescription_array[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;




	//Declare and initialize vkAttachmentReference structure
	VkAttachmentReference vkAttachmentRederence;
	memset((void*)&vkAttachmentRederence, 0, sizeof(VkAttachmentReference));
	vkAttachmentRederence.attachment = 0;			//This means above given array 0th Ataachment reference, O means it is the index number
	vkAttachmentRederence.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;		//Tis means this attachment i can use it color attachment so keep it optimal


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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createRenderPass() : vkCreateRenderPass() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createRenderPass() : vkCreateRenderPass() succeeded.\n");
	}

	return vkresult;
}

VkResult createPipline(void)
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createPipline() : vkCreatePipelineCache() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createPipline() : vkCreatePipelineCache() succeeded.\n");
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createPipeline() : vkCreateGraphicsPipelines() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createPipeline() : vkCreateGraphicsPipelines() succeeded.\n");
	}

	// we have done with pipline cache so destroy it

	if (vkPipelineCache != VK_NULL_HANDLE)
	{
		vkDestroyPipelineCache(vkDevice, vkPipelineCache, NULL);
		vkPipelineCache = VK_NULL_HANDLE;
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createPipeline() : Free vkPipelineCache freed\n");
	}

	return vkresult;

}


VkResult createframeBuffers(void)
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
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "createframeBuffers() : vkCreateFramebuffer() function failed. Error Code: (%d)\n", vkresult);
			return vkresult;
		}
		else
		{
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "createframeBuffers() : vkCreateFramebuffer() succeeded.\n");
		}
	}

	return vkresult;
}

VkResult createSemaphores(void)
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
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createSemaphores() : vkCreateSemaphore() function failed for backbuffer. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createSemaphores() : vkCreateSemaphore() succeeded for backbuffer.\n");
	}

	vkresult = vkCreateSemaphore(vkDevice, &vkSemaphoreCreateInfo, NULL, &vkSemaphore_rendercomplete);
	if (vkresult != VK_SUCCESS)
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createSemaphores() : vkCreateSemaphore() function failed for rendercomplete. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		__android_log_print(ANDROID_LOG_INFO, "PRS:", "createSemaphores() : vkCreateSemaphore() succeeded for rendercomplete.\n");
	}

	return vkresult;
}



VkResult createFences(void)
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
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "createFences() : vkCreateFence() function failed. Error Code: (%d)\n", vkresult);
			return vkresult;
		}
		else
		{
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "createFences() : vkCreateFence() succeeded.\n");
		}
	}

	return vkresult;
}

VkResult buildCommandBuffers(void)
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
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "buildCommandBuffers() : vkResetCommandBuffer() failed at index [%d]. Error Code: (%d)\n", i, vkresult);
			return vkresult;
		}
		else
		{
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "buildCommandBuffers() : vkResetCommandBuffer() succeeded at index [%d].\n", i);
		}

		VkCommandBufferBeginInfo vkCommandBufferBeginInfo;
		memset((void*)&vkCommandBufferBeginInfo, 0, sizeof(VkCommandBufferBeginInfo));

		vkCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkCommandBufferBeginInfo.pNext = NULL;
		vkCommandBufferBeginInfo.flags = 0;

		vkresult = vkBeginCommandBuffer(vkCommandBuffer_Array[i], &vkCommandBufferBeginInfo);
		if (vkresult != VK_SUCCESS)
		{
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "buildCommandBuffers() : VkBeginCommandBuffer() failed at index [%d]. Error Code: (%d)\n", i, vkresult);
			return vkresult;
		}
		else
		{
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "buildCommandBuffers() : VkBeginCommandBuffer() succeeded at index [%d].\n", i);
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


		// bind with vertex buffer
		VkDeviceSize vkDeviceSize_Offest_Array[1];
		memset((void*)vkDeviceSize_Offest_Array, 0, sizeof(VkDeviceSize) * _ARRAYSIZE(vkDeviceSize_Offest_Array));

		vkCmdBindVertexBuffers(vkCommandBuffer_Array[i], 0, 1, &vertexData_Position.vkBuffer, vkDeviceSize_Offest_Array);

		// Here we should call Vulkan drawing functions
		vkCmdDraw(vkCommandBuffer_Array[i], 3, 1, 0, 0);

		// End the render pass
		vkCmdEndRenderPass(vkCommandBuffer_Array[i]);

		// End command buffer recording
		vkresult = vkEndCommandBuffer(vkCommandBuffer_Array[i]);
		if (vkresult != VK_SUCCESS)
		{
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "buildCommandBuffers() : vkEndCommandBuffer() failed at index [%d]. Error Code: (%d)\n", i, vkresult);
			return vkresult;
		}
		else
		{
			__android_log_print(ANDROID_LOG_INFO, "PRS:", "buildCommandBuffers() : vkEndCommandBuffer() succeeded at index [%d].\n", i);
		}

	}

	return vkresult;

}
VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallback(
	VkDebugReportFlagsEXT vkDebugReportFlagsEXT,
	VkDebugReportObjectTypeEXT vkDebugReportObjectTypeEXT,
	uint64_t object,
	size_t location,
	int32_t messageCode,
	const char* pLayerPrefix,
	const char* pMessage,
	void* pUserData
)
{
	//code
	__android_log_print(ANDROID_LOG_INFO, "PRS:", "PRS_Validation : debugReportCallback() :  %s (%d) = %s\n", pLayerPrefix, messageCode, pMessage);

	// Returning VK_FALSE means continue execution (don't abort)
	return VK_FALSE;
}





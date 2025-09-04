// windows header files
#include<windows.h>
#include"VK.h"
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h" // header file for texture
#include<stdio.h> // for file IO
#include<stdlib.h> // for exit

#define VK_USE_PLATFORM_WIN32_KHR  // tells in which platform you are
#include<vulkan/vulkan.h>

// glm related macros and header files
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TWO_ONE
#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"

// vulkun related libraries
#pragma comment(lib, "vulkan-1.lib")



// Macros
#define Win_WIDTH 800
#define Win_HEIGHT 600


//global function declaration
LRESULT CALLBACK Wndproc(HWND, UINT, WPARAM, LPARAM);

const char* gpszAppName = "ARTR";

// global variable declaration
FILE* gpFile = NULL; 


HWND ghwnd = NULL;
BOOL gbActive = FALSE;
DWORD dwstyle = 0;
WINDOWPLACEMENT wprev;
BOOL gbFullscreen = FALSE;
BOOL bWindowMinimized = FALSE;


// vulkun related global variables

uint32_t enabledInstanceExtensionCount = 0;


const char* enabledInstanceExtensionNames_array[3];  // VK_KHR_SURFACE_EXTENSION_NAME and VK_KHR_WIN32_SURFACE_EXTENSION_NAME and VK_EXT_DEBUG_REPORT_EXTENSION_NAME


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

int winWidth = Win_WIDTH;
int winHeight = Win_HEIGHT;

//swap chain related global variables
VkSwapchainKHR vkSwapchainKHR = VK_NULL_HANDLE;
VkExtent2D vkExtent2D_Swapchain;


// swapchain images and swapchain image views relrated data

//for color
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

BOOL bInitialised = FALSE;

uint32_t currentImageIndex = UINT32_MAX;

// Validation
BOOL bValidation = TRUE;

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
VertexData vertexData_Position_Pyramid;

VertexData vertexData_Position_Cube;

VertexData vertexData_Texcoord_Pyramid;

VertexData vertexData_Texcoord_Cube;

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

UniformData uniformData_Pyramid;

UniformData uniformData_Cube;

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
VkDescriptorSet vkDescriptorSet_Pyramid = VK_NULL_HANDLE;

VkDescriptorSet vkDescriptorSet_Cube = VK_NULL_HANDLE;

// For Rotation
float angle_1 = 0.0f;

float angle_2 = 0.0f;

// texture related variables
VkImage vkImage_Texture = VK_NULL_HANDLE;

VkDeviceMemory vkDeviceMemory_Texture = VK_NULL_HANDLE;

VkImageView vkImageView_Texture = VK_NULL_HANDLE;

VkSampler vkSampler_Texture = VK_NULL_HANDLE;

VkImageView vkImageView_Texture_Pyramid = VK_NULL_HANDLE;
VkImageView vkImageView_Texture_Cube = VK_NULL_HANDLE;

VkSampler vkSampler_Texture_Pyramid = VK_NULL_HANDLE;
VkSampler vkSampler_Texture_Cube = VK_NULL_HANDLE;

//entry_point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstace, LPSTR lpszCmdLine, int iCmdShow)
{
	// Function Declaration 
	VkResult initialise(void);
	void uninitialise(void);
	VkResult display(void);
	void update(void);

	// local variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[255];
	int iResult = 0;
	BOOL bDone = FALSE;

	VkResult vkresult = VK_SUCCESS;

	//int height = 600;
	//int width = 800;
	int window_x_coordinate = GetSystemMetrics(SM_CXSCREEN);
	int window_y_coordinate = GetSystemMetrics(SM_CYSCREEN);
	int y;
	int x;
	x = (window_x_coordinate / 2) - Win_WIDTH / 2;
	y = (window_y_coordinate / 2) - Win_HEIGHT / 2;

	//code
	gpFile = fopen("Log.txt", "w");
	if (gpFile == NULL)
	{
		MessageBox(NULL, TEXT("log file cannot be open"), TEXT("Eroor"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	fprintf(gpFile, "programm started successfully...\n");

	wsprintf(szAppName, TEXT("%s"), gpszAppName);

	//WndclassEX initialisation

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = Wndproc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	//register wndclassEX

	RegisterClassEx(&wndclass);


	// create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, 
		szAppName,
		TEXT("PRS : Vulkun"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,


		x,// upperleft x-coordinate
		y,// upperleft y-coordinate
		Win_WIDTH,// width
		Win_HEIGHT,// height

		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	// INITIALISATION

	vkresult = initialise();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "WinMain() : Initialise functioon failed\n");
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
	else
	{
		fprintf(gpFile, "Winmain() : Intialise() succeeded\n");
	}

	// show the window
	ShowWindow(hwnd, iCmdShow);

	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	

	// gameloop
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = TRUE;

			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

			}

		}

		else
		{
			if (gbActive == TRUE)
			{
				if (bWindowMinimized == FALSE)
				{
					// RENDER
					vkresult =  display();
					if (vkresult != VK_FALSE && vkresult != VK_SUCCESS && vkresult != VK_ERROR_OUT_OF_DATE_KHR && vkresult != VK_SUBOPTIMAL_KHR)
					{
						fprintf(gpFile, "Initialisation() :  call to display failed\n");
						bDone = TRUE;
					}

					// update
					if (gbActive == TRUE)
					{
						update();
					}
				}
			}

		}
	}

	// UNINITIALISATION
	uninitialise();

	return((int)msg.wParam);
}


//callback function
LRESULT CALLBACK Wndproc(HWND hwnd , UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// function declaration
	void ToggleFullscreen(void);
	VkResult resize(int, int);

	// code
	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActive = TRUE;
		break;
	case WM_KILLFOCUS:
		gbActive = FALSE;
		break;
	case WM_CREATE:
		memset(&wprev, 0, sizeof(WINDOWPLACEMENT));
		wprev.length = sizeof(WINDOWPLACEMENT);
		break;
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			bWindowMinimized = TRUE;
		}
		else
		{
			resize(LOWORD(lParam), HIWORD(lParam));
			bWindowMinimized = FALSE;
		}
		break;
	case WM_ERASEBKGND:
		return(0);
	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		}
		break;


	case WM_CHAR:
		switch (LOWORD(wParam))
		{
		case 'F':
		case 'f':
			if (gbFullscreen == FALSE)
			{
				ToggleFullscreen();
				gbFullscreen = TRUE;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = FALSE;
			}
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;

	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));

}

void ToggleFullscreen(void)
{
	// local variable decalration
	MONITORINFO mi = { sizeof(MONITORINFO) };

	//code
	if (gbFullscreen == FALSE)
	{
		dwstyle = GetWindowLong(ghwnd, GWL_STYLE);

		if (dwstyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wprev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwstyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowPlacement(ghwnd, &wprev);
		SetWindowLong(ghwnd, GWL_STYLE, dwstyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);

	}

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
	VkResult createTexture(const char*);
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

	vkresult = createTexture("Stone.png");
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createTexture() function failed stone (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createTexture() succeeded for stone \n");
		fflush(gpFile);
	}

	vkImageView_Texture_Pyramid = vkImageView_Texture;
	vkSampler_Texture_Pyramid = vkSampler_Texture;

	vkresult = createTexture("Vijay_Kundali.png");
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createTexture() function failed stone (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createTexture() succeeded for stone \n");
		fflush(gpFile);
	}

	vkImageView_Texture_Cube = vkImageView_Texture;
	vkSampler_Texture_Cube = vkSampler_Texture;
	
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
	vkClearColorValue.float32[2] = 0.0f;
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

	}

	// destroy commandbuffer
	for (uint32_t i = 0; i < swapchainImageCount; i++)
	{
		vkFreeCommandBuffers(vkDevice, vkcommandpool, 1, &vkCommandBuffer_Array[i]);
		//vkDestroyImageView(vkDevice, swapchainImageView_array[i], NULL);
	}

	if (vkCommandBuffer_Array)
	{
		free(vkCommandBuffer_Array);
		vkCommandBuffer_Array = NULL;
	}

	// destroy pipline
	if (vkPipeline)
	{
		vkDestroyPipeline(vkDevice, vkPipeline, NULL);
		vkPipeline = VK_NULL_HANDLE;
	}

	// destroy pipline layout
	if (vkPipelineLayout)
	{
		vkDestroyPipelineLayout(vkDevice, vkPipelineLayout, NULL);
		vkPipelineLayout = VK_NULL_HANDLE;
	}

	// destroy render pass
	if (vkRenderpass)
	{
		vkDestroyRenderPass(vkDevice, vkRenderpass, NULL);
		vkRenderpass = VK_NULL_HANDLE;
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

VkResult display(void)
{

	// function declaration
	VkResult resize(int, int);
	VkResult updateUniformbuffer(void);
	 
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
	if (vkresult != VK_SUCCESS)
	{
		if (vkresult == VK_ERROR_OUT_OF_DATE_KHR || vkresult == VK_SUBOPTIMAL_KHR)
		{
			resize(winWidth, winHeight);
		}
		else
		{
			fprintf(gpFile, "display() : vkAcquireNextImageKHR failed with error: %d\n", vkresult);
			return(vkresult);
		}
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
		if (vkresult == VK_ERROR_OUT_OF_DATE_KHR || vkresult == VK_SUBOPTIMAL_KHR)
		{
			resize(winWidth, winHeight);
		}
		else
		{
			fprintf(gpFile, "display() : vkQueuePresentKHR failed with error: %d\n", vkresult);
			return(vkresult);
		}
	}

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

void update(void)
{
	// code
	// code
	angle_1 = angle_1 + 1.5f;
	if (angle_1 >= 360.0f)
	{
		angle_1 = angle_1 - 360.0f;
	}

	angle_2 = angle_2 + 1.5f;
	if (angle_2 >= 360.0f)
	{
		angle_2 = angle_2 - 360.0f;
	}
}


void uninitialise(void)
{
	// Function declarations
	void ToggleFullscreen(void);

	// Code
	if (gbFullscreen == TRUE)
	{
		ToggleFullscreen();
	}

	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
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
			vkDescriptorSet_Cube = VK_NULL_HANDLE;
			vkDescriptorSet_Pyramid = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFreed vkDescriptorPool and vkDescriptorSet for trangle and rectangle\n");
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

		// Destroy uniform buffer for triangle
		if (uniformData_Cube.vkBuffer)
		{
			vkDestroyBuffer(vkDevice, uniformData_Cube.vkBuffer, NULL);
			uniformData_Cube.vkBuffer = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFreed uniformData_Cube.vkBuffer \n");
		}

		if (uniformData_Cube.vkDeviceMemory)
		{
			vkFreeMemory(vkDevice, uniformData_Cube.vkDeviceMemory, NULL);
			uniformData_Cube.vkDeviceMemory = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFreed uniformData_Cube.vkDeviceMemory \n");
		}

		// Destroy uniform buffer for recatngle
		if (uniformData_Pyramid.vkBuffer)
		{
			vkDestroyBuffer(vkDevice, uniformData_Pyramid.vkBuffer, NULL);
			uniformData_Pyramid.vkBuffer = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFreed uniformData_Pyramid.vkBuffer \n");
		}

		if (uniformData_Pyramid.vkDeviceMemory)
		{
			vkFreeMemory(vkDevice, uniformData_Pyramid.vkDeviceMemory, NULL);
			uniformData_Pyramid.vkDeviceMemory = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFreed uniformData_Pyramid.vkDeviceMemory \n");
		}

		// Destroy sampler (uses the image view)
		if (vkSampler_Texture) 
		{
			vkDestroySampler(vkDevice, vkSampler_Texture, NULL);
			vkSampler_Texture = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFreed vkSampler_Texture \n");
		}

		// Destroy image view (uses the image)
		if (vkImageView_Texture) 
		{
			vkDestroyImageView(vkDevice, vkImageView_Texture, NULL);
			vkImageView_Texture = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFreed vkImageView_Texture \n");
		}

		// Free memory last
		if (vkDeviceMemory_Texture) 
		{
			vkFreeMemory(vkDevice, vkDeviceMemory_Texture, NULL);
			vkDeviceMemory_Texture = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFreed vkDeviceMemory_Texture \n");
		}

		// Destroy image 
		if (vkImage_Texture) 
		{
			vkDestroyImage(vkDevice, vkImage_Texture, NULL);
			vkImage_Texture = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFreed VkImage_Texture \n");
		}

		// free color buffer
		if (vertexData_Texcoord_Pyramid.vkDeviceMemory)
		{
			vkFreeMemory(vkDevice, vertexData_Texcoord_Pyramid.vkDeviceMemory, NULL);
			vertexData_Texcoord_Pyramid.vkDeviceMemory = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFree vertexData_Texcoord_Pyramid.vkDeviceMemory freed\n");

		}

		if (vertexData_Texcoord_Pyramid.vkBuffer)
		{
			vkDestroyBuffer(vkDevice, vertexData_Texcoord_Pyramid.vkBuffer, NULL);
			vertexData_Texcoord_Pyramid.vkBuffer = VK_NULL_HANDLE;
			fprintf(gpFile, "\vertexData_Texcoord_Pyramid.vkBuffer freed\n");

		}

		// for triangle
		if (vertexData_Position_Pyramid.vkDeviceMemory)
		{
			vkFreeMemory(vkDevice, vertexData_Position_Pyramid.vkDeviceMemory, NULL);
			vertexData_Position_Pyramid.vkDeviceMemory = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFree vertexData_Position_Pyramid.vkDeviceMemory freed\n");

		}

		if (vertexData_Position_Pyramid.vkBuffer)
		{
			vkDestroyBuffer(vkDevice, vertexData_Position_Pyramid.vkBuffer, NULL);
			vertexData_Position_Pyramid.vkBuffer = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFree vertexData_Position_Pyramid.vkBuffer freed\n");

		}

		if (vertexData_Texcoord_Cube.vkDeviceMemory)
		{
			vkFreeMemory(vkDevice, vertexData_Texcoord_Cube.vkDeviceMemory, NULL);
			vertexData_Texcoord_Cube.vkDeviceMemory = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFree vertexData_Texcoord_Cube.vkDeviceMemory freed\n");

		}

		if (vertexData_Texcoord_Cube.vkBuffer)
		{
			vkDestroyBuffer(vkDevice, vertexData_Texcoord_Cube.vkBuffer, NULL);
			vertexData_Texcoord_Cube.vkBuffer = VK_NULL_HANDLE;
			fprintf(gpFile, "\vertexData_Texcoord_Cube.vkBuffer freed\n");

		}

		// for rectgnle
		if (vertexData_Position_Cube.vkDeviceMemory)
		{
			vkFreeMemory(vkDevice, vertexData_Position_Cube.vkDeviceMemory, NULL);
			vertexData_Position_Cube.vkDeviceMemory = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFree vertexData_Position_Cube.vkDeviceMemory freed\n");

		}

		if (vertexData_Position_Cube.vkBuffer)
		{
			vkDestroyBuffer(vkDevice, vertexData_Position_Cube.vkBuffer, NULL);
			vertexData_Position_Cube.vkBuffer = VK_NULL_HANDLE;
			fprintf(gpFile, "\nFree vertexData_Position_Cube.vkBuffer freed\n");

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

//////////////////////////////////////////////////////     DEFINATION OF VULKUN RELATED FUNCTION     //////////////////////////////////////////////////////////

VkResult createVulkanInstance(void)
{
	// function declarations
	VkResult fillExtensionNames();

	VkResult fillValidationLayerNames(void);

	VkResult createValidationCallbackfuntion(void);

	// variable declarations
	VkResult vkresult = VK_SUCCESS;

	// code
	vkresult = fillExtensionNames();
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
	if (bValidation == TRUE)
	{
		vkresult = fillValidationLayerNames();
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
	vkInstanceCreateInfo.pApplicationInfo = &vkApplicationInfo;
	vkInstanceCreateInfo.enabledExtensionCount = enabledInstanceExtensionCount;
	vkInstanceCreateInfo.ppEnabledExtensionNames = enabledInstanceExtensionNames_array;  // Corrected the field name

	if (bValidation == TRUE)
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
	if (bValidation == TRUE)
	{
		vkresult = createValidationCallbackfuntion();
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
	VkBool32 win32SurfaceExtensionFound = VK_FALSE;
	VkBool32 debugReportExtensionFound = VK_FALSE;

	for (uint32_t i = 0; i < instanceExtensionCount; i++)
	{
		if (strcmp(InstanceExtensionNames_array[i], VK_KHR_SURFACE_EXTENSION_NAME) == 0)
		{
			vulkunSurfaceExtensionFound = VK_TRUE;
			enabledInstanceExtensionNames_array[enabledInstanceExtensionCount++] = VK_KHR_SURFACE_EXTENSION_NAME;
		}

		if (strcmp(InstanceExtensionNames_array[i], VK_KHR_WIN32_SURFACE_EXTENSION_NAME) == 0)
		{
			win32SurfaceExtensionFound = VK_TRUE;
			enabledInstanceExtensionNames_array[enabledInstanceExtensionCount++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
		}

		if (strcmp(InstanceExtensionNames_array[i], VK_EXT_DEBUG_REPORT_EXTENSION_NAME) == 0)
		{
			debugReportExtensionFound = VK_TRUE;

			if (bValidation == TRUE)
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

	if (win32SurfaceExtensionFound == VK_FALSE)
	{
		vkresult = VK_ERROR_INITIALIZATION_FAILED; // return hardcoded failure
		fprintf(gpFile, "fillExtensionNames() : VK_KHR_WIN32_SURFACE_EXTENSION_NAME not found\n");
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "fillExtensionNames() : VK_KHR_WIN32_SURFACE_EXTENSION_NAME found\n");
	}


	if (vulkunSurfaceExtensionFound == VK_FALSE)
	{
		if (bValidation == TRUE)
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
		if (bValidation == TRUE)
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

VkResult fillValidationLayerNames(void)
{
	// variable declarations
	VkResult vkresult = VK_SUCCESS;

	uint32_t validationLayerCount = 0;

	vkresult = vkEnumerateInstanceLayerProperties(&validationLayerCount, NULL);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "fillValidationLayerNames() : vkEnumerateInstanceLayerProperties function 1st call failed with error code %d\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "fillValidationLayerNames() : vkEnumerateInstanceLayerProperties() 1st call succeeded\n");
	}

	VkLayerProperties* vkLayerProperties_Array = NULL;
	vkLayerProperties_Array = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * validationLayerCount);

	vkresult = vkEnumerateInstanceLayerProperties(&validationLayerCount, vkLayerProperties_Array);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "fillValidationLayerNames() : vkEnumerateInstanceLayerProperties function 2nd call failed with error code %d\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "fillValidationLayerNames() : vkEnumerateInstanceLayerProperties() 2nd call succeeded\n");
	}

	char** validationLayerNames_Array = NULL;
	validationLayerNames_Array = (char**)malloc(sizeof(char*) * validationLayerCount);

	for (uint32_t i = 0; i < validationLayerCount; i++)
	{
		validationLayerNames_Array[i] = (char*)malloc(sizeof(char) * strlen(vkLayerProperties_Array[i].layerName) + 1);
		memcpy(validationLayerNames_Array[i], vkLayerProperties_Array[i].layerName, strlen(vkLayerProperties_Array[i].layerName) + 1);
		fprintf(gpFile, "fillValidationLayerNames() : Vulkan Layer name = %s\n", validationLayerNames_Array[i]);
	}

	free(vkLayerProperties_Array);
	vkLayerProperties_Array = NULL;

	// for requireed one validation layer
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
		vkresult = VK_ERROR_INITIALIZATION_FAILED; // return hardcoded failure
		fprintf(gpFile, "fillValidationLayerNames() : VK_LAYER_KHRONOS_validation not found\n");
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "fillValidationLayerNames() : VK_LAYER_KHRONOS_validation found\n");
	}

	// Step 7: Print only enabled layer names
	for (uint32_t i = 0; i < enabledValidationLayerCount; i++)
	{
		fprintf(gpFile, "fillValidationLayerNames() : enabled Vulkan layer name = %s\n", enabledValidationLayerNames_Array[i]);
	}

	//code
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
		fprintf(gpFile, "createValidationCallbackfuntion() : vkGetInstanceProcAddr() failed to get function pointer failed with error code %d \n vkCreateDebugReportCallbackEXT\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createValidationCallbackfuntion() : vkGetInstanceProcAddr() succeded to get function pointer for \n vkCreateDebugReportCallbackEXT\n");
	}

	vkDestroyDebugReportCallbackEXT_fnptr =(PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugReportCallbackEXT");
	if (vkDestroyDebugReportCallbackEXT_fnptr == NULL)
	{
		vkresult = VK_ERROR_INITIALIZATION_FAILED;
		fprintf(gpFile, "createValidationCallbackFunction() : vkGetInstanceProcAddr() failed to get function pointer for vkDestroyDebugReportCallbackEXT\n",__FUNCTION__);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createValidationCallbackFunction() : vkGetInstanceProcAddr() succeeded in getting function pointer for vkDestroyDebugReportCallbackEXT\n");
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
		fprintf(gpFile, "createValidationCallbackfuntion() : vkCreateDebugReportCallbackEXT_fnptr function failed with error code %d\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createValidationCallbackfuntion() : vkCreateDebugReportCallbackEXT_fnptr() succeeded\n");
	}


	fprintf(gpFile, "createValidationCallbackfuntion() : ******************************************* EXITING FROM THIS FUNCTIION*********************************\n");
	return vkresult;

}

VkResult getSupportedSurface(void)
{
	VkResult vkresult = VK_SUCCESS;

	VkWin32SurfaceCreateInfoKHR vkWin32SurfaceCreateInfoKHR;
	memset((void*)&vkWin32SurfaceCreateInfoKHR, 0, sizeof(VkWin32SurfaceCreateInfoKHR));

	vkWin32SurfaceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	vkWin32SurfaceCreateInfoKHR.pNext = NULL;
	vkWin32SurfaceCreateInfoKHR.flags = 0;
	vkWin32SurfaceCreateInfoKHR.hinstance = (HINSTANCE)GetWindowLongPtr(ghwnd, GWLP_HINSTANCE);
	vkWin32SurfaceCreateInfoKHR.hwnd = ghwnd;

	vkresult = vkCreateWin32SurfaceKHR(vkInstance, &vkWin32SurfaceCreateInfoKHR, NULL, &vkSurfaceKHR);
	if(vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "getSupportedSurface() : vkCreateWin32SurfaceKHR Failed\n");
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "getSupportedSurface() : vkCreateWin32SurfaceKHR succeded \n");
	}

	return vkresult;

}

VkResult getPhysicalDevice(void)
{
	VkResult vkresult = VK_SUCCESS;

	vkresult = vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, NULL);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "getPhysicalDevice() 1st call : vkEnumeratePhysicalDevices Failed (%d)\n", vkresult);
		return vkresult;
	}
	else if (physicalDeviceCount == 0) 
	{
		fprintf(gpFile, "getPhysicalDevice() 1st call : vkEnumeratePhysicalDevices resulted in zero physical Devices \n");
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "getPhysicalDevice() 1st call : vkEnumeratePhysicalDevices succeeded \n");
	}

	vkPhysicalDevice_Array = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * physicalDeviceCount);
	if (vkPhysicalDevice_Array == NULL) 
	{
		fprintf(gpFile, "Memory allocation failed for vkPhysicalDevice_Array\n");
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	vkresult = vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, vkPhysicalDevice_Array);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "getPhysicalDevice() 2nd call : vkEnumeratePhysicalDevices Failed (%d)\n", vkresult);
		free(vkPhysicalDevice_Array);  // Free on error
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "getPhysicalDevice() 2nd call  : vkEnumeratePhysicalDevices succeeded \n");
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
			fprintf(gpFile, "Memory allocation failed for vkQueueFamilyProperties_Array\n");
			free(vkPhysicalDevice_Array);  // Free previous allocations
			return VK_ERROR_OUT_OF_HOST_MEMORY;
		}

		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice_Array[i], &queueCount, vkQueueFamilyProperties_Array);

		VkBool32* isQueueSurface_Array = NULL;

		// Allocate memory for queue surface support
		isQueueSurface_Array = (VkBool32*)malloc(sizeof(VkBool32) * queueCount);
		if (isQueueSurface_Array == NULL)  // Check memory allocation
		{
			fprintf(gpFile, "Memory allocation failed for isQueueSurface_Array\n");
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
		fprintf(gpFile, "getPhysicalDevice succeeded to select the required physical device with graphics enabled\n");
	}
	else
	{
		fprintf(gpFile, "getPhysicalDevice() Failed to find a graphics supported physical device\n");
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
		fprintf(gpFile, "getPhysicalDevice supports tessellation shader\n");
	}
	else
	{
		fprintf(gpFile, "getPhysicalDevice does not support tessellation shader\n");
	}

	// Check for geometry shader support
	if (vkPhysicalDeviceFeatures.geometryShader)
	{
		fprintf(gpFile, "getPhysicalDevice supports geometry shader\n");
	}
	else
	{
		fprintf(gpFile, "getPhysicalDevice does not support geometry shader\n");
	}

	return vkresult;
}

VkResult printVkInfo()
{
	VkResult vkresult = VK_SUCCESS;

	// Header for Vulkan Information
	fprintf(gpFile, "********************** VULKAN INFORMATION *****************************\n");

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
		fprintf(gpFile, "apiVersion    = %d.%d.%d\n", majorVersion, minorVersion, patchVersion);

		// DEVICE NAME
		fprintf(gpFile, "deviceName    = %s\n", vkPhysicalDeviceProperties.deviceName);

		// DEVICE TYPE
		fprintf(gpFile, "deviceType    = ");
		switch (vkPhysicalDeviceProperties.deviceType)
		{
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			fprintf(gpFile, "integratedGPU (iGPU)\n");  // Correct spelling
			break;

		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			fprintf(gpFile, "discreteGPU (dGPU)\n");
			break;

		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			fprintf(gpFile, "virtualGPU (vGPU)\n");
			break;

		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			fprintf(gpFile, "CPU\n");
			break;

		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			fprintf(gpFile, "Other\n");
			break;

		default:
			fprintf(gpFile, "UNKNOWN\n");
		}

		// Vendor ID (Hexadecimal)
		fprintf(gpFile, "vendorID      = 0x%04x\n", vkPhysicalDeviceProperties.vendorID);

		// Device ID
		fprintf(gpFile, "deviceID      = 0x%04x\n", vkPhysicalDeviceProperties.deviceID);

		fprintf(gpFile, "--------------------------------------------------------------------\n");
	}

	// Free allocated memory if necessary
	if (vkPhysicalDevice_Array)
	{
		free(vkPhysicalDevice_Array);
		fprintf(gpFile, "printVkInfo(): Physical Device memory freed.\n");
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
        fprintf(gpFile, "fillDeviceExtensionNames(): VK_KHR_SWAPCHAIN_EXTENSION_NAME not found\n");
        return vkresult;
    }
    fprintf(gpFile, "fillDeviceExtensionNames(): VK_KHR_SWAPCHAIN_EXTENSION_NAME found\n");

    // Step 7: Print enabled extension names
    for (uint32_t i = 0; i < enableDeviceExtensionCount; i++)
    {
        fprintf(gpFile, "fillDeviceExtensionNames(): Enabled Vulkan Device Extension = %s\n", enabledDeviceExtensionNames_array[i]);
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
		fprintf(gpFile, "createVulkanDevice() : fillDeviceExtensionNames function failed\n");
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "createVulkanDevice() : fillDeviceExtensionNames() succeeded\n");
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
		fprintf(gpFile, "createVulkanDevice() : vkCreateDevice function failed\n");
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "createVulkanDevice() : vkCreateDevice() succeeded\n");
	}

	return vkresult;
}


void getDeviceQueue(void)
{
	vkGetDeviceQueue(vkDevice, graphicsQueueFamilyIndex_Selected, 0, &vkQueue);

	if (vkQueue == VK_NULL_HANDLE)
	{
		fprintf(gpFile, "getDeviceQueue() : vkGetDeviceQueue returned NULL for vkQueue. Check if the device was created with the correct queue family.\n");
		return;
	}
	else
	{
		fprintf(gpFile, "getDeviceQueue() : vkGetDeviceQueue() succeeded\n");
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
		fprintf(gpFile, "getPhysicalDeviceSurfaceFormatAndColorSpace(): vkGetPhysicalDeviceSurfaceFormatsKHR() failed\n");
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "getPhysicalDeviceSurfaceFormatAndColorSpace() : vkGetPhysicalDeviceSurfaceFormatsKHR() succeeded\n");
	}

	// Check if no surface formats are found
	if (formatCount == 0)
	{
		fprintf(gpFile, "getPhysicalDeviceSurfaceFormatAndColorSpace(): No surface formats found\n");
		return VK_ERROR_INITIALIZATION_FAILED; // Hardcoded failure
	}

	fprintf(gpFile, "getPhysicalDeviceSurfaceFormatAndColorSpace(): Found %u surface formats\n", formatCount);

	// Allocate memory for surface format array
	VkSurfaceFormatKHR* vkSurfaceFormatKHR_Array = (VkSurfaceFormatKHR*)malloc(formatCount * sizeof(VkSurfaceFormatKHR));

	// Second call to fetch the actual supported surface formats
	vkresult = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice_selected, vkSurfaceKHR, &formatCount, vkSurfaceFormatKHR_Array);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "getPhysicalDeviceSurfaceFormatAndColorSpace(): Second call to vkGetPhysicalDeviceSurfaceFormatsKHR() failed\n");
		free(vkSurfaceFormatKHR_Array);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "getPhysicalDeviceSurfaceFormatAndColorSpace() Second call: vkGetPhysicalDeviceSurfaceFormatsKHR succeeded\n");
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
		fprintf(gpFile, "getPhysicalDeviceSurfaceFormatAndColorSpace(): vkSurfaceFormatKHR_Array Memory freed\n");
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
		fprintf(gpFile, "getPhysicalDevicePresentMode() 1st call: vkGetPhysicalDeviceSurfacePresentModesKHR() failed\n");
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "getPhysicalDevicePresentMode() 1st call : vkGetPhysicalDeviceSurfacePresentModesKHR() succeeded\n");
	}

	// Check if no present modes are found
	if (presentModeCount == 0)
	{
		fprintf(gpFile, "getPhysicalDevicePresentMode(): No present modes found\n");
		return VK_ERROR_INITIALIZATION_FAILED; // Hardcoded failure
	}

	fprintf(gpFile, "getPhysicalDevicePresentMode(): Found %u present modes\n", presentModeCount);

	// Allocate memory for present mode array
	VkPresentModeKHR* vkPresentModeKHR_Array = (VkPresentModeKHR*)malloc(presentModeCount * sizeof(VkPresentModeKHR));
	if (!vkPresentModeKHR_Array)  // Ensure memory allocation was successful
	{
		fprintf(gpFile, "getPhysicalDevicePresentMode(): Memory allocation failed\n");
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	// Second call to fetch the actual supported present modes
	vkresult = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice_selected, vkSurfaceKHR, &presentModeCount, vkPresentModeKHR_Array);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "getPhysicalDevicePresentMode() 2nd call: vkGetPhysicalDeviceSurfacePresentModesKHR() failed\n");
		free(vkPresentModeKHR_Array);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "getPhysicalDevicePresentMode() 2nd call: vkGetPhysicalDeviceSurfacePresentModesKHR() succeeded\n");
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
		fprintf(gpFile, "getPhysicalDevicePresentMode(): Selected Present Mode = MAILBOX_KHR\n");
	}
	else
	{
		fprintf(gpFile, "getPhysicalDevicePresentMode(): Selected Present Mode = FIFO_KHR\n");
	}

	// Free allocated memory
	if (vkPresentModeKHR_Array)
	{
		free(vkPresentModeKHR_Array);
		vkPresentModeKHR_Array = NULL;
		fprintf(gpFile, "getPhysicalDevicePresentMode(): Memory freed, function completed successfully\n");
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
	vkresult = getPhysicalDevicePresentMode();
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

VkResult createImagesAndImageViews(void)
{
	// function declarations
	VkResult getSupportedDepthFormat(void);

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
			fprintf(gpFile, "createImagesAndImageViews() : vkCreateImageView() succeeded for iteration (%d)\n", i);
		}
	}

	// for depth image

	vkresult = getSupportedDepthFormat();
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
	vkImageCreateInfo.extent.width = winWidth;  // Image width
	vkImageCreateInfo.extent.height = winHeight; // Image height
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

	for (uint32_t i = 0; i < vkPhysicalDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((vkMemoryRequirements.memoryTypeBits & 1) == 1)
		{
			if (vkPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
			{
				vkMemoryAllocateInfo.memoryTypeIndex = i;
				break;
			}
		}

		vkMemoryRequirements.memoryTypeBits >>= 1;

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
	vkImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
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

VkResult getSupportedDepthFormat(void)
{
	// code

	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	VkFormat VkFormat_Depth_Array[] =
	{
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
	};

	for (uint32_t i = 0; i < sizeof(VkFormat_Depth_Array) / sizeof(VkFormat_Depth_Array[0]); i++)
	{
		VkFormatProperties vkFormatProperties;
		memset(&vkFormatProperties, 0, sizeof(VkFormatProperties));

		vkGetPhysicalDeviceFormatProperties(vkPhysicalDevice_selected, VkFormat_Depth_Array[i], &vkFormatProperties);

		if (vkFormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			// This format can be used as a depth-stencil attachment
			vkFormat_Depth = VkFormat_Depth_Array[i];

			vkresult = VK_SUCCESS;
			break;
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
		fprintf(gpFile, "createCommandPool() : vkCreateCommandPool() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createCommandPool() : vkCreateCommandPool() succeeded.\n");
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
		fprintf(gpFile, "createCommandBuffers() : vkAllocateCommandBuffers() function failed. Error Code: (%d)\n", vkresult);
		free(vkCommandBuffer_Array);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createCommandBuffers() : vkAllocateCommandBuffers() succeeded.\n");
	}

	return vkresult;
}

VkResult createVertexBuffer(void)
{

	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	// position
	float pyramidVertices[] =
	{
		// front
		0.0f,  1.0f,  0.0f, // front-top
	   -1.0f, -1.0f,  1.0f, // front-left
		1.0f, -1.0f,  1.0f, // front-right

		// right
		0.0f,  1.0f,  0.0f, // right-top
		1.0f, -1.0f,  1.0f, // right-left
		1.0f, -1.0f, -1.0f, // right-right

		// back
		0.0f,  1.0f,  0.0f, // back-top
		1.0f, -1.0f, -1.0f, // back-left
	   -1.0f, -1.0f, -1.0f, // back-right

	   // left
	   0.0f,  1.0f,  0.0f, // left-top
	  -1.0f, -1.0f, -1.0f, // left-left
	  -1.0f, -1.0f,  1.0f, // left-right
	};

	// color
	float pyramidTexcoords[] =
	{
		// front
		0.5, 1.0, // front-top
		0.0, 0.0, // front-left
		1.0, 0.0, // front-right

		// right
		0.5, 1.0, // right-top
		1.0, 0.0, // right-left
		0.0, 0.0, // right-right

		// back
		0.5, 1.0, // back-top
		0.0, 0.0, // back-left
		1.0, 0.0, // back-right

		// left
		0.5, 1.0, // left-top
		1.0, 0.0, // left-left
		0.0, 0.0, // left-right
	};

	// position
	float cubeVertices[] =
	{
		// Front face     
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,

		// Right face 
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,

		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,

		// Back face
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,

		1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		// Left face 
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,

		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,

		// Top face 
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		// Bottom face 
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,

		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f
	};

	float cubeTexcoords[] =
	{
		// Front face
		1.0f, 1.0f, // v0 top-right
		0.0f, 1.0f, // v1 top-left
		0.0f, 0.0f, // v2 bottom-left

		1.0f, 1.0f, // v3 top-right
		0.0f, 0.0f, // v4 bottom-left
		1.0f, 0.0f, // v5 bottom-right

		// Right face
		1.0f, 1.0f, // v0 top-right
		0.0f, 1.0f, // v1 top-left
		0.0f, 0.0f, // v2 bottom-left

		1.0f, 1.0f, // v3 top-right
		0.0f, 0.0f, // v4 bottom-left
		1.0f, 0.0f, // v5 bottom-right

		// Back face
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		// Left face
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		// Top face
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		// Bottom face
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
	};

	// VERTEX POSITION BUFFER for pyramid
	memset((void*)&vertexData_Position_Pyramid, 0, sizeof(VertexData));

	VkBufferCreateInfo vkBufferCreateInfo;
	memset((void*)& vkBufferCreateInfo, 0, sizeof(VkBufferCreateInfo));

	vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBufferCreateInfo.pNext = NULL;
	vkBufferCreateInfo.flags = 0;
	vkBufferCreateInfo.size = sizeof(pyramidVertices);
	vkBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	vkresult = vkCreateBuffer(vkDevice, &vkBufferCreateInfo, NULL, &vertexData_Position_Pyramid.vkBuffer);
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

	vkGetBufferMemoryRequirements(vkDevice, vertexData_Position_Pyramid.vkBuffer, &vkMemoryRequirements);

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

	vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, NULL, &vertexData_Position_Pyramid.vkDeviceMemory);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkAllocateMemory() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkAllocateMemory() succeeded.\n");
	}

	vkresult = vkBindBufferMemory(vkDevice, vertexData_Position_Pyramid.vkBuffer, vertexData_Position_Pyramid.vkDeviceMemory, 0);
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

	vkresult = vkMapMemory(vkDevice, vertexData_Position_Pyramid.vkDeviceMemory, 0, vkMemoryAllocateInfo.allocationSize, 0, &data);
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

	memcpy(data, pyramidVertices, sizeof(pyramidVertices));

	vkUnmapMemory(vkDevice, vertexData_Position_Pyramid.vkDeviceMemory);

	// VERTEX TEXCOORD BUFFER
	memset((void*)&vertexData_Texcoord_Pyramid, 0, sizeof(VertexData));

	memset((void*)&vkBufferCreateInfo, 0, sizeof(VkBufferCreateInfo));

	vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBufferCreateInfo.pNext = NULL;
	vkBufferCreateInfo.flags = 0;
	vkBufferCreateInfo.size = sizeof(pyramidTexcoords);
	vkBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	vkresult = vkCreateBuffer(vkDevice, &vkBufferCreateInfo, NULL, &vertexData_Texcoord_Pyramid.vkBuffer);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkCreateBuffer() function failed for vertex texcoord buffer Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkCreateBuffer() succeeded for vertex texcoord buffer\n");
	}

	memset((void*)&vkMemoryRequirements, 0, sizeof(VkMemoryRequirements));

	vkGetBufferMemoryRequirements(vkDevice, vertexData_Texcoord_Pyramid.vkBuffer, &vkMemoryRequirements);

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

	vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, NULL, &vertexData_Texcoord_Pyramid.vkDeviceMemory);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkAllocateMemory() function failed failed for vertex texcoord buffer Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkAllocateMemory() succeeded for vertex texcoord buffer\n");
	}

	vkresult = vkBindBufferMemory(vkDevice, vertexData_Texcoord_Pyramid.vkBuffer, vertexData_Texcoord_Pyramid.vkDeviceMemory, 0);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkBindBufferMemory() function failed failed for vertex texcoord buffer Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkBindBufferMemory() succeeded  for vertex texcoord buffer\n");
	}

	data = NULL;

	vkresult = vkMapMemory(vkDevice, vertexData_Texcoord_Pyramid.vkDeviceMemory, 0, vkMemoryAllocateInfo.allocationSize, 0, &data);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkMapMemory() function failed for vertex texcoord buffer Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkMapMemory() succeeded for vertex texcoord buffer\n");
	}

	// actual memory mapped

	memcpy(data, pyramidTexcoords, sizeof(pyramidTexcoords));

	vkUnmapMemory(vkDevice, vertexData_Texcoord_Pyramid.vkDeviceMemory);


	// VERTEX POSITION BUFFER FOR CUBE
	memset((void*)&vertexData_Position_Cube, 0, sizeof(VertexData));

	memset((void*)&vkBufferCreateInfo, 0, sizeof(VkBufferCreateInfo));

	vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBufferCreateInfo.pNext = NULL;
	vkBufferCreateInfo.flags = 0;
	vkBufferCreateInfo.size = sizeof(cubeVertices);
	vkBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	vkresult = vkCreateBuffer(vkDevice, &vkBufferCreateInfo, NULL, &vertexData_Position_Cube.vkBuffer);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkCreateBuffer() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkCreateBuffer() succeeded.\n");
	}

	memset((void*)&vkMemoryRequirements, 0, sizeof(VkMemoryRequirements));

	vkGetBufferMemoryRequirements(vkDevice, vertexData_Position_Cube.vkBuffer, &vkMemoryRequirements);

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

	vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, NULL, &vertexData_Position_Cube.vkDeviceMemory);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkAllocateMemory() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkAllocateMemory() succeeded.\n");
	}

	vkresult = vkBindBufferMemory(vkDevice, vertexData_Position_Cube.vkBuffer, vertexData_Position_Cube.vkDeviceMemory, 0);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkBindBufferMemory() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkBindBufferMemory() succeeded.\n");
	}

	data = NULL;

	vkresult = vkMapMemory(vkDevice, vertexData_Position_Cube.vkDeviceMemory, 0, vkMemoryAllocateInfo.allocationSize, 0, &data);
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

	memcpy(data, cubeVertices, sizeof(cubeVertices));

	vkUnmapMemory(vkDevice, vertexData_Position_Cube.vkDeviceMemory);

	// VERTEX TEXCOORD BUFFER FOR CUBE
	memset((void*)&vertexData_Texcoord_Cube, 0, sizeof(VertexData));

	memset((void*)&vkBufferCreateInfo, 0, sizeof(VkBufferCreateInfo));

	vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBufferCreateInfo.pNext = NULL;
	vkBufferCreateInfo.flags = 0;
	vkBufferCreateInfo.size = sizeof(cubeTexcoords);
	vkBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	vkresult = vkCreateBuffer(vkDevice, &vkBufferCreateInfo, NULL, &vertexData_Texcoord_Cube.vkBuffer);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkCreateBuffer() function failed for vertex texcoord buffer Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkCreateBuffer() succeeded for vertex texcoord buffer\n");
	}

	memset((void*)&vkMemoryRequirements, 0, sizeof(VkMemoryRequirements));

	vkGetBufferMemoryRequirements(vkDevice, vertexData_Texcoord_Cube.vkBuffer, &vkMemoryRequirements);

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

	vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, NULL, &vertexData_Texcoord_Cube.vkDeviceMemory);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkAllocateMemory() function failed failed for vertex texcoord buffer Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkAllocateMemory() succeeded for vertex texcoord buffer\n");
	}

	vkresult = vkBindBufferMemory(vkDevice, vertexData_Texcoord_Cube.vkBuffer, vertexData_Texcoord_Cube.vkDeviceMemory, 0);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkBindBufferMemory() function failed failed for vertex texcoord buffer Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkBindBufferMemory() succeeded  for vertex texcoord buffer\n");
	}

	data = NULL;

	vkresult = vkMapMemory(vkDevice, vertexData_Texcoord_Cube.vkDeviceMemory, 0, vkMemoryAllocateInfo.allocationSize, 0, &data);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkMapMemory() function failed for vertex texcoord buffer Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkMapMemory() succeeded for vertex texcoord buffer\n");
	}

	// actual memory mapped

	memcpy(data, cubeTexcoords, sizeof(cubeTexcoords));

	vkUnmapMemory(vkDevice, vertexData_Texcoord_Cube.vkDeviceMemory);

	return vkresult;

}

VkResult createTexture(const char* textureFileName)
{
	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	//code
	
	// step 1:
	// get image data
	FILE* fp = NULL;

	fp = fopen(textureFileName, "rb");

	if (fp == NULL)
	{
		fprintf(gpFile, "fopen failed for reading texture file");
		vkresult = VK_ERROR_INITIALIZATION_FAILED;
		return vkresult;
	}

	uint8_t *image_Data = NULL;

	int texture_width, texture_Height, texture_channels;

	image_Data = stbi_load_from_file(fp, &texture_width, &texture_Height, &texture_channels, STBI_rgb_alpha);

	if (image_Data == NULL || texture_width <= 0 || texture_Height <= 0 || texture_channels <= 0)
	{
		fprintf(gpFile, "createTexture(): stbi_loadf_from_file function failed \n");
		vkresult = VK_ERROR_INITIALIZATION_FAILED;
		return vkresult;
	}

	VkDeviceSize image_size = texture_width * texture_Height * 4; // for rgba

	// step 2 
	// create stagging buffer

	VkBuffer vkBuffer_StaggingBuffer = VK_NULL_HANDLE;

	VkDeviceMemory VkDeviceMemory_StaggingBuffer = VK_NULL_HANDLE;

	VkBufferCreateInfo VkBufferCreateInfo_StagingBuffer;
	memset((void*)&VkBufferCreateInfo_StagingBuffer, 0, sizeof(VkBufferCreateInfo));

	VkBufferCreateInfo_StagingBuffer.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	VkBufferCreateInfo_StagingBuffer.pNext = NULL;
	VkBufferCreateInfo_StagingBuffer.size = image_size; // total size of image in bytes
	VkBufferCreateInfo_StagingBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT; // staging buffer usage
	VkBufferCreateInfo_StagingBuffer.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // usually exclusive

	vkresult = vkCreateBuffer(vkDevice, &VkBufferCreateInfo_StagingBuffer, NULL, &vkBuffer_StaggingBuffer);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkCreateBuffer() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkCreateBuffer() succeeded.\n");
	}

	VkMemoryRequirements vkMemoryRequirements_StaggingBuffer;
	memset((void*)&vkMemoryRequirements_StaggingBuffer, 0, sizeof(VkMemoryRequirements));

	vkGetBufferMemoryRequirements(vkDevice, vkBuffer_StaggingBuffer, &vkMemoryRequirements_StaggingBuffer);

	VkMemoryAllocateInfo vkMemoryAllocateInfo_StaggingBuffer;
	memset((void*)&vkMemoryAllocateInfo_StaggingBuffer, 0, sizeof(VkMemoryAllocateInfo));

	vkMemoryAllocateInfo_StaggingBuffer.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkMemoryAllocateInfo_StaggingBuffer.pNext = NULL;
	vkMemoryAllocateInfo_StaggingBuffer.allocationSize = vkMemoryRequirements_StaggingBuffer.size;
	vkMemoryAllocateInfo_StaggingBuffer.memoryTypeIndex = 0; // initial value before entering into loop

	for (uint32_t i = 0; i < vkPhysicalDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((vkMemoryRequirements_StaggingBuffer.memoryTypeBits & 1) == 1)
		{
			if (vkPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
			{
				vkMemoryAllocateInfo_StaggingBuffer.memoryTypeIndex = i;
				break;
			}
		}

		vkMemoryRequirements_StaggingBuffer.memoryTypeBits >>= 1;

	}

	vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo_StaggingBuffer, NULL, &VkDeviceMemory_StaggingBuffer);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkAllocateMemory() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkAllocateMemory() succeeded.\n");
	}

	vkresult = vkBindBufferMemory(vkDevice, vkBuffer_StaggingBuffer, VkDeviceMemory_StaggingBuffer, 0);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkBindBufferMemory() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkBindBufferMemory() succeeded.\n");
	}


	void* data = NULL;

	vkresult = vkMapMemory(vkDevice, VkDeviceMemory_StaggingBuffer, 0, image_size, 0, &data);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkMapMemory() function failed for vertex texcoord buffer Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkMapMemory() succeeded for vertex texcoord buffer\n");
	}

	// actual memory mapped

	memcpy(data, image_Data, image_size);

	vkUnmapMemory(vkDevice, VkDeviceMemory_StaggingBuffer);

	// as copying of imagge data is already done into stagging buffer , we can free the actual image data given by stb

	stbi_image_free(image_Data);

	image_Data = NULL;

	fprintf(gpFile, "createTexture() : stbi_image_free() succeeded for image data\n");

	// step 3 

	VkImageCreateInfo vkImageCreateInfo;
	memset((void*)&vkImageCreateInfo, 0, sizeof(VkImageCreateInfo));

	vkImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	vkImageCreateInfo.pNext = NULL;
	vkImageCreateInfo.flags = 0;
	vkImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	vkImageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM; // if using stbi_loadf
	vkImageCreateInfo.extent.width = texture_width;
	vkImageCreateInfo.extent.height = texture_Height;
	vkImageCreateInfo.extent.depth = 1;
	vkImageCreateInfo.mipLevels = 1;
	vkImageCreateInfo.arrayLayers = 1;
	vkImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	vkImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	vkImageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	vkImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vkImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	vkresult = vkCreateImage(vkDevice, &vkImageCreateInfo, NULL, &vkImage_Texture);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkCreateImageView() function failed\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkCreateImageView() succeeded\n");
	}

	VkMemoryRequirements vkMemoryRequirements_Image;
	memset((void*)&vkMemoryRequirements_Image, 0, sizeof(VkMemoryRequirements));

	vkGetImageMemoryRequirements(vkDevice, vkImage_Texture, &vkMemoryRequirements_Image);

	VkMemoryAllocateInfo vkMemoryAllocateInfo_Image;
	memset((void*)&vkMemoryAllocateInfo_Image, 0, sizeof(VkMemoryAllocateInfo));

	vkMemoryAllocateInfo_Image.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkMemoryAllocateInfo_Image.pNext = NULL;
	vkMemoryAllocateInfo_Image.allocationSize = vkMemoryRequirements_Image.size;
	vkMemoryAllocateInfo_Image.memoryTypeIndex = 0; // initial value before entering into loop

	for (uint32_t i = 0; i < vkPhysicalDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((vkMemoryRequirements_Image.memoryTypeBits & 1) == 1)
		{
			if (vkPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
			{
				vkMemoryAllocateInfo_Image.memoryTypeIndex = i;
				break;
			}
		}

		vkMemoryRequirements_Image.memoryTypeBits >>= 1;
	}

	vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo_Image, NULL, &vkDeviceMemory_Texture);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkAllocateMemory() function failed for image Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkAllocateMemory() succeeded for image\n");
	}

	vkresult = vkBindImageMemory(vkDevice, vkImage_Texture, vkDeviceMemory_Texture, 0);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkBindBufferMemory() function failed for image Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkBindBufferMemory() succeeded for image\n");

	}

	// step 4:
	VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo_Transition_Image_Layout;
	memset((void*)& vkCommandBufferAllocateInfo_Transition_Image_Layout, 0, sizeof(VkCommandBufferAllocateInfo));

	vkCommandBufferAllocateInfo_Transition_Image_Layout.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	vkCommandBufferAllocateInfo_Transition_Image_Layout.pNext = NULL;
	vkCommandBufferAllocateInfo_Transition_Image_Layout.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	vkCommandBufferAllocateInfo_Transition_Image_Layout.commandPool = vkcommandpool;
	vkCommandBufferAllocateInfo_Transition_Image_Layout.commandBufferCount = 1;

	VkCommandBuffer VkCommandBuffer_Transition_Image_Layout = VK_NULL_HANDLE;

	vkresult = vkAllocateCommandBuffers(vkDevice,
		&vkCommandBufferAllocateInfo_Transition_Image_Layout,
		&VkCommandBuffer_Transition_Image_Layout);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkAllocateCommandBuffers() function failed  Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkAllocateCommandBuffers() succeeded\n");
	}

	VkCommandBufferBeginInfo vkCommandBufferBeginInfo_Transition_Layout;
	memset((void*)&vkCommandBufferBeginInfo_Transition_Layout, 0, sizeof(VkCommandBufferBeginInfo));

	vkCommandBufferBeginInfo_Transition_Layout.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkCommandBufferBeginInfo_Transition_Layout.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkresult = vkBeginCommandBuffer(VkCommandBuffer_Transition_Image_Layout, &vkCommandBufferBeginInfo_Transition_Layout);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkBeginCommandBuffer() function failed  Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkBeginCommandBuffer() succeeded\n");
	}

	VkPipelineStageFlags vkPipelineStageFlags_Source = 0;
	VkPipelineStageFlags vkPipelineStageFlags_Destination = 0;


	VkImageMemoryBarrier vkImageMemoryBarrier;
	memset((void*)&vkImageMemoryBarrier, 0, sizeof(vkImageMemoryBarrier));

	vkImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	vkImageMemoryBarrier.pNext = NULL;
	vkImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	vkImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	vkImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vkImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vkImageMemoryBarrier.image = vkImage_Texture;
	vkImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	vkImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	vkImageMemoryBarrier.subresourceRange.levelCount = 1;
	vkImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	vkImageMemoryBarrier.subresourceRange.layerCount = 1;

	if (vkImageMemoryBarrier.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && vkImageMemoryBarrier.newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		vkImageMemoryBarrier.srcAccessMask = 0;
		vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkPipelineStageFlags_Source = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		vkPipelineStageFlags_Destination = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (vkImageMemoryBarrier.oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && vkImageMemoryBarrier.newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		vkPipelineStageFlags_Source = VK_PIPELINE_STAGE_TRANSFER_BIT;
		vkPipelineStageFlags_Destination = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		fprintf(gpFile, "createTexture() : unspported texture layout transitions\n");
		vkresult = VK_ERROR_INITIALIZATION_FAILED;
		return vkresult;
	}

	vkCmdPipelineBarrier(VkCommandBuffer_Transition_Image_Layout, vkPipelineStageFlags_Source, vkPipelineStageFlags_Destination, 0, 0, NULL, 0, NULL, 1, &vkImageMemoryBarrier);

	vkresult = vkEndCommandBuffer(VkCommandBuffer_Transition_Image_Layout);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkEndCommandBuffer() function failed  Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkEndCommandBuffer() succeeded\n");
	}

	VkSubmitInfo VkSubmitInfo_Transition_Image_Layout;
	memset(&VkSubmitInfo_Transition_Image_Layout, 0, sizeof(VkSubmitInfo));

	VkSubmitInfo_Transition_Image_Layout.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSubmitInfo_Transition_Image_Layout.pNext = NULL;
	VkSubmitInfo_Transition_Image_Layout.commandBufferCount = 1;
	VkSubmitInfo_Transition_Image_Layout.pCommandBuffers = &VkCommandBuffer_Transition_Image_Layout;

	vkresult = vkQueueSubmit(vkQueue, 1, &VkSubmitInfo_Transition_Image_Layout, VK_NULL_HANDLE);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkQueueSubmit() function failed  Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkQueueSubmit() succeeded\n");
	}

	// Wait for the queue to finish the operation
	vkresult = vkQueueWaitIdle(vkQueue);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkQueueWaitIdle() function failed  Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkQueueWaitIdle() succeeded\n");
		fflush(gpFile);
	}

	vkFreeCommandBuffers(vkDevice, vkcommandpool, 1, &VkCommandBuffer_Transition_Image_Layout);
	VkCommandBuffer_Transition_Image_Layout = VK_NULL_HANDLE;

	// step 5:

	VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo_Buffer_To_Image_Copy;
	memset((void*)&vkCommandBufferAllocateInfo_Buffer_To_Image_Copy, 0, sizeof(VkCommandBufferAllocateInfo));

	vkCommandBufferAllocateInfo_Buffer_To_Image_Copy.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	vkCommandBufferAllocateInfo_Buffer_To_Image_Copy.pNext = NULL;
	vkCommandBufferAllocateInfo_Buffer_To_Image_Copy.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	vkCommandBufferAllocateInfo_Buffer_To_Image_Copy.commandPool = vkcommandpool;
	vkCommandBufferAllocateInfo_Buffer_To_Image_Copy.commandBufferCount = 1;

	VkCommandBuffer VkCommandBuffer_Buffer_To_Image_Copy = VK_NULL_HANDLE;

	vkresult = vkAllocateCommandBuffers(vkDevice,
		&vkCommandBufferAllocateInfo_Buffer_To_Image_Copy,
		&VkCommandBuffer_Buffer_To_Image_Copy);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkAllocateCommandBuffers() function failed for buffer to image Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkAllocateCommandBuffers() succeeded for buffer to image\n");
		fflush(gpFile);
	}

	VkCommandBufferBeginInfo vkCommandBufferBeginInfo_Buffer_To_Image_Copy;
	memset((void*)&vkCommandBufferBeginInfo_Buffer_To_Image_Copy, 0, sizeof(VkCommandBufferBeginInfo));

	vkCommandBufferBeginInfo_Buffer_To_Image_Copy.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkCommandBufferBeginInfo_Buffer_To_Image_Copy.pNext = NULL;
	vkCommandBufferBeginInfo_Buffer_To_Image_Copy.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkresult = vkBeginCommandBuffer(VkCommandBuffer_Buffer_To_Image_Copy, &vkCommandBufferBeginInfo_Buffer_To_Image_Copy);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkBeginCommandBuffer() function failed for image to copy Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkBeginCommandBuffer() succeeded for image to copy\n");
		fflush(gpFile);

	}

	VkBufferImageCopy vkBufferImageCopy;
	memset((void*)&vkBufferImageCopy, 0, sizeof(vkBufferImageCopy));

	vkBufferImageCopy.bufferOffset = 0;
	vkBufferImageCopy.bufferRowLength = 0;       
	vkBufferImageCopy.bufferImageHeight = 0;   
	vkBufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	vkBufferImageCopy.imageSubresource.mipLevel = 0;
	vkBufferImageCopy.imageSubresource.baseArrayLayer = 0;
	vkBufferImageCopy.imageSubresource.layerCount = 1;
	vkBufferImageCopy.imageOffset.x = 0;
	vkBufferImageCopy.imageOffset.y = 0;
	vkBufferImageCopy.imageOffset.z = 0;
	vkBufferImageCopy.imageExtent.width = texture_width;
	vkBufferImageCopy.imageExtent.height = texture_Height;
	vkBufferImageCopy.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(VkCommandBuffer_Buffer_To_Image_Copy, vkBuffer_StaggingBuffer, vkImage_Texture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vkBufferImageCopy);

	vkresult = vkEndCommandBuffer(VkCommandBuffer_Buffer_To_Image_Copy);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkEndCommandBuffer() function failed for buffer to image Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkEndCommandBuffer() succeeded buffer to image \n");
		fflush(gpFile);
	}

	VkSubmitInfo VkSubmitInfo_Buffer_To_Image_Copy;
	memset(&VkSubmitInfo_Buffer_To_Image_Copy, 0, sizeof(VkSubmitInfo));

	VkSubmitInfo_Buffer_To_Image_Copy.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSubmitInfo_Buffer_To_Image_Copy.pNext = NULL;
	VkSubmitInfo_Buffer_To_Image_Copy.commandBufferCount = 1;
	VkSubmitInfo_Buffer_To_Image_Copy.pCommandBuffers = &VkCommandBuffer_Buffer_To_Image_Copy;

	vkresult = vkQueueSubmit(vkQueue, 1, &VkSubmitInfo_Buffer_To_Image_Copy, VK_NULL_HANDLE);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkQueueSubmit() function failed for buffer to image   Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkQueueSubmit() succeeded buffer to image\n");
		fflush(gpFile);
	}

	// Wait for the queue to finish the operation
	vkresult = vkQueueWaitIdle(vkQueue);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkQueueWaitIdle() function failed  buffer to image Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkQueueWaitIdle() succeeded buffer to image\n");
		fflush(gpFile);
	}

	// Free the command buffer used for buffer-to-image copy
	vkFreeCommandBuffers(vkDevice, vkcommandpool, 1, &VkCommandBuffer_Buffer_To_Image_Copy);
	VkCommandBuffer_Buffer_To_Image_Copy = VK_NULL_HANDLE;


	// step 6:

	memset((void*)&vkCommandBufferAllocateInfo_Transition_Image_Layout, 0, sizeof(VkCommandBufferAllocateInfo));

	vkCommandBufferAllocateInfo_Transition_Image_Layout.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	vkCommandBufferAllocateInfo_Transition_Image_Layout.pNext = NULL;
	vkCommandBufferAllocateInfo_Transition_Image_Layout.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	vkCommandBufferAllocateInfo_Transition_Image_Layout.commandPool = vkcommandpool;
	vkCommandBufferAllocateInfo_Transition_Image_Layout.commandBufferCount = 1;

	VkCommandBuffer_Transition_Image_Layout = VK_NULL_HANDLE;

	vkresult = vkAllocateCommandBuffers(vkDevice,
		&vkCommandBufferAllocateInfo_Transition_Image_Layout,
		&VkCommandBuffer_Transition_Image_Layout);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkAllocateCommandBuffers() function failed  Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkAllocateCommandBuffers() succeeded\n");
		fflush(gpFile);
	}

	memset((void*)&vkCommandBufferBeginInfo_Transition_Layout, 0, sizeof(VkCommandBufferBeginInfo));

	vkCommandBufferBeginInfo_Transition_Layout.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkCommandBufferBeginInfo_Transition_Layout.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkresult = vkBeginCommandBuffer(VkCommandBuffer_Transition_Image_Layout, &vkCommandBufferBeginInfo_Transition_Layout);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkBeginCommandBuffer() function failed  Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkBeginCommandBuffer() succeeded\n");
		fflush(gpFile);
	}

	 vkPipelineStageFlags_Source = 0;
	 vkPipelineStageFlags_Destination = 0;


	memset((void*)&vkImageMemoryBarrier, 0, sizeof(vkImageMemoryBarrier));

	vkImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	vkImageMemoryBarrier.pNext = NULL;
	vkImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	vkImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	vkImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vkImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vkImageMemoryBarrier.image = vkImage_Texture;
	vkImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	vkImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	vkImageMemoryBarrier.subresourceRange.levelCount = 1;
	vkImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	vkImageMemoryBarrier.subresourceRange.layerCount = 1;

	if (vkImageMemoryBarrier.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && vkImageMemoryBarrier.newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		vkImageMemoryBarrier.srcAccessMask = 0;
		vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkPipelineStageFlags_Source = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		vkPipelineStageFlags_Destination = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (vkImageMemoryBarrier.oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && vkImageMemoryBarrier.newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		vkPipelineStageFlags_Source = VK_PIPELINE_STAGE_TRANSFER_BIT;
		vkPipelineStageFlags_Destination = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		fprintf(gpFile, "createTexture() : unspported texture layout transitions for step 6 : \n");
		vkresult = VK_ERROR_INITIALIZATION_FAILED;
		return vkresult;
	}

	vkCmdPipelineBarrier(VkCommandBuffer_Transition_Image_Layout, vkPipelineStageFlags_Source, vkPipelineStageFlags_Destination, 0, 0, NULL, 0, NULL, 1, &vkImageMemoryBarrier);

	vkresult = vkEndCommandBuffer(VkCommandBuffer_Transition_Image_Layout);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkEndCommandBuffer() function failed  Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkEndCommandBuffer() succeeded\n");
		fflush(gpFile);
	}

	memset(&VkSubmitInfo_Transition_Image_Layout, 0, sizeof(VkSubmitInfo));

	VkSubmitInfo_Transition_Image_Layout.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSubmitInfo_Transition_Image_Layout.pNext = NULL;
	VkSubmitInfo_Transition_Image_Layout.commandBufferCount = 1;
	VkSubmitInfo_Transition_Image_Layout.pCommandBuffers = &VkCommandBuffer_Transition_Image_Layout;

	vkresult = vkQueueSubmit(vkQueue, 1, &VkSubmitInfo_Transition_Image_Layout, VK_NULL_HANDLE);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkQueueSubmit() function failed  Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkQueueSubmit() succeeded\n");
		fflush(gpFile);
	}

	// Wait for the queue to finish the operation
	vkresult = vkQueueWaitIdle(vkQueue);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkQueueWaitIdle() function failed  Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkQueueWaitIdle() succeeded\n");
		fflush(gpFile);
	}

	vkFreeCommandBuffers(vkDevice, vkcommandpool, 1, &VkCommandBuffer_Transition_Image_Layout);
	VkCommandBuffer_Transition_Image_Layout = VK_NULL_HANDLE;

	// step 7  :

	if (vkBuffer_StaggingBuffer)
	{
		vkDestroyBuffer(vkDevice, vkBuffer_StaggingBuffer, NULL);
		vkBuffer_StaggingBuffer = VK_NULL_HANDLE;
	}

	if (VkDeviceMemory_StaggingBuffer)
	{
		vkFreeMemory(vkDevice, VkDeviceMemory_StaggingBuffer, NULL);
		VkDeviceMemory_StaggingBuffer = VK_NULL_HANDLE;
	}


	// stepp 8 :  createimageview for texture

	// crateImageView For above image view
	VkImageViewCreateInfo vkImageViewCreateInfo;
	memset((void*)&vkImageViewCreateInfo, 0, sizeof(VkImageViewCreateInfo));

	vkImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	vkImageViewCreateInfo.pNext = NULL;
	vkImageViewCreateInfo.flags = 0;
	vkImageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	vkImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	vkImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	vkImageViewCreateInfo.subresourceRange.levelCount = 1;
	vkImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	vkImageViewCreateInfo.subresourceRange.layerCount = 1;
	vkImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	vkImageViewCreateInfo.image = vkImage_Texture;

	vkresult = vkCreateImageView(vkDevice, &vkImageViewCreateInfo, NULL, &vkImageView_Texture);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : VkCreateImageView() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : VkCreateImageView() succeeded.\n");
		fflush(gpFile);
	}

	// step 9 :

	VkSamplerCreateInfo vkSamplerCreateInfo;
	memset(&vkSamplerCreateInfo, 0, sizeof(VkSamplerCreateInfo));

	vkSamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	vkSamplerCreateInfo.pNext = NULL;
	vkSamplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	vkSamplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	vkSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	vkSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	vkSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	vkSamplerCreateInfo.anisotropyEnable = VK_FALSE;
	vkSamplerCreateInfo.maxAnisotropy = 16;
	vkSamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	vkSamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	vkSamplerCreateInfo.compareEnable = VK_FALSE;   
	vkSamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	vkSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	vkresult = vkCreateSampler(vkDevice, &vkSamplerCreateInfo, nullptr, &vkSampler_Texture);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkCreateSampler() failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkCreateSampler() succeeded.\n");
		fflush(gpFile);
	}

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

	memset((void*)&uniformData_Pyramid, 0, sizeof(UniformData));

	vkresult = vkCreateBuffer(vkDevice, &vkBufferCreateInfo, NULL, &uniformData_Pyramid.vkBuffer);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createUniformBuffer() : vkCreateBuffer() function failed  for traingle Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createUniformBuffer() : vkCreateBuffer() succeeded  for traingle\n");
	}

	VkMemoryRequirements vkMemoryRequirements;
	memset((void*)&vkMemoryRequirements, 0, sizeof(VkMemoryRequirements));

	vkGetBufferMemoryRequirements(vkDevice, uniformData_Pyramid.vkBuffer, &vkMemoryRequirements);

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

	vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, NULL, &uniformData_Pyramid.vkDeviceMemory);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createUniformBuffer() : vkAllocateMemory() function failed  for traingle Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createUniformBuffer() : vkAllocateMemory() succeeded  for traingle\n");
	}

	vkresult = vkBindBufferMemory(vkDevice, uniformData_Pyramid.vkBuffer, uniformData_Pyramid.vkDeviceMemory, 0);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createUniformBuffer() : vkBindBufferMemory() function failed  for traingle Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createUniformBuffer() : vkBindBufferMemory() succeeded  for traingle\n");
		fflush(gpFile);
	}

	// RECTANGLE
	memset((void*)&uniformData_Cube, 0, sizeof(UniformData));

	vkresult = vkCreateBuffer(vkDevice, &vkBufferCreateInfo, NULL, &uniformData_Cube.vkBuffer);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createUniformBuffer() : vkCreateBuffer() function failed  for reactangle Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createUniformBuffer() : vkCreateBuffer() succeeded  for reactangle\n");
	}

	memset((void*)&vkMemoryRequirements, 0, sizeof(VkMemoryRequirements));

	vkGetBufferMemoryRequirements(vkDevice, uniformData_Cube.vkBuffer, &vkMemoryRequirements);

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

	vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, NULL, &uniformData_Cube.vkDeviceMemory);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createUniformBuffer() : vkAllocateMemory() function failed  for reactangle Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createUniformBuffer() : vkAllocateMemory() succeeded  for reactangle\n");
	}

	vkresult = vkBindBufferMemory(vkDevice, uniformData_Cube.vkBuffer, uniformData_Cube.vkDeviceMemory, 0);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createUniformBuffer() : vkBindBufferMemory() function failed  for reactangle Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createUniformBuffer() : vkBindBufferMemory() succeeded  for reactangle\n");
	}

	// call updateUnifomBuffer
	vkresult = updateUniformbuffer();
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

VkResult updateUniformbuffer(void)
{
	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	// code
	MyUniformData myUniformData;
	memset((void*)&myUniformData, 0, sizeof(MyUniformData));

	// update matrices
	myUniformData.modelMatrix = glm::mat4(1.0);

	glm::mat4 translationMatrix = glm::mat4(1.0);

	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, -5.0f));

	glm::mat4 rotationMatrix = glm::mat4(1.0);

	rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle_1), glm::vec3(0.0f, 1.0f, 0.0f));

	myUniformData.modelMatrix = translationMatrix * rotationMatrix;

	myUniformData.viewMatrix = glm::mat4(1.0);

	glm::mat4 perspectiveProjectionMatrix = glm::mat4(1.0);

	perspectiveProjectionMatrix = glm::perspective(glm::radians(45.0f), float(winWidth) / float(winHeight), 0.1f, 100.0f);

	perspectiveProjectionMatrix[1][1] = perspectiveProjectionMatrix[1][1] * (-1.0f);

	myUniformData.projectionMatrix = perspectiveProjectionMatrix;

	// map unifrom buffer

	void* data = NULL;

	vkresult = vkMapMemory(vkDevice, uniformData_Pyramid.vkDeviceMemory, 0, sizeof(MyUniformData), 0, &data);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "updateUniformbuffer() : vkMapMemory() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}

	// actual memory mapped
	memcpy(data, &myUniformData, sizeof(myUniformData));

	vkUnmapMemory(vkDevice, uniformData_Pyramid.vkDeviceMemory);

	// update matrices
	myUniformData.modelMatrix = glm::mat4(1.0);

	translationMatrix = glm::mat4(1.0);

	translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.0f, -5.0f));

	glm::mat4 rotationMatrix_X = glm::mat4(1.0);

	rotationMatrix_X = glm::rotate(glm::mat4(1.0f), glm::radians(angle_1), glm::vec3(1.0f, 0.0f, 0.0f)); // x axis rotation

	glm::mat4 rotationMatrix_Y = glm::mat4(1.0);

	rotationMatrix_Y = glm::rotate(glm::mat4(1.0f), glm::radians(angle_1), glm::vec3(0.0f, 1.0f, 0.0f)); // Y axis rotation

	glm::mat4 rotationMatrix_Z = glm::mat4(1.0);

	rotationMatrix_Z = glm::rotate(glm::mat4(1.0f), glm::radians(angle_1), glm::vec3(0.0f, 0.0f, 1.0f)); // Z axis rotation

	rotationMatrix = rotationMatrix_X * rotationMatrix_Y * rotationMatrix_Z;

	glm::mat4 scaleMatrix = glm::mat4(1.0);

	scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.75f, 0.75f, 0.75f)); // Y axis rotation

	myUniformData.modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;

	myUniformData.viewMatrix = glm::mat4(1.0);

	perspectiveProjectionMatrix = glm::mat4(1.0);

	perspectiveProjectionMatrix = glm::perspective(glm::radians(45.0f), float(winWidth) / float(winHeight), 0.1f, 100.0f);

	perspectiveProjectionMatrix[1][1] = perspectiveProjectionMatrix[1][1] * (-1.0f);

	myUniformData.projectionMatrix = perspectiveProjectionMatrix;

	// map unifrom buffer

	data = NULL;

	vkresult = vkMapMemory(vkDevice, uniformData_Cube.vkDeviceMemory, 0, sizeof(MyUniformData), 0, &data);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "updateUniformbuffer() : vkMapMemory() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}

	// actual memory mapped
	memcpy(data, &myUniformData, sizeof(myUniformData));

	vkUnmapMemory(vkDevice, uniformData_Cube.vkDeviceMemory);

	return vkresult;
}


VkResult createShaders(void)
{
	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	//code

	// for vertex shaders
	const char* szfileName = "Shader.vert.spv";

	FILE* fp = NULL;

	size_t size;

	fp = fopen(szfileName, "rb");

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
	}


	if (shaderData)
	{
		free(shaderData);
		shaderData = NULL;
	}

	fprintf(gpFile, "Vertexshader Module sucessfully created\n");

	// for fragmnt shader

	szfileName = "Shader.frag.spv";

	fp = NULL;

	size = 0;

	fp = fopen(szfileName, "rb");

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
	vkShaderModuleCreateInfo.flags = 0; // future use can be haapend but now zero
	vkShaderModuleCreateInfo.codeSize = size;
	vkShaderModuleCreateInfo.pCode = (uint32_t*)shaderData;

	vkresult = vkCreateShaderModule(vkDevice, &vkShaderModuleCreateInfo, NULL, &vkShaderModule_fragment_shader);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createShaders() : vkCreateShaderModule() function failed. for fragment shader Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createShaders() : vkCreateShaderModule() succeeded for fragment shader.\n");
	}


	if (shaderData)
	{
		free(shaderData);
		shaderData = NULL;
	}

	fprintf(gpFile, "fragment Module sucessfully created\n");

	return vkresult;
}

VkResult createDiscriptorSetLayout(void)
{
	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	// initialise descriptorsetBinding
	VkDescriptorSetLayoutBinding vkdescriptorSetLayoutBinding_Array[2];
	memset((void*)vkdescriptorSetLayoutBinding_Array, 0, sizeof(VkDescriptorSetLayoutBinding) * _ARRAYSIZE(vkdescriptorSetLayoutBinding_Array));

	// for mvp uniform
	vkdescriptorSetLayoutBinding_Array[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vkdescriptorSetLayoutBinding_Array[0].binding = 0;  // this 0 is related with the binding  = 0 of vertex shader
	vkdescriptorSetLayoutBinding_Array[0].descriptorCount = 1;
	vkdescriptorSetLayoutBinding_Array[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	vkdescriptorSetLayoutBinding_Array[0].pImmutableSamplers = NULL;

	
	// for texture image and sampler
	vkdescriptorSetLayoutBinding_Array[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	vkdescriptorSetLayoutBinding_Array[1].binding = 1;  
	vkdescriptorSetLayoutBinding_Array[1].descriptorCount = 1;
	vkdescriptorSetLayoutBinding_Array[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	vkdescriptorSetLayoutBinding_Array[1].pImmutableSamplers = NULL;


	VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo;
	memset((void*)&vkDescriptorSetLayoutCreateInfo, 0, sizeof(VkDescriptorSetLayoutCreateInfo));

;	vkDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	vkDescriptorSetLayoutCreateInfo.pNext = NULL;
	vkDescriptorSetLayoutCreateInfo.flags = 0;
	vkDescriptorSetLayoutCreateInfo.bindingCount = _ARRAYSIZE(vkdescriptorSetLayoutBinding_Array);
	vkDescriptorSetLayoutCreateInfo.pBindings =vkdescriptorSetLayoutBinding_Array;  // pbinding array is actually array VkDiscriptorSetLayoutBinding having 5 members  1) uint32_t binding : an integer value where you want to bind descriptor set  
	                                                                                                                                    // 2) VkDiscriptorSetType DiscriptorType : which type of descriptor
	                                                                                                                                    // 3) uint32_t discriptorCount : How many descriptor
	                                                                                                                                    // 4) VkShaderStageFalgs stageFalgs : konty shader mdhe vaprycha aahe?
	                                                                                                                                    // 5) const VkSamplaar* pImmutableSamplers : jevha aaplyakde sampler aahe permanant

	vkresult = vkCreateDescriptorSetLayout(vkDevice, &vkDescriptorSetLayoutCreateInfo, NULL, &vkDescriptorSetLayout);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createDescriptorSetLayout() : vkCreateDescriptorSetLayout() failed Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createDescriptorSetLayout() : vkCreateDescriptorSetLayout() succeeded.\n");
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
		fprintf(gpFile, "createPiplineLayout() : vkCreatePipelineLayout() failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createPiplineLayout() : vkCreatePipelineLayout() succeeded.\n");
	}

	return vkresult;
}

VkResult createDescriptorpool(void)
{
	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	VkDescriptorPoolSize vkdescriptorPoolSize_Array[2];
	memset((void*)vkdescriptorPoolSize_Array, 0, sizeof(VkDescriptorPoolSize) * _ARRAYSIZE(vkdescriptorPoolSize_Array));

	// for mvp ubo
	vkdescriptorPoolSize_Array[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vkdescriptorPoolSize_Array[0].descriptorCount = 2;

	// for texture sampler
	vkdescriptorPoolSize_Array[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	vkdescriptorPoolSize_Array[1].descriptorCount = 2;


	// create the pool 
	VkDescriptorPoolCreateInfo vkdescriptorPoolCreateInfo;
	memset((void*)&vkdescriptorPoolCreateInfo, 0, sizeof(VkDescriptorPoolCreateInfo));

	vkdescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	vkdescriptorPoolCreateInfo.pNext = NULL;
	vkdescriptorPoolCreateInfo.flags = 0;
	vkdescriptorPoolCreateInfo.poolSizeCount = _ARRAYSIZE(vkdescriptorPoolSize_Array);
	vkdescriptorPoolCreateInfo.pPoolSizes = vkdescriptorPoolSize_Array;
	vkdescriptorPoolCreateInfo.maxSets = 2;

	vkresult = vkCreateDescriptorPool(vkDevice, &vkdescriptorPoolCreateInfo, NULL, &vkDescriptorPool);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createDescriptorpool() : vkCreateDescriptorPool() failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createDescriptorpool() : vkCreateDescriptorPool() succeeded.\n");
	}


	return vkresult;
}

VkResult createDescriptorSet(void)
{
	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	// code
	VkDescriptorSetAllocateInfo vkDescriptorSetAllocateInfo;
	memset((void*)&vkDescriptorSetAllocateInfo, 0, sizeof(VkDescriptorSetAllocateInfo));

	vkDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	vkDescriptorSetAllocateInfo.pNext = NULL;
	vkDescriptorSetAllocateInfo.descriptorPool = vkDescriptorPool;
	vkDescriptorSetAllocateInfo.descriptorSetCount = 1;
	vkDescriptorSetAllocateInfo.pSetLayouts = &vkDescriptorSetLayout;

	// ==============================
	// Pyramid Descriptor Set
	// ==============================
	vkresult = vkAllocateDescriptorSets(vkDevice, &vkDescriptorSetAllocateInfo, &vkDescriptorSet_Pyramid);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createDescriptorSet() : vkAllocateDescriptorSets() failed for Pyramid. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createDescriptorSet() : vkAllocateDescriptorSets() succeeded for Pyramid.\n");
	}

	// Uniform buffer info
	VkDescriptorBufferInfo vkdescriptorBufferInfo;
	memset((void*)&vkdescriptorBufferInfo, 0, sizeof(VkDescriptorBufferInfo));
	vkdescriptorBufferInfo.buffer = uniformData_Pyramid.vkBuffer;
	vkdescriptorBufferInfo.offset = 0;
	vkdescriptorBufferInfo.range = sizeof(MyUniformData);

	// Image + sampler info
	VkDescriptorImageInfo vkDescriptorImageInfo;
	memset((void*)&vkDescriptorImageInfo, 0, sizeof(VkDescriptorImageInfo));
	vkDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	vkDescriptorImageInfo.sampler = vkSampler_Texture_Pyramid;   // <-- Pyramid texture sampler
	vkDescriptorImageInfo.imageView = vkImageView_Texture_Pyramid; // <-- Pyramid image view

	// Write descriptors
	VkWriteDescriptorSet vkWriteDescriptorSet_Array[2];
	memset((void*)vkWriteDescriptorSet_Array, 0, sizeof(vkWriteDescriptorSet_Array));

	vkWriteDescriptorSet_Array[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	vkWriteDescriptorSet_Array[0].dstSet = vkDescriptorSet_Pyramid;
	vkWriteDescriptorSet_Array[0].dstBinding = 0;
	vkWriteDescriptorSet_Array[0].descriptorCount = 1;
	vkWriteDescriptorSet_Array[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vkWriteDescriptorSet_Array[0].pBufferInfo = &vkdescriptorBufferInfo;

	vkWriteDescriptorSet_Array[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	vkWriteDescriptorSet_Array[1].dstSet = vkDescriptorSet_Pyramid;
	vkWriteDescriptorSet_Array[1].dstBinding = 1;
	vkWriteDescriptorSet_Array[1].descriptorCount = 1;
	vkWriteDescriptorSet_Array[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	vkWriteDescriptorSet_Array[1].pImageInfo = &vkDescriptorImageInfo;

	vkUpdateDescriptorSets(vkDevice, _ARRAYSIZE(vkWriteDescriptorSet_Array), vkWriteDescriptorSet_Array, 0, NULL);
	fprintf(gpFile, "vkUpdateDescriptorSets() succeeded for Pyramid\n");


	// ==============================
	// Cube Descriptor Set
	// ==============================
	vkresult = vkAllocateDescriptorSets(vkDevice, &vkDescriptorSetAllocateInfo, &vkDescriptorSet_Cube);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createDescriptorSet() : vkAllocateDescriptorSets() failed for Cube. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createDescriptorSet() : vkAllocateDescriptorSets() succeeded for Cube.\n");
	}

	memset((void*)&vkdescriptorBufferInfo, 0, sizeof(VkDescriptorBufferInfo));
	vkdescriptorBufferInfo.buffer = uniformData_Cube.vkBuffer;
	vkdescriptorBufferInfo.offset = 0;
	vkdescriptorBufferInfo.range = sizeof(MyUniformData);

	memset((void*)&vkDescriptorImageInfo, 0, sizeof(VkDescriptorImageInfo));
	vkDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	vkDescriptorImageInfo.sampler = vkSampler_Texture_Cube;    // <-- Cube texture sampler
	vkDescriptorImageInfo.imageView = vkImageView_Texture_Cube; // <-- Cube image view

	memset((void*)vkWriteDescriptorSet_Array, 0, sizeof(vkWriteDescriptorSet_Array));

	vkWriteDescriptorSet_Array[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	vkWriteDescriptorSet_Array[0].dstSet = vkDescriptorSet_Cube;
	vkWriteDescriptorSet_Array[0].dstBinding = 0;
	vkWriteDescriptorSet_Array[0].descriptorCount = 1;
	vkWriteDescriptorSet_Array[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vkWriteDescriptorSet_Array[0].pBufferInfo = &vkdescriptorBufferInfo;

	vkWriteDescriptorSet_Array[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	vkWriteDescriptorSet_Array[1].dstSet = vkDescriptorSet_Cube;
	vkWriteDescriptorSet_Array[1].dstBinding = 1;
	vkWriteDescriptorSet_Array[1].descriptorCount = 1;
	vkWriteDescriptorSet_Array[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	vkWriteDescriptorSet_Array[1].pImageInfo = &vkDescriptorImageInfo;

	vkUpdateDescriptorSets(vkDevice, _ARRAYSIZE(vkWriteDescriptorSet_Array), vkWriteDescriptorSet_Array, 0, NULL);
	fprintf(gpFile, "vkUpdateDescriptorSets() succeeded for Cube\n");

	return vkresult;
}



VkResult createRenderPass(void)
{
	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	VkAttachmentDescription vkAttachmentDescription_array[2];

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

	// for depth
	vkAttachmentDescription_array[1].flags = 0;
	vkAttachmentDescription_array[1].format = vkFormat_Depth;
	vkAttachmentDescription_array[1].samples = VK_SAMPLE_COUNT_1_BIT;   //No multi sampling so 1 bit is enough
	vkAttachmentDescription_array[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	vkAttachmentDescription_array[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;				//color attachment related
	vkAttachmentDescription_array[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;			//This is for both Depth and Stencil although it is for stencil
	vkAttachmentDescription_array[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	vkAttachmentDescription_array[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;													//Image data when in and when out
	vkAttachmentDescription_array[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// for color
	//Declare and initialize vkAttachmentReference structure
	VkAttachmentReference vkAttachmentRefrence_Color;
	memset((void*)&vkAttachmentRefrence_Color, 0, sizeof(VkAttachmentReference));
	vkAttachmentRefrence_Color.attachment = 0;			//This means above given array 0th Ataachment reference, O means it is the index number
	vkAttachmentRefrence_Color.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;		//Tis means this attachment i can use it color attachment so keep it optimal

	// for depth atttachment
	VkAttachmentReference vkAttachmentRefrence_Depth;
	memset((void*)&vkAttachmentRefrence_Depth, 0, sizeof(VkAttachmentReference));
	vkAttachmentRefrence_Depth.attachment = 1;			//This means above given array 0th Ataachment reference, O means it is the index number
	vkAttachmentRefrence_Depth.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//Step 3 : Declare and Initialize vkSubpassDescription

	VkSubpassDescription vkSubpassDesciption;
	memset((void*)&vkSubpassDesciption, 0, sizeof(VkSubpassDescription));
	vkSubpassDesciption.flags = 0;
	vkSubpassDesciption.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	vkSubpassDesciption.inputAttachmentCount = 0;
	vkSubpassDesciption.pInputAttachments = NULL;
	vkSubpassDesciption.colorAttachmentCount = 1;
	vkSubpassDesciption.pColorAttachments = &vkAttachmentRefrence_Color;
	vkSubpassDesciption.pResolveAttachments = NULL;
	vkSubpassDesciption.pDepthStencilAttachment = &vkAttachmentRefrence_Depth;
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
		fprintf(gpFile, "createRenderPass() : vkCreateRenderPass() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createRenderPass() : vkCreateRenderPass() succeeded.\n");
	}

	return vkresult;
}

VkResult createPipline(void)
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

	// for texcoord
	vkVertexInputBindingDescription_Array[1].binding = 1; // corresponding to location  = 1 in vertex shader
	vkVertexInputBindingDescription_Array[1].stride = sizeof(float) * 2;
	vkVertexInputBindingDescription_Array[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription vkVertexInputAttributeDescription_Array[2];
	memset((void*)vkVertexInputAttributeDescription_Array, 0, sizeof(VkVertexInputAttributeDescription) * _ARRAYSIZE(vkVertexInputAttributeDescription_Array));

	// for position
	vkVertexInputAttributeDescription_Array[0].binding = 0;
	vkVertexInputAttributeDescription_Array[0].location = 0;
	vkVertexInputAttributeDescription_Array[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	vkVertexInputAttributeDescription_Array[0].offset = 0;

	// for texcoord
	vkVertexInputAttributeDescription_Array[1].binding = 1;
	vkVertexInputAttributeDescription_Array[1].location = 1;
	vkVertexInputAttributeDescription_Array[1].format = VK_FORMAT_R32G32_SFLOAT;
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
	vkPipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
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

	VkPipelineDepthStencilStateCreateInfo vkPipelineDepthStencilStateCreateInfo;
	memset((void*)&vkPipelineDepthStencilStateCreateInfo, 0, sizeof(VkPipelineDepthStencilStateCreateInfo));

	vkPipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
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

VkResult createframeBuffers(void)
{
	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	vkFramebuffer_Array = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * swapchainImageCount);



	for (uint32_t i = 0; i < swapchainImageCount; i++)
	{
		// Declare array of VkImageView
		VkImageView vkImageView_Attchment_Array[2];
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


		vkImageView_Attchment_Array[0] = SwapchainImageView_Array[i];

		vkImageView_Attchment_Array[1] = vkImageView_Depth;

		vkresult = vkCreateFramebuffer(vkDevice, &vkFramebufferCreateInfo, NULL, &vkFramebuffer_Array[i]);
		if (vkresult != VK_SUCCESS)
		{
			fprintf(gpFile, "createframeBuffers() : vkCreateFramebuffer() function failed. Error Code: (%d)\n", vkresult);
			return vkresult;
		}
		else
		{
			fprintf(gpFile, "createframeBuffers() : vkCreateFramebuffer() succeeded.\n");
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
		fprintf(gpFile, "createSemaphores() : vkCreateSemaphore() function failed for backbuffer. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createSemaphores() : vkCreateSemaphore() succeeded for backbuffer.\n");
	}

	vkresult = vkCreateSemaphore(vkDevice, &vkSemaphoreCreateInfo, NULL, &vkSemaphore_rendercomplete);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createSemaphores() : vkCreateSemaphore() function failed for rendercomplete. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createSemaphores() : vkCreateSemaphore() succeeded for rendercomplete.\n");
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
			fprintf(gpFile, "createFences() : vkCreateFence() function failed. Error Code: (%d)\n", vkresult);
			return vkresult;
		}
		else
		{
			fprintf(gpFile, "createFences() : vkCreateFence() succeeded.\n");
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
			fprintf(gpFile, "buildCommandBuffers() : vkResetCommandBuffer() failed at index [%d]. Error Code: (%d)\n", i, vkresult);
			return vkresult;
		}
		else
		{
			fprintf(gpFile, "buildCommandBuffers() : vkResetCommandBuffer() succeeded at index [%d].\n", i);
		}

		VkCommandBufferBeginInfo vkCommandBufferBeginInfo;
		memset((void*)&vkCommandBufferBeginInfo, 0, sizeof(VkCommandBufferBeginInfo));

		vkCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkCommandBufferBeginInfo.pNext = NULL;
		vkCommandBufferBeginInfo.flags = 0;

		vkresult = vkBeginCommandBuffer(vkCommandBuffer_Array[i], &vkCommandBufferBeginInfo);
		if (vkresult != VK_SUCCESS)
		{
			fprintf(gpFile, "buildCommandBuffers() : VkBeginCommandBuffer() failed at index [%d]. Error Code: (%d)\n", i, vkresult);
			return vkresult;
		}
		else
		{
			fprintf(gpFile, "buildCommandBuffers() : VkBeginCommandBuffer() succeeded at index [%d].\n", i);
		}

		// set clear values
		VkClearValue vkClearValue_Array[2];
		memset((void*)vkClearValue_Array, 0, sizeof(VkClearValue) * _ARRAYSIZE(vkClearValue_Array));

		vkClearValue_Array[0].color = vkClearColorValue;
		vkClearValue_Array[1].depthStencil.depth = 1.0f;
		vkClearValue_Array[1].depthStencil.stencil = 0;


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
			&vkDescriptorSet_Pyramid,
			0,              // dynamicOffsetCount
			NULL            // pDynamicOffsets
		);


		// bind vertex position vertex buffer
		VkDeviceSize vkDeviceSize_Offest_Position[1];
		memset((void*)vkDeviceSize_Offest_Position, 0, sizeof(VkDeviceSize) * ARRAYSIZE(vkDeviceSize_Offest_Position));

		vkCmdBindVertexBuffers(vkCommandBuffer_Array[i], 0, 1, &vertexData_Position_Pyramid.vkBuffer, vkDeviceSize_Offest_Position);

		// bind vertex color vertex buffer
		VkDeviceSize vkDeviceSize_Offest_Texcoord[1];
		memset((void*)vkDeviceSize_Offest_Texcoord, 0, sizeof(VkDeviceSize) * ARRAYSIZE(vkDeviceSize_Offest_Texcoord));

		vkCmdBindVertexBuffers(vkCommandBuffer_Array[i], 1, 1, &vertexData_Texcoord_Pyramid.vkBuffer, vkDeviceSize_Offest_Texcoord);

		// Here we should call Vulkan drawing functions
		vkCmdDraw(vkCommandBuffer_Array[i], 12, 1, 0, 0);

		// CUBE
		// bind descriptor set to pipline
		vkCmdBindDescriptorSets(
			vkCommandBuffer_Array[i],
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			vkPipelineLayout, // must match the one used to create pipeline
			0,              // firstSet
			1,              // descriptorSetCount
			&vkDescriptorSet_Cube,
			0,              // dynamicOffsetCount
			NULL            // pDynamicOffsets
		);


		// bind with vertex buffer
		memset((void*)vkDeviceSize_Offest_Position, 0, sizeof(VkDeviceSize) * ARRAYSIZE(vkDeviceSize_Offest_Position));

		vkCmdBindVertexBuffers(vkCommandBuffer_Array[i], 0, 1, &vertexData_Position_Cube.vkBuffer, vkDeviceSize_Offest_Position);

		// bind textcord vertex buffer
		memset((void*)vkDeviceSize_Offest_Texcoord, 0, sizeof(VkDeviceSize) * ARRAYSIZE(vkDeviceSize_Offest_Texcoord));

		vkCmdBindVertexBuffers(vkCommandBuffer_Array[i], 1, 1, &vertexData_Texcoord_Cube.vkBuffer, vkDeviceSize_Offest_Texcoord);

		// Here we should call Vulkan drawing functions
		vkCmdDraw(vkCommandBuffer_Array[i], 36, 1, 0, 0);

		// End the render pass
		vkCmdEndRenderPass(vkCommandBuffer_Array[i]);

		// End command buffer recording
		vkresult = vkEndCommandBuffer(vkCommandBuffer_Array[i]);
		if (vkresult != VK_SUCCESS)
		{
			fprintf(gpFile, "buildCommandBuffers() : vkEndCommandBuffer() failed at index [%d]. Error Code: (%d)\n", i, vkresult);
			return vkresult;
		}
		else
		{
			fprintf(gpFile, "buildCommandBuffers() : vkEndCommandBuffer() succeeded at index [%d].\n", i);
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
	fprintf(gpFile, "PRS_Validation : debugReportCallback() :  %s (%d) = %s\n", pLayerPrefix, messageCode, pMessage);

	// Returning VK_FALSE means continue execution (don't abort)
	return VK_FALSE;
}





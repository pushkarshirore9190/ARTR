// windows header files
#include<windows.h>
#include"VK.h"
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h" // header file for texture
#include"Teapot.h"
#include"Sphere.h"
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
#pragma comment(lib, "Sphere.lib")


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
VkFormat vkFormat_color_fbo = VK_FORMAT_UNDEFINED;

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
VkFormat vkFormat_Depth_fbo = VK_FORMAT_UNDEFINED;

VkImage vkImage_Depth_fbo = VK_NULL_HANDLE;

VkDeviceMemory vkDeviceMemory_Depth_fbo = VK_NULL_HANDLE;

VkImageView vkImageView_Depth_fbo = VK_NULL_HANDLE;



// command pool 
VkCommandPool vkcommandpool = VK_NULL_HANDLE;

// command buffer
VkCommandBuffer vkCommandBuffer_fbo = VK_NULL_HANDLE;

// vertex buffers
float* pPositions = NULL;
float* pNormal = NULL;
float* pTexcoords = NULL;
unsigned int* pElements = NULL;

unsigned int numFaceIndices = 0;
unsigned int numElements = 0;
unsigned int numVerts = 0;

// render pass
VkRenderPass vkRenderpass_fbo = VK_NULL_HANDLE;

// frameBuffer
VkFramebuffer vkFramebuffer_fbo = NULL;

// Semaphore
VkSemaphore vkSemaphore_fbo = VK_NULL_HANDLE;


// Clear color values
VkClearColorValue vkClearColorValue_fbo;

VkClearDepthStencilValue vkClearDepthStencilValue_fbo;

BOOL bInitialised_fbo = FALSE;

uint32_t currentImageIndex = UINT32_MAX;

// Validation
BOOL bValidation = TRUE;

uint32_t enabledValidationLayerCount = 0;

const char* enabledValidationLayerNames_Array[1];  // for VK_LAYER_CRONOS_validation

VkDebugReportCallbackEXT vkDebugReportCallbackEXT = VK_NULL_HANDLE;

PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT_fnptr = NULL;

// Pipline
VkViewport vkViewport_fbo;

VkRect2D vkRect2D_Scissor_fbo;

VkPipeline vkPipeline_fbo = VK_NULL_HANDLE;


// VertexBuffer Related Variable
typedef struct
{
	VkBuffer vkBuffer;
	VkDeviceMemory vkDeviceMemory;

} VertexData;

// poaition
VertexData vertexData_Position_fbo;

VertexData vertexData_Normal_fbo;

VertexData vertexData_Texcoord_fbo;

VertexData vertexData_Index_fbo;

// uniform related declarations
struct MyUniformData_fbo
{
	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	// light related uniform
	float lightAmbient[4];
	float lightDiffuse[4];
	float lightSpecular[4];
	float lightPosition[4];

	// material related uniform
	float materialAmbient[4];
	float materialDiffuse[4];
	float materialSpecular[4];
	float materialShininess;

	// keypressed related uniform
	unsigned int lKeyIsPressed;
};

struct UniformData
{
	VkBuffer vkBuffer;
	VkDeviceMemory vkDeviceMemory;
};

UniformData uniformData_fbo;

// shader related variables
VkShaderModule vkShaderModule_vertex_shader_fbo = VK_NULL_HANDLE;

VkShaderModule vkShaderModule_fragment_shader_fbo = VK_NULL_HANDLE;

// discrptorsetlayout object
VkDescriptorSetLayout vkDescriptorSetLayout_fbo = VK_NULL_HANDLE;


// Pipeline Layout Object
VkPipeline vkPipeline_fboLayout_fbo = VK_NULL_HANDLE;

// Descriptor pool
VkDescriptorPool vkDescriptorPool_fbo = VK_NULL_HANDLE;

// Descriptor set
VkDescriptorSet vkDescriptorSet_fbo = VK_NULL_HANDLE;

float angleTeapot = 0.0f;
BOOL  bAnimate = FALSE;

// texture related variables
VkImage vkImage_Texture_fbo = VK_NULL_HANDLE;

VkDeviceMemory vkDeviceMemory_Texture_fbo = VK_NULL_HANDLE;

VkImageView vkImageView_Texture_fbo = VK_NULL_HANDLE;

VkSampler vkSampler_Texture_fbo = VK_NULL_HANDLE;

BOOL bLight = FALSE;


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
						if(bAnimate == TRUE)
						{
							update();
						}
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
	VkResult resize_fbo(int, int);

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
			resize_fbo(LOWORD(lParam), HIWORD(lParam));
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
		case 'A':
		case 'a':
			if (bAnimate == FALSE)
			{
				bAnimate = TRUE;
			}
			else
			{
				bAnimate = FALSE;
			}
			break;
		case 'L':
		case 'l':
			if (bLight == FALSE)
			{
				bLight = TRUE;
			}
			else
			{
				bLight = FALSE;
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
	VkResult createImagesAndImageViews_fbo(void);
	VkResult createCommandPool(void);
	VkResult createCommandBuffers_fbo(void);
	void addTriangle(float [3][3], float [3][3], float[3][2]);
	VkResult createVertexBuffer(void);
	VkResult createIndexBuffer(void);
	VkResult createTexture(const char*);
	VkResult createUniformBuffer(void);
	VkResult createShaders(void);
	VkResult createDiscriptorSetLayout(void);
	VkResult createPiplineLayout_fbo(void);
	VkResult createDescriptorpool(void);
	VkResult createDescriptorSet(void);
	VkResult createRenderPass_fbo(void);
	VkResult createPipline(void);
	VkResult createframeBuffers_fbo(void);
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

	vkresult = createImagesAndImageViews_fbo();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createImagesAndImageViews_fbo() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createImagesAndImageViews_fbo() succeeded\n");
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

	vkresult = createCommandBuffers_fbo();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createCommandBuffers_fbo() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createCommandBuffers_fbo() succeeded\n");
	}

	// calculate no of facesindices
	numFaceIndices = sizeof(face_indicies) / sizeof(face_indicies[0]);

	// Position
	pPositions = (float*)malloc(numFaceIndices * 3 * sizeof(float)); // 3 vertices and each vertex has 3 components x,y,z

	// normals
	pNormal = (float*)malloc(numFaceIndices * 3 * sizeof(float)); // 3 vertices and each vertex has 3 components x,y,z

	// texcoords
	pTexcoords = (float*)malloc(numFaceIndices * 2 * sizeof(float)); // 3 vertices and each vertex has 2 components s,t

	// elements
	pElements = (unsigned int*)malloc(numFaceIndices * 3 * sizeof(unsigned int)); // 3 vertices

	// declare temporary arrays to hold single triangle vertices
	float ver[3][3];
	float norm[3][3];
	float tex[3][2];

	for(int i = 0 ; i <(int)numFaceIndices; i++)
	{
		for(int j = 0 ; j < 3 ; j++)
		{
			ver[j][0] = vertices[face_indicies[i][j+0]] [0];
			ver[j][1] = vertices[face_indicies[i][j+0]] [1];
			ver[j][2] = vertices[face_indicies[i][j+0]] [2];

			norm[j][0] = normals[face_indicies[i][j+3]] [0];
			norm[j][1] = normals[face_indicies[i][j+3]] [1];	
			norm[j][2] = normals[face_indicies[i][j+3]] [2];

			tex[j][0] = textures[face_indicies[i][j+6]] [0];
			tex[j][1] = textures[face_indicies[i][j+6]] [1];

		}

		addTriangle(ver, norm, tex);

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

	vkresult = createIndexBuffer();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createIndexBuffer() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createIndexBuffer() succeeded\n");
	}

	vkresult = createTexture("marble.png");
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createTexture() function failed marble (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createTexture() succeeded for marble \n");
		fflush(gpFile);
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

	vkresult = createPiplineLayout_fbo();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createPiplineLayout_fbo() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createPiplineLayout_fbo() succeeded\n");
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



	vkresult = createRenderPass_fbo();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "initialise() : createRenderPass_fbo() function failed (%d)\n", vkresult);
		return(vkresult);
	}
	else
	{
		fprintf(gpFile, "initialise() : createRenderPass_fbo() succeeded\n");
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

	vkresult = createframeBuffers_fbo();
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
	memset((void*)&vkClearColorValue_fbo, 0, sizeof(VkClearColorValue_fbo));

	vkClearColorValue_fbo.float32[0] = 0.0f;
	vkClearColorValue_fbo.float32[1] = 0.0f;
	vkClearColorValue_fbo.float32[2] = 0.0f;
	vkClearColorValue_fbo.float32[3] = 1.0f;  // analogse to glclear color

	memset((void*)&vkClearDepthStencilValue_fbo, 0, sizeof(VkClearDepthStencilValue_fbo));

	// set defalut clear depth
	vkClearDepthStencilValue_fbo.depth = 1.0f;
	vkClearDepthStencilValue_fbo.stencil = 0;


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

	bInitialised_fbo = TRUE;


	fprintf(gpFile, "******************************************* Initialise comment *****************************\n");

	fprintf(gpFile, "Initialised()  :  Initialisation() Complete Successfully\n");

	return(vkresult);
}



VkResult resize_fbo(int fbo_width, int fbo_heigth)
{
	// function declaration ;
	VkResult createImagesAndImageViews_fbo(void);
	VkResult createCommandBuffer_fbo(void);
	VkResult createPiplineLayout_fbo(void);
	VkResult createRenderPass_fbo(void);
	VkResult createPipline_fbo(void);
	VkResult createframeBuffer_fbo(void);
	VkResult buildCommandBuffer_fbo(void);

	

	// variable declarations
	VkResult vkresult = VK_SUCCESS;

	// code
	if (fbo_heigth <= 0)
		fbo_heigth = 1;

	// check the bInitialised_fbo variable
	if (bInitialised_fbo == FALSE)
	{
		fprintf(gpFile, "resize_fbo() : Initialisation yet not completed or failed\n");
		vkresult = VK_ERROR_INITIALIZATION_FAILED;
		return vkresult;
	}

	// as recreation of swapchain is needed we are going to repeate many steps of initialise again hence set bInitialised_fbo  =  FALSE again
	bInitialised_fbo = FALSE;

	// set global winwidth and winheight variables
	fbo_width = FBO_WIDTH;
	fbo_heigth = FBO_HEIGHT;


	// check presence of swapchain
	if (vkSwapchainKHR == VK_NULL_HANDLE)
	{
		fprintf(gpFile, "\n resize_fbo() : swapchain is aleady null cannot proceed\n");
		vkresult = VK_ERROR_INITIALIZATION_FAILED;
		return vkresult;

	}


	// destory frame buffer
	if (vkFramebuffer_fbo)
	{
		vkDestroyFramebuffer(vkDevice, vkFramebuffer_fbo, NULL);
		vkFramebuffer_fbo = NULL;

	}

	// free command buffers
	if (vkCommandBuffer_fbo)
	{
		vkFreeCommandBuffers(vkDevice, vkcommandpool, 1, &vkCommandBuffer_fbo);
		vkCommandBuffer_fbo = NULL;
	}

	// destroy pipline
	if (vkPipeline_fbo)
	{
		vkDestroyPipeline(vkDevice, vkPipeline_fbo, NULL);
		vkPipeline_fbo = VK_NULL_HANDLE;
	}

	// destroy pipline layout
	if (vkPipelineLayout_fbo)
	{
		vkDestroyPipelineLayout(vkDevice, vkPipelineLayout_fbo, NULL);
		vkPipelineLayout_fbo = VK_NULL_HANDLE;
	}

	// destroy render pass
	if (vkRenderpass_fbo)
	{
		vkDestroyRenderPass(vkDevice, vkRenderpass_fbo, NULL);
		vkRenderpass_fbo = VK_NULL_HANDLE;
	}

	// Destroy depth image view
	if (vkImageView_Depth_fbo)
	{
		vkDestroyImageView(vkDevice, vkImageView_Depth_fbo, NULL);
		vkImageView_Depth_fbo = VK_NULL_HANDLE;
	}

	// Free memory for depth image
	if (vkDeviceMemory_Depth_fbo)
	{
		vkFreeMemory(vkDevice, vkDeviceMemory_Depth_fbo, NULL);
		vkDeviceMemory_Depth_fbo = VK_NULL_HANDLE;
	}

	// destroy depth image
	if (vkImage_Depth_fbo)
	{
		vkDestroyImage(vkDevice, vkImage_Depth_fbo, NULL);
		vkImage_Depth_fbo = VK_NULL_HANDLE;
	}

	if(vksampler_fbo)
	{
		vkDestroySampler(vkDevice, vksampler_fbo, NULL);
		vksampler_fbo = VK_NULL_HANDLE;
	}

	if(vkImageView_fbo)
	{
		vkDestroyImageView(vkDevice, vkImageView_fbo, NULL);
		vkImageView_fbo = VK_NULL_HANDLE;
	}

	if(vkImage_fbo)
	{
		vkDestroyImage(vkDevice, vkImage_fbo, NULL);
		vkImage_fbo = VK_NULL_HANDLE;
	}

	if(vkDeviceMemory_fbo)
	{
		vkFreeMemory(vkDevice, vkDeviceMemory_fbo, NULL);
		vkDeviceMemory_fbo = VK_NULL_HANDLE;
	}


	////// RECREATE FIR RESIZE_fbo \\\\\\\

	vkresult = createImagesAndImageViews_fbo();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, " resize_fbo() : createImagesAndImageViews_fbo() function failed (%d)\n", vkresult);
		return(vkresult);
	}

	vkresult = createRenderPass_fbo();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, " resize_fbo() : createRenderPass_fbo() function failed (%d)\n", vkresult);
		return(vkresult);
	}

	vkresult = createPiplineLayout_fbo();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, " resize_fbo() : createPiplineLayout_fbo() function failed (%d)\n", vkresult);
		return(vkresult);
	}

	vkresult = createPipline_fbo();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, " resize_fbo() : createPipline() function failed (%d)\n", vkresult);
		return(vkresult);
	}

	vkresult = createframeBuffer_fbo();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, " resize_fbo() : createframeBuffer() function failed (%d)\n", vkresult);
		return(vkresult);
	}

	vkresult = createCommandBuffer_fbo();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, " resize_fbo() : createCommandBuffers_fbo() function failed (%d)\n", vkresult);
		return(vkresult);
	}


	// build commmand buffers
	vkresult = buildCommandBuffer_fbo();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, " resize_fbo() : buildCommandBuffers() function failed (%d)\n", vkresult);
		return(vkresult);
	}


	bInitialised_fbo = TRUE;

	return(vkresult);
}

VkResult display(void)
{

	// function declaration
	VkResult resize_fbo(int, int);
	VkResult updateUniformbuffer(void);
	 
	// variable declarations
	VkResult vkresult = VK_SUCCESS;

	// code

	// if control comes here before initilisation gets completed return false

	if (bInitialised_fbo == FALSE)
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
			resize_fbo(winWidth, winHeight);
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
	const VkPipeline_fboStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	// declare and memset and initliase VkSubmitInfo structure
	VkSubmitInfo vksubmitInfo;
	memset((void*)&vksubmitInfo, 0, sizeof(VkSubmitInfo));

	vksubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	vksubmitInfo.pNext = NULL;
	vksubmitInfo.pWaitDstStageMask = &waitDstStageMask;
	vksubmitInfo.waitSemaphoreCount = 1;
	vksubmitInfo.pWaitSemaphores = &vkSemaphore_backbuffer;
	vksubmitInfo.commandBufferCount = 1;
	vksubmitInfo.pCommandBuffers = &vkCommandBuffer_fbo[currentImageIndex];
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
			resize_fbo(winWidth, winHeight);
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

void update_fbo(void)
{
	// code
	angleTeapot = angleTeapot + 1.0f;
	if (angleTeapot >= 360.0f)
	{
		angleTeapot = angleTeapot - 360.0f;
	}
}


void uninitialise_fbo(void)
{
	
	//Destroy vulkan device

	if (vkSemaphore_fbo)
	{
		vkDestroySemaphore(vkDevice, vkSemaphore_fbo, NULL);
		vkSemaphore_fbo = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFree vkSemaphore_fbo freed\n");

	}

	//Framebuffer free

	if (vkFramebuffer_fbo)
	{
		vkDestroyFramebuffer(vkDevice, vkFramebuffer_fbo, NULL);
		vkFramebuffer_fbo = NULL;
		fprintf(gpFile, "\nFree vkFramebuffer_fbo freed\n");
	}

	if (vkDescriptorSetLayout_fbo)
	{
		vkDestroyDescriptorSetLayout(vkDevice, vkDescriptorSetLayout_fbo, NULL);
		vkDescriptorSetLayout_fbo = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFree vkDescriptorSetLayout_fbo freed\n");
	}

	if (vkPipelineLayout_fbo)
	{
		vkDestroyPipelineLayout(vkDevice, vkPipelineLayout_fbo, NULL);
		vkPipelineLayout_fbo = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFree vkPipelineLayout_fbo freed\n");
	}

	if (vkPipeline_fbo)
	{
		vkDestroyPipeline(vkDevice, vkPipeline_fbo, NULL);
		vkPipeline_fbo = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFree vkPipeline_fbo: Pipeline freed successfully.\n");
	}


	if (vkRenderpass_fbo)
	{
		vkDestroyRenderPass(vkDevice, vkRenderpass_fbo, NULL);
		vkRenderpass_fbo = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFree vkRenderpass_fbo freed\n");
	}

	// destroy descriptorpool
	// when descriptor pull is destroyed descriptor set created by that pull get destroy imlicitly
	if (vkDescriptorPool_fbo)
	{
		vkDestroyDescriptorPool(vkDevice, vkDescriptorPool_fbo, NULL);
		vkDescriptorPool_fbo = VK_NULL_HANDLE;
		vkDescriptorSet = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFreed vkDescriptorPool_fbo and vkDescriptorSet\n");
	}


	// destroy shader modules
	if (vkShaderModule_fragment_shader_fbo)
	{
		vkDestroyShaderModule(vkDevice, vkShaderModule_fragment_shader_fbo, NULL);
		vkShaderModule_fragment_shader_fbo = NULL;
		fprintf(gpFile, "\nFree vkShaderModule_fragment_shader_fbo freed\n");
	}

	if (vkShaderModule_vertex_shader_fbo)
	{
		vkDestroyShaderModule(vkDevice, vkShaderModule_vertex_shader_fbo, NULL);
		vkShaderModule_vertex_shader_fbo = NULL;
		fprintf(gpFile, "\nFree vkShaderModule_vertex_shader_fbo freed\n");
	}

	// Destroy uniform buffer
	if (uniformData_fbo.vkBuffer)
	{
		vkDestroyBuffer(vkDevice, uniformData_fbo.vkBuffer, NULL);
		uniformData_fbo.vkBuffer = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFreed uniformData_fbo.vkBuffer \n");
	}

	if (uniformData_fbo.vkDeviceMemory)
	{
		vkFreeMemory(vkDevice, uniformData_fbo.vkDeviceMemory, NULL);
		uniformData_fbo.vkDeviceMemory = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFreed uniformData_fbo.vkDeviceMemory \n");
	}

	// Destroy the sampler
	if (vkSampler_Texture_fbo)
	{
		vkDestroySampler(vkDevice, vkSampler_Texture_fbo, NULL);
		vkSampler_Texture_fbo = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFreed vkSampler_Texture_fbo \n");
	}

	// Destroy the image view
	if (vkImageView_Texture_fbo)
	{
		vkDestroyImageView(vkDevice, vkImageView_Texture_fbo, NULL);
		vkImageView_Texture_fbo = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFreed vkImageView_Texture_fbo \n");
	}

	// Free the image memory
	if (vkDeviceMemory_fbo)
	{
		vkFreeMemory(vkDevice, vkDeviceMemory_fbo, NULL);
		vkDeviceMemory_fbo = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFreed VkDeviceMemory_fbo \n");
	}

	// Destroy the image
	if (vkImage_Texture_fbo)
	{
		vkDestroyImage(vkDevice, vkImage_Texture_fbo, nullptr);
		vkImage_Texture_fbo = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFreed VkImage_Texture_fbo \n");
	}

	if(vertexData_Index_fbo.vkDeviceMemory)
	{
		vkFreeMemory(vkDevice, vertexData_Index_fbo.vkDeviceMemory, NULL);
		vertexData_Index_fbo.vkDeviceMemory = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFree vertexData_Index_fbo.vkDeviceMemory freed\n");
	}

	if(vertexData_Index_fbo.vkBuffer)
	{
		vkDestroyBuffer(vkDevice, vertexData_Index_fbo.vkBuffer, NULL);
		vertexData_Index_fbo.vkBuffer = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFree vertexData_Index_fbo.vkBuffer freed\n");
	}

	if(vertexData_Texcoord_fbo.vkDeviceMemory)
	{
		vkFreeMemory(vkDevice, vertexData_Texcoord_fbo.vkDeviceMemory, NULL);
		vertexData_Texcoord_fbo.vkDeviceMemory = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFree vertexData_Texcoord_fbo.vkDeviceMemory freed\n");
	}

	if(vertexData_Texcoord_fbo.vkBuffer)
	{
		vkDestroyBuffer(vkDevice, vertexData_Texcoord_fbo.vkBuffer, NULL);
		vertexData_Texcoord_fbo.vkBuffer = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFree vertexData_Texcoord_fbo.vkBuffer freed\n");
	}

	if(vertexData_Normal_fbo.vkDeviceMemory)
	{
		vkFreeMemory(vkDevice, vertexData_Normal_fbo.vkDeviceMemory, NULL);
		vertexData_Normal_fbo.vkDeviceMemory = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFree vertexData_Normal_fbo.vkDeviceMemory freed\n");
	}

	if(vertexData_Normal_fbo.vkBuffer)
	{
		vkDestroyBuffer(vkDevice, vertexData_Normal_fbo.vkBuffer, NULL);
		vertexData_Normal_fbo.vkBuffer = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFree vertexData_Normal_fbo.vkBuffer freed\n");
	}

	if (vertexData_Position_fbo.vkDeviceMemory)
	{
		vkFreeMemory(vkDevice, vertexData_Position_fbo.vkDeviceMemory, NULL);
		vertexData_Position_fbo.vkDeviceMemory = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFree vertexData_Position_fbo.vkDeviceMemory freed\n");

	}

	if (vertexData_Position_fbo.vkBuffer)
	{
		vkDestroyBuffer(vkDevice, vertexData_Position_fbo.vkBuffer, NULL);
		vertexData_Position_fbo.vkBuffer = VK_NULL_HANDLE;
		fprintf(gpFile, "\nFree vertexData_Position_fbo.vkBuffer freed\n");

	}

	// free mallocated buffers
	if(pElements)
	{
		free(pElements);
		pElements = NULL;
		fprintf(gpFile, "\nFree pElements freed\n");

	}

	if(pTexcoords)
	{
		free(pTexcoords);
		pTexcoords = NULL;
		fprintf(gpFile, "\nFree pTexcoords freed\n");

	}

	if(pNormal)
	{
		free(pNormal);
		pNormal = NULL;
		fprintf(gpFile, "\nFree pNormal freed\n");

	}

	if(pPositions)
	{
		free(pPositions);
		pPositions = NULL;
		fprintf(gpFile, "\nFree pPositions freed\n");

	}


	if (vkCommandBuffer_fbo)
	{
		vkFreeCommandBuffers(vkDevice, vkcommandpool, 1, &vkCommandBuffer_fbo);
		vkCommandBuffer_fbo = NULL;
		fprintf(gpFile, "\n vkCommandBuffer_fbo is Freed\n");
	}

	// Destroy depth image view
	if (vkImageView_Depth_fbo)
	{
		vkDestroyImageView(vkDevice, vkImageView_Depth_fbo, NULL);
		vkImageView_Depth_fbo = VK_NULL_HANDLE;
		fprintf(gpFile, "\n[Depth] VkImageView destroyed.\n");
	}

	// Free memory for depth image
	if (vkDeviceMemory_Depth_fbo)
	{
		vkFreeMemory(vkDevice, vkDeviceMemory_Depth_fbo, NULL);
		vkDeviceMemory_Depth_fbo = VK_NULL_HANDLE;
		fprintf(gpFile, "\n[Depth] VkDeviceMemory freed.\n");
	}

	// destroy depth image
	if (vkImage_Depth_fbo)
	{
		vkDestroyImage(vkDevice, vkImage_Depth_fbo, NULL);
		vkImage_Depth_fbo = VK_NULL_HANDLE;
		fprintf(gpFile, "\n[Depth] vkImage_Depth_fbo freed.\n");

	}

	if(vksampler_fbo)
	{
		vkDestroySampler(vkDevice, vksampler_fbo, NULL);
		vksampler_fbo = VK_NULL_HANDLE;
		fprintf(gpFile, "\n vkSampler_fbo is Freed\n");
	}

	if(vkImageView_fbo)
	{
		vkDestroyImageView(vkDevice, vkImageView_fbo, NULL);
		vkImageView_fbo = VK_NULL_HANDLE;
		fprintf(gpFile, "\n vkImageView_fbo is Freed\n");
	}

	if(vkImage_fbo)
	{
		vkDestroyImage(vkDevice, vkImage_fbo, NULL);
		vkImage_fbo = VK_NULL_HANDLE;
		fprintf(gpFile, "\n vkImage_fbo is Freed\n");
	}

	if(vkDeviceMemory_fbo)
	{
		vkFreeMemory(vkDevice, vkDeviceMemory_fbo, NULL);
		vkDeviceMemory_fbo = VK_NULL_HANDLE;
		fprintf(gpFile, "\n vkDeviceMemory_fbo is Freed\n");
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
		vkFormat_color_fbo = VK_FORMAT_B8G8R8A8_UNORM;
	}
	else
	{
		vkFormat_color_fbo = vkSurfaceFormatKHR_Array[0].format;
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
	vkSwapchainCreateInfoKHR.imageFormat = vkFormat_color_fbo;
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


VkResult getSupportedDepthFormat(void)
{
	// code

	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	VkFormat VkFormat_Depth_fbo_Array[] =
	{
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
	};

	for (uint32_t i = 0; i < sizeof(VkFormat_Depth_fbo_Array) / sizeof(VkFormat_Depth_fbo_Array[0]); i++)
	{
		VkFormatProperties vkFormatProperties;
		memset(&vkFormatProperties, 0, sizeof(VkFormatProperties));

		vkGetPhysicalDeviceFormatProperties(vkPhysicalDevice_selected, VkFormat_Depth_fbo_Array[i], &vkFormatProperties);

		if (vkFormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			// This format can be used as a depth-stencil attachment
			vkFormat_Depth_fbo = VkFormat_Depth_fbo_Array[i];

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


VkResult createCommandBuffer_fbo(void)
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

	// Allocate command buffers
	vkresult = vkAllocateCommandBuffers(vkDevice, &vkCommandBufferAllocateInfo, vkCommandBuffer_fbo);

	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createCommandBuffer_fbo() : vkAllocateCommandBuffers() function failed. Error Code: (%d)\n", vkresult);
		free(vkCommandBuffer_fbo);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createCommandBuffer_fbo() : vkAllocateCommandBuffers() succeeded.\n");
	}

	return vkresult;
}

VkResult createVertexBuffer_fbo(void)
{

	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	memset((void*)&vertexData_Position_fbo, 0, sizeof(VertexData));

	VkBufferCreateInfo vkBufferCreateInfo;
	memset((void*)& vkBufferCreateInfo, 0, sizeof(VkBufferCreateInfo));

	vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBufferCreateInfo.pNext = NULL;
	vkBufferCreateInfo.flags = 0;
	vkBufferCreateInfo.size = sizeof(float) * 3 * numVerts;
	vkBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	vkresult = vkCreateBuffer(vkDevice, &vkBufferCreateInfo, NULL, &vertexData_Position_fbo.vkBuffer);
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

	vkGetBufferMemoryRequirements(vkDevice, vertexData_Position_fbo.vkBuffer, &vkMemoryRequirements);

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

	vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, NULL, &vertexData_Position_fbo.vkDeviceMemory);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkAllocateMemory() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkAllocateMemory() succeeded.\n");
	}

	vkresult = vkBindBufferMemory(vkDevice, vertexData_Position_fbo.vkBuffer, vertexData_Position_fbo.vkDeviceMemory, 0);
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

	vkresult = vkMapMemory(vkDevice, vertexData_Position_fbo.vkDeviceMemory, 0, vkMemoryAllocateInfo.allocationSize, 0, &data);
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

	memcpy(data, pPositions, sizeof(float) * 3 * numVerts);

	vkUnmapMemory(vkDevice, vertexData_Position_fbo.vkDeviceMemory);


	// FOR NORMAL
	memset((void*)&vertexData_Normal_fbo, 0, sizeof(VertexData));

	memset((void*)& vkBufferCreateInfo, 0, sizeof(VkBufferCreateInfo));

	vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBufferCreateInfo.pNext = NULL;
	vkBufferCreateInfo.flags = 0;
	vkBufferCreateInfo.size = sizeof(float) * 3 * numVerts;
	vkBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	vkresult = vkCreateBuffer(vkDevice, &vkBufferCreateInfo, NULL, &vertexData_Normal_fbo.vkBuffer);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkCreateBuffer() function failed for normal Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkCreateBuffer() succeeded for normal\n");
	}

	memset((void*)&vkMemoryRequirements, 0, sizeof(VkMemoryRequirements));

	vkGetBufferMemoryRequirements(vkDevice, vertexData_Normal_fbo.vkBuffer, &vkMemoryRequirements);

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

	vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, NULL, &vertexData_Normal_fbo.vkDeviceMemory);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkAllocateMemory() function failed for normal Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkAllocateMemory() succeeded for normal\n");
	}

	vkresult = vkBindBufferMemory(vkDevice, vertexData_Normal_fbo.vkBuffer, vertexData_Normal_fbo.vkDeviceMemory, 0);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkBindBufferMemory() function failed for normal Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkBindBufferMemory() succeeded for normal\n");
	}

	data = NULL;

	vkresult = vkMapMemory(vkDevice, vertexData_Normal_fbo.vkDeviceMemory, 0, vkMemoryAllocateInfo.allocationSize, 0, &data);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkMapMemory() function failed for normal Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkMapMemory() succeeded for normal\n");
	}

	// actual memory mapped

	memcpy(data, pNormal, sizeof(float) * 3 * numVerts);

	vkUnmapMemory(vkDevice, vertexData_Normal_fbo.vkDeviceMemory);



	// FOR TEXCOORD
	memset((void*)&vertexData_Texcoord_fbo, 0, sizeof(VertexData));

	memset((void*)& vkBufferCreateInfo, 0, sizeof(VkBufferCreateInfo));

	vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBufferCreateInfo.pNext = NULL;
	vkBufferCreateInfo.flags = 0;
	vkBufferCreateInfo.size = sizeof(float) * 2 * numVerts;
	vkBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	vkresult = vkCreateBuffer(vkDevice, &vkBufferCreateInfo, NULL, &vertexData_Texcoord_fbo.vkBuffer);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkCreateBuffer() function failed for texcoord Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkCreateBuffer() succeeded for texcoord\n");
	}

	memset((void*)&vkMemoryRequirements, 0, sizeof(VkMemoryRequirements));

	vkGetBufferMemoryRequirements(vkDevice, vertexData_Texcoord_fbo.vkBuffer, &vkMemoryRequirements);

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

	vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, NULL, &vertexData_Texcoord_fbo.vkDeviceMemory);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkAllocateMemory() function failed for texcoord Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkAllocateMemory() succeeded for texcoord\n");
	}

	vkresult = vkBindBufferMemory(vkDevice, vertexData_Texcoord_fbo.vkBuffer, vertexData_Texcoord_fbo.vkDeviceMemory, 0);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkBindBufferMemory() function failed for texcoord Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkBindBufferMemory() succeeded for texcoord\n");
	}

	data = NULL;

	vkresult = vkMapMemory(vkDevice, vertexData_Texcoord_fbo.vkDeviceMemory, 0, vkMemoryAllocateInfo.allocationSize, 0, &data);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createVertexBuffer() : vkMapMemory() function failed for texcoord Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createVertexBuffer() : vkMapMemory() succeeded for texcoord\n");
	}

	// actual memory mapped

	memcpy(data, pTexcoords, sizeof(float) * 2 * numVerts);

	vkUnmapMemory(vkDevice, vertexData_Texcoord_fbo.vkDeviceMemory);

	return vkresult;

}

VkResult createIndexBuffer_fbo(void)
{
	// code

	// Variable declaration
	VkResult vkresult = VK_SUCCESS;

	VkBufferCreateInfo vkBufferCreateInfo;
	memset((void*)& vkBufferCreateInfo, 0, sizeof(VkBufferCreateInfo));

	// POSITION INDEX BUFFER
	memset((void*)&vertexData_Index_fbo, 0, sizeof(VertexData));

	memset((void*)&vkBufferCreateInfo, 0, sizeof(VkBufferCreateInfo));

	vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBufferCreateInfo.pNext = NULL;
	vkBufferCreateInfo.flags = 0;
	vkBufferCreateInfo.size = sizeof(unsigned int) * numElements;
	vkBufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

	vkresult = vkCreateBuffer(vkDevice, &vkBufferCreateInfo, NULL, &vertexData_Index_fbo.vkBuffer);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createIndexBuffer() : vkCreateBuffer() function failed for position index buffer Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createIndexBuffer() : vkCreateBuffer() succeeded position index buffer \n");
	}


	VkMemoryRequirements vkMemoryRequirements;
	memset((void*)&vkMemoryRequirements, 0, sizeof(VkMemoryRequirements));

	vkGetBufferMemoryRequirements(vkDevice, vertexData_Index_fbo.vkBuffer, &vkMemoryRequirements);

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

	vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo, NULL, &vertexData_Index_fbo.vkDeviceMemory);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createIndexBuffer() : vkAllocateMemory() function failed for position index buffer Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createIndexBuffer() : vkAllocateMemory() succeeded for position index buffer \n");
	}

	vkresult = vkBindBufferMemory(vkDevice, vertexData_Index_fbo.vkBuffer, vertexData_Index_fbo.vkDeviceMemory, 0);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createIndexBuffer() : vkBindBufferMemory() function failed for position index buffer Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createIndexBuffer() : vkBindBufferMemory() succeeded for position index buffer\n");
	}

	void* data = NULL;

	vkresult = vkMapMemory(vkDevice, vertexData_Index_fbo.vkDeviceMemory, 0, vkMemoryAllocateInfo.allocationSize, 0, &data);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createIndexBuffer() : vkMapMemory() function failed for position index buffer Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createIndexBuffer() : vkMapMemory() succeeded for position index buffer\n");
	}

	// actual memory mapped
	memcpy(data, pElements, sizeof(unsigned int) * numElements);

	vkUnmapMemory(vkDevice, vertexData_Index_fbo.vkDeviceMemory);

	return vkresult;

}

VkResult createTexture_fbo(const char* textureFileName)
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

	vkresult = vkCreateImage(vkDevice, &vkImageCreateInfo, NULL, &vkImage_Texture_fbo);
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

	vkGetImageMemoryRequirements(vkDevice, vkImage_Texture_fbo, &vkMemoryRequirements_Image);

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

	vkresult = vkAllocateMemory(vkDevice, &vkMemoryAllocateInfo_Image, NULL, &vkDeviceMemory_fbo);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createTexture() : vkAllocateMemory() function failed for image Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createTexture() : vkAllocateMemory() succeeded for image\n");
	}

	vkresult = vkBindImageMemory(vkDevice, vkImage_Texture_fbo, vkDeviceMemory_fbo, 0);
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

	VkPipeline_fboStageFlags vkPipeline_fboStageFlags_Source = 0;
	VkPipeline_fboStageFlags vkPipeline_fboStageFlags_Destination = 0;


	VkImageMemoryBarrier vkImageMemoryBarrier;
	memset((void*)&vkImageMemoryBarrier, 0, sizeof(vkImageMemoryBarrier));

	vkImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	vkImageMemoryBarrier.pNext = NULL;
	vkImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	vkImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	vkImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vkImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vkImageMemoryBarrier.image = vkImage_Texture_fbo;
	vkImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	vkImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	vkImageMemoryBarrier.subresourceRange.levelCount = 1;
	vkImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	vkImageMemoryBarrier.subresourceRange.layerCount = 1;

	if (vkImageMemoryBarrier.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && vkImageMemoryBarrier.newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		vkImageMemoryBarrier.srcAccessMask = 0;
		vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkPipeline_fboStageFlags_Source = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		vkPipeline_fboStageFlags_Destination = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (vkImageMemoryBarrier.oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && vkImageMemoryBarrier.newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		vkPipeline_fboStageFlags_Source = VK_PIPELINE_STAGE_TRANSFER_BIT;
		vkPipeline_fboStageFlags_Destination = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		fprintf(gpFile, "createTexture() : unspported texture layout transitions\n");
		vkresult = VK_ERROR_INITIALIZATION_FAILED;
		return vkresult;
	}

	vkCmdPipelineBarrier(VkCommandBuffer_Transition_Image_Layout, vkPipeline_fboStageFlags_Source, vkPipeline_fboStageFlags_Destination, 0, 0, NULL, 0, NULL, 1, &vkImageMemoryBarrier);

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

	vkCmdCopyBufferToImage(VkCommandBuffer_Buffer_To_Image_Copy, vkBuffer_StaggingBuffer, vkImage_Texture_fbo, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vkBufferImageCopy);

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

	 vkPipeline_fboStageFlags_Source = 0;
	 vkPipeline_fboStageFlags_Destination = 0;


	memset((void*)&vkImageMemoryBarrier, 0, sizeof(vkImageMemoryBarrier));

	vkImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	vkImageMemoryBarrier.pNext = NULL;
	vkImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	vkImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	vkImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vkImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vkImageMemoryBarrier.image = vkImage_Texture_fbo;
	vkImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	vkImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	vkImageMemoryBarrier.subresourceRange.levelCount = 1;
	vkImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	vkImageMemoryBarrier.subresourceRange.layerCount = 1;

	if (vkImageMemoryBarrier.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && vkImageMemoryBarrier.newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		vkImageMemoryBarrier.srcAccessMask = 0;
		vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkPipeline_fboStageFlags_Source = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		vkPipeline_fboStageFlags_Destination = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (vkImageMemoryBarrier.oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && vkImageMemoryBarrier.newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		vkPipeline_fboStageFlags_Source = VK_PIPELINE_STAGE_TRANSFER_BIT;
		vkPipeline_fboStageFlags_Destination = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		fprintf(gpFile, "createTexture() : unspported texture layout transitions for step 6 : \n");
		vkresult = VK_ERROR_INITIALIZATION_FAILED;
		return vkresult;
	}

	vkCmdPipelineBarrier(VkCommandBuffer_Transition_Image_Layout, vkPipeline_fboStageFlags_Source, vkPipeline_fboStageFlags_Destination, 0, 0, NULL, 0, NULL, 1, &vkImageMemoryBarrier);

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
	vkImageViewCreateInfo.image = vkImage_Texture_fbo;

	vkresult = vkCreateImageView(vkDevice, &vkImageViewCreateInfo, NULL, &vkImageView_Texture_fbo);
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

	vkresult = vkCreateSampler(vkDevice, &vkSamplerCreateInfo, nullptr, &vkSampler_Texture_fbo);
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

// this is reagarding teapot.h
void addTriangle(float single_vertex[3][3], float single_normal[3][3], float single_texCoord[3][2])
{
	// function declarations
	BOOL closeEnough(const float, const float, const float);
	void normalizeVector(float[3]);

	// code
	unsigned int maxElements = numFaceIndices * 3;
	const float e = 0.00001f; // How small a difference to equate

	// First thing we do is make sure the normals are unit length!
	// It's almost always a good idea to work with pre-normalized normals
	normalizeVector(single_normal[0]);
	normalizeVector(single_normal[1]);
	normalizeVector(single_normal[2]);

	// Search for match - triangle consists of three verts
	for (unsigned int i = 0; i < 3; i++)
	{
		unsigned int j = 0;
		for (j = 0; j < numVerts; j++)
		{
			// If the vertex positions are the same
			if (closeEnough(pPositions[j * 3], single_vertex[i][0], e) &&
				closeEnough(pPositions[(j * 3) + 1], single_vertex[i][1], e) &&
				closeEnough(pPositions[(j * 3) + 2], single_vertex[i][2], e) &&

				// AND the Normal is the same...
				closeEnough(pNormal[j * 3], single_normal[i][0], e) &&
				closeEnough(pNormal[(j * 3) + 1], single_normal[i][1], e) &&
				closeEnough(pNormal[(j * 3) + 2], single_normal[i][2], e) &&

				// And Texture is the same...
				closeEnough(pTexcoords[j * 2], single_texCoord[i][0], e) &&
				closeEnough(pTexcoords[(j * 2) + 1], single_texCoord[i][1], e))
			{
				// Then add the index only
				pElements[numElements] = j;
				numElements++;
				break;
			}
		}

		// No match for this vertex, add to end of list
		if (j == numVerts && numVerts < maxElements && numElements < maxElements)
		{
			pPositions[numVerts * 3] = single_vertex[i][0];
			pPositions[(numVerts * 3) + 1] = single_vertex[i][1];
			pPositions[(numVerts * 3) + 2] = single_vertex[i][2];

			pNormal[numVerts * 3] = single_normal[i][0];
			pNormal[(numVerts * 3) + 1] = single_normal[i][1];
			pNormal[(numVerts * 3) + 2] = single_normal[i][2];

			pTexcoords[numVerts * 2] = single_texCoord[i][0];
			pTexcoords[(numVerts * 2) + 1] = single_texCoord[i][1];

			pElements[numElements] = numVerts;
			numElements++;
			numVerts++;
		}
	}
}

void normalizeVector(float u[3])
{
	// function declarations
	void scaleVector(float[3], const float);
	float getVectorLength(const float[3]);

	// code
	scaleVector(u, 1.0f / getVectorLength(u));
}

void scaleVector(float v[3], const float scale)
{
	// code
	v[0] *= scale;
	v[1] *= scale;
	v[2] *= scale;
}

float getVectorLength(const float u[3])
{
	// function declarations
	float getVectorLengthSquared(const float[3]);

	// code
	return(sqrtf(getVectorLengthSquared(u)));
}

float getVectorLengthSquared(const float u[3])
{
	// code
	return((u[0] * u[0]) + (u[1] * u[1]) + (u[2] * u[2]));
}

BOOL closeEnough(const float fCandidate, const float fCompare, const float fEpsilon)
{
	// code
	return((fabs(fCandidate - fCompare) < fEpsilon));
}





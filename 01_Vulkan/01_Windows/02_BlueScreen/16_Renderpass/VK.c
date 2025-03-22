// windows header files
#include<windows.h>
#include"VK.h"
#include<stdio.h> // for file IO
#include<stdlib.h> // for exit

#define VK_USE_PLATFORM_WIN32_KHR  // tells in which platform you are
#include<vulkan/vulkan.h>

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


// vulkun related global variables

uint32_t enabledInstanceExtensionCount = 0;


const char* enabledInstanceExtensionNames_array[2];  // VK_KHR_SURFACE_EXTENSION_NAME and VK_KHR_WIN32_SURFACE_EXTENSION_NAME


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
uint32_t SwapchainImageCount = UINT32_MAX;  // Fixed 'unit32_t' to 'uint32_t' and 'UNIT32_MAX' to 'UINT32_MAX'
VkImage* SwapchainImage_Array = NULL;
VkImageView* SwapchainImageView_Array = NULL;


// command pool 
VkCommandPool vkcommandpool = VK_NULL_HANDLE;

// command buffer
VkCommandBuffer* vkCommandBuffer_Array = NULL;

// render pass
VkRenderPass vkRenderpass = VK_NULL_HANDLE;



//entry_point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstace, LPSTR lpszCmdLine, int iCmdShow)
{
	// Function Declaration 
	VkResult initialise(void);
	void uninitialise(void);
	void display(void);
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
				// RENDER
				display();

				// update
				update();
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
	void resize(int, int);

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
		resize(LOWORD(lParam), HIWORD(lParam));
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
	VkResult createRenderPass(void);


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

	fprintf(gpFile, "******************************************* Initialise comment *****************************\n");

	return(vkresult);
}



void resize(int width, int heigth)
{
	// code
	if (heigth <= 0)
		heigth = 1;
}

void display(void)
{
	// code

}

void update(void)
{
	// code
}


void uninitialise(void)
{
	// function declaration
	void ToggleFullscreen(void);
	//code
	//if appliacation is exitting in fullscreen
	if (gbFullscreen == TRUE)
	{
		ToggleFullscreen();
		gbFullscreen = FALSE;
	}

	// Destroywindow
	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	// no need to destroy / unitialise device queue

	// Destroy Vulkan Device
	if (vkDevice)
	{
		vkDeviceWaitIdle(vkDevice);
		fprintf(gpFile, "uninitialise() : vkDeviceWaitIdle is Done\n");

		// destroy image views
		for (uint32_t i = 0; i < SwapchainImageCount; i++)
		{
			vkDestroyImageView(vkDevice, SwapchainImageView_Array[i], NULL);
			fprintf(gpFile, "uninitialise() : Image view is free\n");
		}

		if (SwapchainImageView_Array)
		{
			free(SwapchainImageView_Array);
			SwapchainImageView_Array = NULL;
			fprintf(gpFile, "uninitialise() : Images array is free\n");
		}

		// free swapchain images 
		//for (uint32_t i = 0; i < SwapchainImageCount; i++) 
		//{
		//	vkDestroyImage(vkDevice, SwapchainImage_Array[i], NULL); // Fixed 'VkDestroyImage' to 'vkDestroyImage'
		//	fprintf(gpFile, "uninitialise() : VkDestroyImage is Done\n");
		//}

		// free actual image view array
		if (SwapchainImage_Array)
		{
			free(SwapchainImage_Array);
			SwapchainImage_Array = NULL;
			fprintf(gpFile, "uninitialise() : SwapchainImage_Array is free\n");
		}

		for (uint32_t i = 0; i < SwapchainImageCount; i++)
		{
			vkFreeCommandBuffers(vkDevice, vkcommandpool, 1, &vkCommandBuffer_Array[i]);
			//vkDestroyImageView(vkDevice, swapchainImageView_array[i], NULL);
			fprintf(gpFile, "\nFree commandbuffers freed\n");
		}

		// Destroy render pass
		if (vkRenderpass)
		{
			vkDestroyRenderPass(vkDevice, vkRenderpass, NULL);
			vkRenderpass = VK_NULL_HANDLE;
			fprintf(gpFile, "uninitialise() : vkDestroyRenderPass is Done\n");
		}

		// free actual array
		if (vkCommandBuffer_Array)
		{
			free(vkCommandBuffer_Array);
			vkCommandBuffer_Array = NULL;
			fprintf(gpFile, "uninitialise() : vkCommandBuffer_Array is free\n");
		}

		// destroy command pool
		if (vkcommandpool)
		{
			vkDestroyCommandPool(vkDevice, vkcommandpool, NULL);
			vkcommandpool = VK_NULL_HANDLE;
			fprintf(gpFile, "uninitialise() : VkDestroyCommandpool is Done\n");
		}

		// destroy swapchain
		if (vkSwapchainKHR)
		{
			vkDestroySwapchainKHR(vkDevice, vkSwapchainKHR, NULL);
			vkSwapchainKHR = VK_NULL_HANDLE;
			fprintf(gpFile, "uninitialise() : vkDestroySwapchainKHR is Done\n");
		}

		vkDestroyDevice(vkDevice, NULL);
		vkDevice = VK_NULL_HANDLE;
		fprintf(gpFile, "uninitialise() : vkDestroyDevice is Done\n");
	}


	// No need to destroy selected physical device

	if (vkSurfaceKHR)
	{
		vkDestroySurfaceKHR(vkInstance, vkSurfaceKHR, NULL);
		vkSurfaceKHR = VK_NULL_HANDLE;
		fprintf(gpFile, "uninitialise() : vkDestroySurfaceKHR() succeeded\n");

	}

	// destroy Vulkan instance
	if (vkInstance)
	{
		vkDestroyInstance(vkInstance, NULL);
		vkInstance = VK_NULL_HANDLE;
		fprintf(gpFile, "uninitialise() : vkDestroyInstance() succeeded\n");
	}

	// closed log file
	if (gpFile)

	{
		fprintf(gpFile, "uninitialise() : programm is sucessfully ended\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}

//////////////////////////////////////////////////////     DEFINATION OF VULKUN RELATED FUNCTION     //////////////////////////////////////////////////////////

VkResult createVulkanInstance(void)
{
	// function declarations
	VkResult fillExtensionNames();

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
		return vkresult;
	}


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

	// Step 7: Print only enabled extension names
	for (uint32_t i = 0; i < enabledInstanceExtensionCount; i++)
	{
		fprintf(gpFile, "fillExtensionNames() : enabled Vulkan extension name = %s\n", enabledInstanceExtensionNames_array[i]);
	}

	// Return after completing the loop
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

		vkExtent2D_Swapchain.width = max(vkSurfaceCapabilitiesKHR.minImageExtent.width, min(vkSurfaceCapabilitiesKHR.maxImageExtent.width, vkExtent2D.width));
		vkExtent2D_Swapchain.height = max(vkSurfaceCapabilitiesKHR.minImageExtent.height, min(vkSurfaceCapabilitiesKHR.maxImageExtent.height, vkExtent2D.height));

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
	// variable declaration
	VkResult vkresult = VK_SUCCESS;

	// get swapchain image count
	vkresult = vkGetSwapchainImagesKHR(vkDevice, vkSwapchainKHR, &SwapchainImageCount, NULL);
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "createImagesAndImageViews() 1st call : vkGetSwapchainImagesKHR() function failed (%d)\n", vkresult);
		return vkresult;
	}

	else if (SwapchainImageCount == 0)
	{
		fprintf(gpFile, "createImagesAndImageViews() 1st call : swapchain image count is zero, returning hardcoded error value\n");
		return VK_ERROR_INITIALIZATION_FAILED;
	}
	else
	{
		fprintf(gpFile, "createImagesAndImageViews() 1st call : this func is giving the swapchain image count = %d\n", SwapchainImageCount);
	}

	//// allocate the swapchain image array
	SwapchainImage_Array = (VkImage*)malloc(sizeof(VkImage) * SwapchainImageCount);

	//// fill this array with swapchain images
	vkresult = vkGetSwapchainImagesKHR(vkDevice, vkSwapchainKHR, &SwapchainImageCount, SwapchainImage_Array);
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
	SwapchainImageView_Array = (VkImageView*)malloc(sizeof(VkImageView) * SwapchainImageCount);

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
	for (uint32_t i = 0; i < SwapchainImageCount; i++)
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
	vkCommandBufferAllocateInfo.commandBufferCount = SwapchainImageCount;

	// Allocate memory for command buffer array
	vkCommandBuffer_Array = (VkCommandBuffer*)malloc(sizeof(VkCommandBuffer) * SwapchainImageCount);

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
		fprintf(gpFile, "createRenderPass() : vkCreateRenderPass() function failed. Error Code: (%d)\n", vkresult);
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "createRenderPass() : vkCreateRenderPass() succeeded.\n");
	}

	return vkresult;
}















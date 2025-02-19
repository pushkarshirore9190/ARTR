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






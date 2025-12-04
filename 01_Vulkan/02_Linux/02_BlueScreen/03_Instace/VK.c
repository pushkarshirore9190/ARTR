// standard header file
#include<stdio.h> // for prinf()
#include<stdlib.h>  // for exit()
#include<memory.h> // for memset()
#include<limits.h>


// X11 header files
#include<X11/Xlib.h> // for all XWindow APIs
#include<X11/Xutil.h> // for XVisiualInfo and related API
#include<X11/XKBlib.h>
#include <X11/keysym.h>
#include<X11/Xatom.h> // for XA_ATOM

#define VK_USE_PLATFORM_XLIB_KHR
#include<vulkan/vulkan.h>


// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))



// global varialble declaration
const char * gpszAppName = "ARTR";


Display *gpDisplay = NULL;
Colormap colormap;
Window window;
XVisualInfo *gpXvisualInfo = NULL;


int winWidth = WIN_WIDTH;
int winHeight = WIN_HEIGHT;

Bool bActiveWIndow = False;
Bool bEscapeKeyIsPressed = False;
Bool bFullscreen = False;
Bool bWindowMinimized = False;

FILE * gpFile = NULL;



// vulkun related global variables

uint32_t enabledInstanceExtensionCount = 0;


const char* enabledInstanceExtensionNames_array[2];  // VK_KHR_SURFACE_EXTENSION_NAME and VK_KHR_WIN32_SURFACE_EXTENSION_NAME


// instance extension related variables

VkInstance vkInstance = VK_NULL_HANDLE;



// entry point function
int main(int argc, char *argv[])
{
    // Function Declaration 
	VkResult initialise(void);
	void uninitialise(void);
	void display(void);
	void update(void);
	void toggleFullScreen(void);
	void resize(int, int);
    Bool isWindowMinimized(void);


    VkResult vkresult = VK_SUCCESS;


    // Local Variable Declation
    XVisualInfo XvisualInfo;
    int iNumFBConfig = 0;
    XSetWindowAttributes WindowAttributes;
    int defaultScreen;
    int defaultDepth;
    Status status;
    XSetWindowAttributes windowAttribute;
    int styleMask;
    Atom windowManagerDelete;
    XEvent event;
    KeySym keysym;
    int screenWidth;
    int screenHeight;
    char keys[26];
    Bool bDone = False;

    // code

    gpFile = fopen("Log.txt","w");
    if(gpFile == NULL)
    {
        printf(" main(): Log File Can Not Be Created. Exiting Now ...\n");
        exit(1);
    }
    else
    {
        fprintf(gpFile," main() : Programm Started Successfully. \n");
    }

	if (XInitThreads() == 0) {
        fprintf(gpFile, "main(): XInitThreads() failed\n");
        uninitialise();
        exit(1);
    }

    // Open the connection with x-server interface and get gpDisplay interface
    gpDisplay = XOpenDisplay(NULL);
    if(gpDisplay == NULL)
    {
        fprintf(gpFile, "main(): XOpenDisplay Failed \n");
        uninitialise();
        exit(1);
    }

    // Get Default screen from above gpDisplay
    defaultScreen = XDefaultScreen(gpDisplay);

    // Get Default Depth from above two 
    //defaultDepth = XDefaultDepth(gpDisplay,defaultScreen);

    // Get Visiualinfo from above three
    memset((void*)&XvisualInfo,0,sizeof(XVisualInfo));

    XvisualInfo.screen = defaultScreen;
    gpXvisualInfo = XGetVisualInfo(gpDisplay,VisualScreenMask,&XvisualInfo,&iNumFBConfig);

    if(gpXvisualInfo == NULL)
    {
        fprintf(gpFile,"main(): XGetVisualInfo Failed \n");
        uninitialise();
        exit(1);
    }

    fprintf(gpFile, "Found Number Of iNumFBConfig = %d\n",iNumFBConfig);

    // create colormap
    colormap = XCreateColormap(gpDisplay,
    XRootWindow(gpDisplay,XvisualInfo.screen),gpXvisualInfo->visual,AllocNone);

    // set Window attributes / Properties
    memset((void*)&windowAttribute,0,sizeof(XSetWindowAttributes));

    windowAttribute.border_pixel = 0;
    windowAttribute.background_pixel = XBlackPixel(gpDisplay,defaultScreen);
    windowAttribute.background_pixmap = 0;
    windowAttribute.colormap = colormap;
    windowAttribute.event_mask = ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask | PropertyChangeMask | FocusChangeMask;
     // set the style of window
    styleMask = CWBorderPixel | CWBackPixel | CWColormap | CWEventMask;

    // Now finally create window
    window = XCreateWindow(gpDisplay,
    XRootWindow(gpDisplay,XvisualInfo.screen),
    0,
    0,
    WIN_WIDTH,
    WIN_HEIGHT,
    0,
    gpXvisualInfo->depth,
    InputOutput,
    gpXvisualInfo->visual,
    styleMask,
    &windowAttribute);

    if(!window)
    {
        fprintf(gpFile,"main(): XCreateWindow Failed \n");
        uninitialise();
        exit(1);
    }

    vkresult = initialise();
	if (vkresult != VK_SUCCESS)
	{
		fprintf(gpFile, "Winmain() : Intialise() failed with error code : %d\n", vkresult);
        uninitialise();
        exit(0);
	}
	else
	{
		fprintf(gpFile, "Winmain() : Intialise() Succeded. \n");
	}

    // Give caption to window
    XStoreName(gpDisplay,window,"PRS : Vulkan");

    // specify window manager delete atom
    windowManagerDelete = XInternAtom(gpDisplay,"WM_DELETE_WINDOW",True);

    // set above atom as protocol for window manager
    XSetWMProtocols(gpDisplay,window,&windowManagerDelete,1);

     // show / map the window
    XMapWindow(gpDisplay,window);


    // Center the Window
    screenWidth = XWidthOfScreen(XScreenOfDisplay(gpDisplay,defaultScreen));
    screenHeight = XHeightOfScreen(XScreenOfDisplay(gpDisplay,defaultScreen));
    XMoveWindow(gpDisplay,window,(screenWidth - WIN_WIDTH) / 2 , (screenHeight - WIN_HEIGHT) / 2);

    // Event Loop
    while (bDone == False)
    {
        while (XPending(gpDisplay))
        {
           
            XNextEvent(gpDisplay,&event);
            switch (event.type)
            {
                case MapNotify:
                    break;
                case FocusIn: // simlar to WM_SETFOCUS
                    bActiveWIndow = True;
                    break;
                case FocusOut: // simlar to WM_KILLFOCUS
                    bActiveWIndow = False;
                    break;
                case KeyPress :
                    keysym = XkbKeycodeToKeysym(gpDisplay,event.xkey.keycode,0,0);

                switch (keysym)
                {
                    case XK_Escape:
                        bEscapeKeyIsPressed = True;
                        break; 

                    default:
                        break;   
                }

                XLookupString(&event.xkey,keys,sizeof(keys),NULL,NULL);

                switch(keys[0])
                {
                    case 'F':
                    case 'f':
                        if(bFullscreen == False)
                        {
                            toggleFullScreen();
                            bFullscreen = True;
                        }
                        else
                        {
                            toggleFullScreen();
                            bFullscreen = False;
                        }
                        break;

                        default:
                            break;
                        
                }
                break;

                case ConfigureNotify: // similar to WM_SIZE
                    winWidth = event.xconfigure.width;
                    winHeight = event.xconfigure.height;
                    resize(winWidth,winHeight);
                    break;

                case PropertyNotify:
                    if (isWindowMinimized() == True)
                    {
                        bWindowMinimized = True;
                    }
                    else
                    {
                        bWindowMinimized = False;
                    }
                    break;

                case DestroyNotify:
                    break;
    
                case 33 : // for WM_DESTROY
                    bDone = True;
                    break;
                break;
                default:
                break;
            }
        }

        if (bActiveWIndow == True)
        {
            if (bEscapeKeyIsPressed == True)
            {
                bDone = True;
            }

            // Display
            if (bWindowMinimized == False)
            {
               // RENDER
				display();

                // update
                update();
            }
        }
    }

    uninitialise();

    printf("End of the programm\n");

    return 0; 
}

void toggleFullScreen(void)
{
    // Local Variable Declaration
    Atom windowManagerStateNormal;
    Atom windowManagerStateFullscreen;
    XEvent event; 

    // code
    windowManagerStateNormal = XInternAtom(gpDisplay,"_NET_WM_STATE",False);

    windowManagerStateFullscreen = XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);

    // memeset event struct and fill it with above two atoms
    memset((void*)&event,0,sizeof(XEvent));

    event.type = ClientMessage;
    event.xclient.window = window;
    event.xclient.message_type = windowManagerStateNormal;
    event.xclient.format = 32;
    event.xclient.data.l[0] = bFullscreen?0:1;
    event.xclient.data.l[1] = windowManagerStateFullscreen;

    // send the event
    XSendEvent(gpDisplay,
    XRootWindow(gpDisplay,gpXvisualInfo->screen),
    False,
    SubstructureNotifyMask,
    &event);


}

Bool isWindowMinimized(void)
{
    // function declaration
    void uninitialise(void);

    // Local Varible Delcaration
    Bool windowMinimized = False;
    int iResult = -1;
    Atom Return_Property_Type = None;
    int Return_Property_Format = -1;
    unsigned long number_of_returned_items = 0;
    unsigned long number_of_bytes_remained = 0;
    unsigned char *returned_property_data_array = NULL;
    Atom wm_state = XInternAtom(gpDisplay,"_NET_WM_STATE", True);

    if(wm_state == None)
    {
        fprintf(gpFile,"isWindowMinimized(): XInternAtom Failed To Find _NET_WM_STATE \n");
        uninitialise();
        exit(1);
    }

    Atom wm_state_hidden = XInternAtom(gpDisplay,"_NET_WM_STATE_HIDDEN", True);
    if(wm_state_hidden == None)
    {
        fprintf(gpFile,"isWindowMinimized(): XInternAtom Failed To Find _NET_WM_STATE_HIDDEN \n");
        uninitialise();
        exit(1);
    }

    iResult = XGetWindowProperty(gpDisplay,
                                 window,
                                 wm_state,
                                 0L,
                                 1024,
                                 False,
                                 XA_ATOM,
                                 &Return_Property_Type,
                                 &Return_Property_Format,
                                 &number_of_returned_items,
                                 &number_of_bytes_remained,
                                 &returned_property_data_array);

    if(iResult != Success || returned_property_data_array == NULL)
    {
        if(returned_property_data_array != NULL)
        {
            XFree(returned_property_data_array);
            returned_property_data_array = NULL;
        }

        return False;
    }
    else
    {
        // loop the returned array to get required property
        Atom *atomArray = (Atom *)returned_property_data_array;
        for(unsigned long i = 0; i < number_of_returned_items; i++)
        {
            // check whether array contain hidden property or not
            if(atomArray[i] == wm_state_hidden)
            {
                windowMinimized = True;
                break;
            }
        }
    }

    if(returned_property_data_array != NULL)
    {
        XFree(returned_property_data_array);
        returned_property_data_array = NULL;
    }

    return windowMinimized;
}

VkResult initialise(void)
{
	// function declarations

	VkResult createVulkanInstance(void);


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


	// initialisation is completed	
	fprintf(gpFile, "******************************************* initialise comment *****************************\n");

	fprintf(gpFile, "initialised()  :  Initialisation() Complete Successfully\n");

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
	// Function declarations
	void toggleFullScreen(void);

    // restore fullscreen
    if(bFullscreen == True)
    {
        toggleFullScreen();
        bFullscreen = False;
    }

    if(window)
    {
        XDestroyWindow(gpDisplay,window);
    }


	// no need to destroy / unitialise device queue

	// no need to destroy / unitialise device queue

	// No need to destroy selected physical device
	
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

		if (strcmp(InstanceExtensionNames_array[i], VK_KHR_XLIB_SURFACE_EXTENSION_NAME) == 0)
		{
			win32SurfaceExtensionFound = VK_TRUE;
			enabledInstanceExtensionNames_array[enabledInstanceExtensionCount++] = VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
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
		fprintf(gpFile, "fillExtensionNames() : VK_KHR_XLIB_SURFACE_EXTENSION_NAME not found\n");
		return vkresult;
	}
	else
	{
		fprintf(gpFile, "fillExtensionNames() : VK_KHR_XLIB_SURFACE_EXTENSION_NAME found\n");
	}

	// Step 7: Print only enabled extension names
	for (uint32_t i = 0; i < enabledInstanceExtensionCount; i++)
	{
		fprintf(gpFile, "fillExtensionNames() : enabled Vulkan extension name = %s\n", enabledInstanceExtensionNames_array[i]);
	}

	// Return after completing the loop
	return vkresult;
}







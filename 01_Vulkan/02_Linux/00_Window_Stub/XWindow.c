// standard header file
#include<stdio.h> // for prinf()
#include<stdlib.h>  // for exit()
#include<memory.h> // for memset()


// X11 header files
#include<X11/Xlib.h> // for all XWindow APIs
#include<X11/Xutil.h> // for XVisiualInfo and related API
#include<X11/XKBlib.h>
#include <X11/keysym.h>
#include<X11/Xatom.h> // for XA_ATOM


// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600


// global varialble declaration
const char * gpszAppName = "ARTR";


Display *gpDisplay = NULL;
Colormap colormap;
Window window;
XVisualInfo *gpXvisualInfo = NULL;


int win_width = WIN_WIDTH;
int win_Height = WIN_HEIGHT;

Bool bActiveWIndow = False;
Bool bEscapeKeyIsPressed = False;
Bool bFullscreen = False;
Bool bWindowMinimized = False;

FILE * gpFIle = NULL;

// entry point function
int main(int argc, char *argv[])
{
    // Local Functoin Declation
    void toggleFullScreen(void);
    Bool isWindowMinimized(void);
    int initialize(void);
    void resize(int, int);
    void display(void);
    void update(void);
    void unitialise(void);


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
    Bool bDOne = False;

    // code

    gpFIle = fopen("Log.txt","w");
    if(gpFIle == NULL)
    {
        printf(" main(): Log File Can Not Be Created. Exiting Now ...\n");
        exit(1);
    }
    else
    {
        fprintf(gpFIle," main() : Programm Started Successfully. \n");
    }

    // Open the connection with x-server interface and get gpDisplay interface
    gpDisplay = XOpenDisplay(NULL);
    if(gpDisplay == NULL)
    {
        fprintf(gpFIle, "main(): XOpenDisplay Failed \n");
        unitialise();
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
        fprintf(gpFIle,"main(): XGetVisualInfo Failed \n");
        unitialise();
        exit(1);
    }

    fprintf(gpFIle, "Found Number Of iNumFBConfig = %d\n",iNumFBConfig);

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
        fprintf(gpFIle,"main(): XCreateWindow Failed \n");
        unitialise();
        exit(1);
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

    // initialize Window
    int iResult = initialize();
    if(iResult != 0)
    {
        fprintf(gpFIle,"main(): initialize() Failed \n");
        unitialise();
        exit(1);
    }
    else
    {
        fprintf(gpFIle,"main(): initialize() Succeded \n");
    }

    // Event Loop
    while (bDOne == False)
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
                    win_width = event.xconfigure.width;
                    win_Height = event.xconfigure.height;
                    resize(win_width,win_Height);
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
                    bDOne = True;
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
                bDOne = True;
            }

            // Display
            if (bWindowMinimized == False)
            {
                display();

                // Update
                update();
            }
        }
    }

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
    void unitialise(void);

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
        fprintf(gpFIle,"isWindowMinimized(): XInternAtom Failed To Find _NET_WM_STATE \n");
        unitialise();
        exit(1);
    }

    Atom wm_state_hidden = XInternAtom(gpDisplay,"_NET_WM_STATE_HIDDEN", True);
    if(wm_state_hidden == None)
    {
        fprintf(gpFIle,"isWindowMinimized(): XInternAtom Failed To Find _NET_WM_STATE_HIDDEN \n");
        unitialise();
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

int initialize(void)
{
    // code
    return 0;
}

void resize(int width, int height)
{
    // code
}

void display(void)
{
    // code
}


void update(void)
{
    // code
}

void unitialise(void)
{
    // function declaration
    void toggleFullScreen(void);

    // code

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

    if(colormap)
    {
        XFreeColormap(gpDisplay,colormap);
    }

    if(gpXvisualInfo)
    {
        XFree((void*)gpXvisualInfo);
        gpXvisualInfo = NULL;
    }

    if(gpDisplay)
    {
        XCloseDisplay(gpDisplay);
        gpDisplay = NULL;
    }
    if(gpFIle)
    {
        fprintf(gpFIle,"unitialise(): Program Terminated Successfully. \n");
        fclose(gpFIle);
        gpFIle = NULL;
    }

}



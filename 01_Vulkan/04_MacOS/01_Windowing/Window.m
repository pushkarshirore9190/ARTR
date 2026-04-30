#import <Foundation/Foundation.h>
#import<cocoa/cocoa.h>

// macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// global variable declarations
int winWidth = WIN_WIDTH;
int winHeight = WIN_HEIGHT;

BOOL bActiveWindow = NO;
BOOL bFullScreen = NO;
BOOL bWindowMinimized = NO;

char gszLogFileName[] = "Log.txt";
FILE *gpFile = NULL;

// forward interface declarations
@interface AppDelegate:NSObject<NSApplicationDelegate,NSWindowDelegate>
@end

@interface View:NSView<NSWindowDelegate>
@end

// entry point function
int main(int argc, char* argv[]) 
{

    // code

    // create auto relaese pool for memory mangemnt
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc]init];

    // cretate global share app application object
    NSApp = [NSApplication sharedApplication];

    [[NSApp] setActivationPolicy:NSApplicationActivationPolicyRegular];

    // set its delete to our app delegate custom calss
    [NSApp setDelegate : [[AppDelegate alloc]init]];

    // start NS App loop
    [NSApp run];

    //let auto release pool release all pending objectts in our application
    [pool release];

    return 0;
}

// app delegate interface implementation
@implementation AppDelegate
{
    @private
    NSWindow * window;
    
    View* view;
}

    -(void)applicationDidFinishLaunching:(NSNotification*)notification
    {
        // code
        // Declare rectangle for frame or border of our window
        // log file opening code
        
                NSBundle* appBundle = [NSBundle mainBundle];
                NSString* appDirPath= [appBundle bundlePath];
                NSString* parentDirPath = [appDirPath stringByDeletingLastPathComponent];
         
                NSString* logFileNameWithPath = [NSString stringWithFormat: @"%@/log.txt",parentDirPath];
                const char* pszLogFileNameWithPath = [logFileNameWithPath cStringUsingEncoding : NSASCIIStringEncoding];
                gpFile = fopen(pszLogFileNameWithPath, "w");
                if (gpFile == NULL)
                {
                    //NSLog(@"Log file cannot be created");
                    printf("Log file not created");
                    [self release] ;
                    [NSApp terminate : self] ;
                }
        
                fprintf(gpFile, "Program started succesfully");
        
        NSRect win_rect = NSMakeRect(0.0, 0.0, 800.0, 600.0);

        // create the window
        window = [[NSWindow alloc]initWithContentRect:win_rect
                                            styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable
                                             backing : NSBackingStoreBuffered
                                               defer : NO];

        //  give the title to the window

        [window setTitle : @"PRS : Cocoa window"];

        // centre the window
        [window center] ;

        // set the background color of window to black
        [window setBackgroundColor : [NSColor blackColor]] ;

        // create the custom view
        view = [[View alloc]initWithFrame:win_rect];

        // set this newly created custom view as view of our newly created custom window
        [window setContentView : view] ;


        [window setDelegate : self] ;

        // now actually show the window, give it keyboeard focus and make it top in the z order
        [window makeKeyAndOrderFront : self] ;

        // tell NS App to activate our application and its window
        [NSApp activateIgnoringOtherApps : YES] ;
    }

    -(void)applicationWillTerminate:(NSNotification*)notification
    {
        [self release] ;
    }

    -(void)dealloc
    {
        [super dealloc] ;
        [view release] ;
        [window release] ;
    }
@end

// implemetation of custom view interface
@implementation View


    -(id)initWithFrame:(NSRect)frame
    {
        // code
        self = [super initWithFrame:frame];
        if(self)
        {
            int result = [self initialise];

            if (result == -1)
            {
                fprintf(gpFile, "Cannot initialise failed");
            }
            else
            {
                fprintf(gpFile, "Initialisation successful");
            }
        }

        return(self);
    }

    -(void)windowDidBecomeKey:(NSNotification*)notification
    {
        // code
        bActiveWindow = YES;
    }

    -(void)windowDidResignKey:(NSNotification*)notification
    {
        // code
        bActiveWindow = NO;
    }

    -(NSSize)windowWillResize:(NSWindow*)sender toSize:(NSSize)frameSize
    {
        // code
       if(bWindowMinimized == NO)
       {
            [[self Resize] : frameSize.width : frameSize.height];
       }
           
        return (frameSize);
    }

    -(void)windowDidResize : (NSNotification*)notification
    {
        // code
    }

    -(void)windowWillMiniaturize : (NSNotification*)notification
    {
        // code
        bWindowMinimized = YES;
    }

    -(void)windowDidMiniaturize : (NSNotification*)notification
    {
        // code
    }

    -(void)windowDidDeminiaturize : (NSNotification*)notification
    {
        // code
        bWindowMinimized = NO;
    }

    -(void)windowWillClose : (NSNotification*)notification
    {
        // code
        [self uninitialise] ;
        [NSApp terminate : self] ;
    }

    -(void)drawRect:(NSRect)dirtyRect
    {
        // code 
        [self drawView];

    }

    -(void)drawView
    {
       // code

       // display
       [self display];

       // update
       [self update];
    }

    -(BOOL)acceptsFirstResponder
    {
        // code
        [[self window]makeFirstResponder:self];
        return (YES);
    }

    -(void)keyDown:(NSEvent*)event
    {
        //code
        int key = (int) [[event characters]characterAtIndex:0];

        switch(key)
        {
            case 27:
                if(bFullScreen == YES)
                {
                    [[self window]toggleFullScreen:self];
                    bFullScreen = NO;
                }

                [[self window]performClose:self]; 
                break;

            case 'F':
            case 'f':
                if(bFullScreen == NO)
                {
                    [[self window]toggleFullScreen:nil];
                    bFullScreen = YES;
                }
                else
                {
                    [[self window]toggleFullScreen:self];
                    bFullScreen = NO;
                }
                break;

            default:
                break;
        }
    }

    -(void)dealloc
    {
        [super dealloc] ;
    }

    // user defined methods of view class

    -(int)initialise
    {
        // code
        return (0);
    }

    -(void)Resize : (int)width : (int)height
    {
        // code
    }

    -(void)display
    {
        // code
    }

    -(void)update
    {
        // code
    }

    -(void)uninitialise
    {
        // code
        if(bFullScreen == YES)
        {
            [[self window]toggleFullScreen:nil];
            bFullScreen = NO;
        }

        if(gpFile)
        {
            fprintf(gpFile, "Program terminated successfully");
            fclose(gpFile);
            gpFile = NULL;
        }   
    }
    
@end



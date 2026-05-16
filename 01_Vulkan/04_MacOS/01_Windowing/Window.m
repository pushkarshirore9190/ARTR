#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

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

-(id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if(self)
    {
        int result = [self initialise];

        if (result == -1)
            fprintf(gpFile, "Initialisation failed\n");
        else
            fprintf(gpFile, "Initialisation successful\n");
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
    if(bWindowMinimized == NO)
    {
        [self Resize:frameSize.width :frameSize.height];
    }
    return frameSize;
}

-(void)windowWillMiniaturize:(NSNotification*)notification
{
    bWindowMinimized = YES;
}

-(void)windowDidDeminiaturize:(NSNotification*)notification
{
    bWindowMinimized = NO;
}

-(void)windowWillClose:(NSNotification*)notification
{
    [self uninitialise];
    [NSApp terminate:self];
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
    [super dealloc];
}

// user methods
-(int)initialise
{
    return 0;
}

-(void)Resize:(int)width :(int)height
{
}

-(void)display
{
}

-(void)update
{
}

-(void)uninitialise
{
    if(bFullScreen == YES)
    {
        [[self window] toggleFullScreen:nil];
        bFullScreen = NO;
    }

    if(gpFile)
    {
        fprintf(gpFile, "Program terminated successfully\n");
        fclose(gpFile);
        gpFile = NULL;
    }
}

@end




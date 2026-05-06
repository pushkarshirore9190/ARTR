#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#import <QuartzCore/CVDisplayLink.h> // this is for coreVideo display link
#import<QuartzCore/CAMetalLayer.h> // Metal based core animation layer

// global function declarations
CVReturn displayLinkCallback(CVDisplayLinkRef, const CVTimeStamp*, const CVTimeStamp* , CVOptionFlags , CVOptionFlags* , void*);

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
    // code
    CALayer*layer = [[[self class] layerClass] layer];

    CGSize viewSize = [self convertSizeToBacking: CGSizeMake(1.0, 1.0)];

    [layer setContentsScale: min(viewSize.width, viewSize.height)];
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

// global function definitions
CVReturn displayLinkCallback(CVDisplayLinkRef myDisplayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* renderer)
{
    // code
    CVReturn result = [(View*)renderer getFrameForTime:outputTime];

    return result;
}




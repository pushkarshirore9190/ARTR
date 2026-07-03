//
//  ViewController.m
//  Template
//
//  Created by Pushkar Rajendra Shirore on 17/06/26.
//

#import "ViewController.h"
#import "View.h"

@interface ViewController ()
{
    CADisplayLink *displayLink;
}
@end

@implementation ViewController

-(void)loadView
{
    // code
    View *view = [[View alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    [self setView:view];
    [view release];
}

-(void) viewDidLoad
{
    // code
    [super viewDidLoad];

    // start the render loop
    displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(gameLoop:)];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];

    // don't render until app is actually foregrounded/active
    displayLink.paused = YES;

    [[NSNotificationCenter defaultCenter] addObserver:self
                                              selector:@selector(appDidBecomeActive)
                                                  name:UIApplicationDidBecomeActiveNotification
                                                object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                              selector:@selector(appWillResignActive)
                                                  name:UIApplicationWillResignActiveNotification
                                                object:nil];
}

-(void)appDidBecomeActive
{
    // code
    displayLink.paused = NO;
}

-(void)appWillResignActive
{
    // code
    displayLink.paused = YES;
}

-(void)gameLoop:(CADisplayLink *)sender
{
    // self.view must actually be your Vulkan View instance
    //[(View *)self.view drawView];
}

// to make our scene landscape only follwing code will assist the setting we did in general tab

-(UIInterfaceOrientationMask)supportedInterfaceOrientations
{
    // code
    return UIInterfaceOrientationMaskLandscape;
}

// hide status bcoz we are going to fullscreen always and it will also assist that we did in general tab

-(BOOL)prefersStatusBarHidden
{
    return YES;
}

-(void)dealloc
{
    // code
    [[NSNotificationCenter defaultCenter] removeObserver:self];

    [displayLink invalidate];
    displayLink = nil;

    [super dealloc];
}

@end



//
//  SceneDelegate.m
//  Template
//
//  Created by Pushkar Rajendra Shirore on 17/06/26.
//

#import "SceneDelegate.h"
#import "ViewController.h"
#import "View.h"

@implementation SceneDelegate
{
@private
    UIWindow *window;
}

- (void)scene:(UIScene *)scene
willConnectToSession:(UISceneSession *)session
      options:(UISceneConnectionOptions *)connectionOptions
{
    // code

    UIWindowScene *windowScene = nil;

    UIViewController *viewController = nil;

    View *view = nil;

    CGRect rect;

    // check whether scene returned by system is of our required type

    if ([scene isKindOfClass:[UIWindowScene class]] == NO)
    {
        return;
    }

    // if valid assign it to our local variable
    windowScene = (UIWindowScene *)scene;

    // create the window bound to this scene
    window = [[UIWindow alloc] initWithWindowScene:windowScene];

    // set the background color window to black
    [window setBackgroundColor:[UIColor blackColor]];

    // create our custom view controller which will work as main view controller
    // for all views is called as root view controller

    viewController = [[UIViewController alloc] init];

    // set this view controller as window's root view controller
    [window setRootViewController:viewController];

    // window increments its reference count when assigned to root view controller
    // to balance call release
    [viewController release];

    // create our custom view = size of scene bound
    rect = [[[windowScene effectiveGeometry]coordinateSpace]bounds];

    view = [[View alloc] initWithFrame:rect];

    // set this view as view controller's view
    [viewController setView:view];

    // window increments its reference count when assigned to root view
    // to balance call release

    [view release];

    // make this window with focus and visible
    [window makeKeyAndVisible];
}

- (void)sceneDidDisconnect:(UIScene *)scene
{
    // code
}

- (void)sceneDidBecomeActive:(UIScene *)scene
{
    // code
}

- (void)sceneWillResignActive:(UIScene *)scene
{
    // code
}

- (void)sceneWillEnterForeground:(UIScene *)scene
{
    // code
}

- (void)sceneDidEnterBackground:(UIScene *)scene
{
    // code
}

- (void)dealloc
{
    // code
    [window release];
    [super dealloc];
}

@end





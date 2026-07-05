//
//  AppDelegate.m
//  Template
//
//  Created by Pushkar Rajendra Shirore on 17/06/26.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"

@implementation AppDelegate

-(BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{
    // code
    return YES;
}

-(UISceneConfiguration*)application:(UIApplication*)application configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession options:(UISceneConnectionOptions *)options

{
    // code
    return [[[UISceneConfiguration alloc]initWithName:@"Default Configuration" sessionRole:(connectingSceneSession.role)]autorelease];
}

-(void)application:(UIApplication*)application didDiscardSceneSessions:(NSSet<UISceneSession *> *)sceneSessions
{
    // code
}

-(void)dealloc
{
    // code
    [super dealloc];
}

@end

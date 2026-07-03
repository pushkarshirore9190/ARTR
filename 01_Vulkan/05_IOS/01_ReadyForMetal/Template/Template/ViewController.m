//
//  ViewController.m
//  Template
//
//  Created by Pushkar Rajendra Shirore on 17/06/26.
//


#import "ViewController.h"

@implementation ViewController

-(void) viewDidLoad
{
    // code
    [super viewDidLoad];
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
    [super dealloc];
}


@end


//
//  View.h
//  Template
//
//  Created by Pushkar Rajendra Shirore on 18/06/26.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <MoltenVK/mvk_vulkan.h>

@interface View : UIView <UIGestureRecognizerDelegate>
{
@private
    CADisplayLink *displayLink;
}

+(Class)layerClass;

-(id)initWithFrame:(CGRect)frameRect;

-(void)drawView;

-(void)update;

-(void)pauseRendering;

-(void)resumeRendering;

-(void)uninitialise;

-(VkResult)initialise;

-(VkResult)resize:(int)width :(int)heigth;

-(VkResult)render;

@end



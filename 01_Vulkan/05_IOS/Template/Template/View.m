//
//  View.m
//  Template
//
//  Created by Pushkar Rajendra Shirore on 18/06/26.
//

#import "View.h"

// global variable declarations
int winWidth = 0;
int winHeight = 0;

@implementation View

-(id)initWithFrame:(CGRect)frameRect
{
    // variable declarations
    UITapGestureRecognizer *singletabGestureRecognizer = nil;
    UITapGestureRecognizer *doubleTabGestureRecognizer = nil;
    UISwipeGestureRecognizer *swipeGestureRecognizer = nil;
    UILongPressGestureRecognizer *longPressGestureRecognizer = nil;

    int Result = 0;

    // code
    self = [super initWithFrame:frameRect];
    if(self)
    {
        winWidth = self.bounds.size.width;
        winHeight = self.bounds.size.height;

        // initialise
        Result = [self initialise];
        if(Result != 0)
        {
            printf("initialise failed");
        }
        else
        {
            printf("initialise succeeded");
        }
        
        // respond to events
        [self becomeFirstResponder];

        // gesture recognization

        // single tap
        singletabGestureRecognizer = [[UITapGestureRecognizer alloc]
                                      initWithTarget:self
                                      action:@selector(onSingleTap:)];

        [singletabGestureRecognizer setNumberOfTapsRequired:1];

        [singletabGestureRecognizer setNumberOfTouchesRequired:1];

        [singletabGestureRecognizer setDelegate:self];

        [self addGestureRecognizer:singletabGestureRecognizer];
        
        // double tap
        doubleTabGestureRecognizer = [[UITapGestureRecognizer alloc]
                                      initWithTarget:self
                                      action:@selector(onDoubleTap:)];

        [doubleTabGestureRecognizer setNumberOfTapsRequired:2];

        [doubleTabGestureRecognizer setNumberOfTouchesRequired:1];

        [doubleTabGestureRecognizer setDelegate:self];

        [self addGestureRecognizer:doubleTabGestureRecognizer];
        
        // late single tap wait double tap to fail so that the two get diffrenciated
        [singletabGestureRecognizer requireGestureRecognizerToFail:(doubleTabGestureRecognizer)];
        
        // swipe
        swipeGestureRecognizer = [[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
        
        [swipeGestureRecognizer setDelegate:self];
        
        [self addGestureRecognizer:swipeGestureRecognizer];
        
        // long press
        longPressGestureRecognizer = [[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
        
        [self addGestureRecognizer:longPressGestureRecognizer];
        
        // remember when you register your gesture recognizer object to the view means to self it retains it by incrementing the refrence count so we must decrement it
        [longPressGestureRecognizer release];
        longPressGestureRecognizer = nil;
        
        [swipeGestureRecognizer release];
        swipeGestureRecognizer = nil;
        
        [doubleTabGestureRecognizer release];
        doubleTabGestureRecognizer = nil;
        
        [singletabGestureRecognizer release];
        singletabGestureRecognizer = nil;
        
    }
    
    return self;
}

// implement this function only if you have custome deawing not in rendering and still if you kepp it empty without commenting performance hurts during animation because to painting events cuncurrenlty get called one is maintreads drawrect and another is renderer's render

/*-(void)drawRect:(CGRect)rect
{
    //code
}*/


-(void)layoutSubviews
{
    // code
    [super layoutSubviews];

    // set changed winWidth and winHeight according to View's layout
    winWidth = [self bounds].size.width * [self contentScaleFactor];
    winHeight = [self bounds].size.height * [self contentScaleFactor];

    // call our resize here
    [self Resize:winWidth :winHeight];
}

-(BOOL)canBecomeFirstResponder
{
    // code
    return YES;
}

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    // code
}

-(void)onSingleTap:(UITapGestureRecognizer *)gestureRecognizer
{
    // code
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gestureRecognizer
{
    // code
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gestureRecognizer
{
    // code
    [self uninitialise];
    exit(0);
}

-(void) dealloc
{
    // code
    [super dealloc];
}


-(void)onLongPress:(UITapGestureRecognizer *)gestureRecognizer
{
    // code
}

-(int)initialise
{
    // code
    return 0;
}

-(void)Resize:(int)width :(int)height
{
    // code
}

-(void)render
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
}

@end



#ifndef CORE_SDL
#include "environment.h"
#ifdef PLATFORM_GUI_IOS // Not SDL build; not console build

// VidWindow.m - July 31-August 2 2015

#import "core.h"
#import "core_opengl.h"
#import "VidUIWindow.h"       // Courtesy
#import "iphonequake.h"
#import "quakedef.h"

@implementation myView



#define logc Con_PrintLinef


int touches_count;

UIWindow *mine;

void *cw = NULL;
void *ptr = NULL;

- (void)setProp:(void *)pcw withPtr:(void *)pptr
{
    cw = pcw;
	ptr = pptr;
}


- (void) setUIWindow: (UIWindow *) uiWindow;
{
	mine = uiWindow;
}

- (CGRect) contentFrame
{
	CGRect rect = [self.view frame]; 
	return rect;
}


#if 1
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	return (interfaceOrientation == UIDeviceOrientationPortrait);
}

- (BOOL)shouldAutorotate {
  return NO; // Don't rotate // Hit Hit again
}


#else

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	return (interfaceOrientation == UIInterfaceOrientationLandscapeLeft);
}

- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation {
  return UIInterfaceOrientationLandscapeLeft; // or Right of course
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations {
  return UIInterfaceOrientationMaskLandscape; // Hit Hit again
}
#endif // It aint workin

- (BOOL)prefersStatusBarHidden {return YES;}

static int shiftbits (void)
{
//    NSUInteger shifts = [[NSApp currentEvent] modifierFlags]; // unsigned long, which is might be 64 bit. tocanvas

    // NSNumericPadKeyMask NSAlphaShiftKeyMask
    int shifted = 0; // Flag_Check_Bool (shifts, NSShiftKeyMask);
    int ctrled  = 0; // Flag_Check_Bool (shifts, NSControlKeyMask);
    int alted   = 0; // Flag_Check_Bool (shifts, NSAlternateKeyMask);   // Option
    int guied   = 0; // Flag_Check_Bool (shifts, NSCommandKeyMask);

    return shifted + ctrled * 2 + alted * 4 + guied * 8;
}


// getmousebits
static void getmousebits (required int *button_bits, required int *shift_bits) //, required int *x, required int *y)
{
    int m1 = touches_count > 0; // Number of fingers down is button
    int m2 = touches_count > 1;
    int m3 = touches_count > 2;
    int m4 = touches_count > 3;
    int m5 = touches_count > 4;
	
	REQUIRED_ASSIGN (shift_bits, shiftbits());
    REQUIRED_ASSIGN (button_bits,  m1 * 1 + m2 * 2 + m3 * 4 + m4 * 8 + m5 * 16);
//    *x = p3.x;
//    *y = p3.y;
	
}


NSArray *mcommands;



// In a view or view controller subclass:
- (BOOL)canBecomeFirstResponder
{
    return YES;	// Part of keypress
}

//- (void)viewDidLoad {
//    keyDown = NO;
//    [super viewDidLoad];
//}

- (void)addKeyCommandsToArray:(NSMutableArray *)array key:(NSString *)key
{
	int base = UIKeyModifierAlphaShift; // 64 combinations per key?
	
	SEL s = @selector(keyCommandTriggered:);
	for (int n = 0; n < 64; n ++) {
		[array addObject:[UIKeyCommand keyCommandWithInput:key modifierFlags:(n * base) action:s]];
	}
	
	NSLog (@"Added %@", key);
}

- (void) init_keycommands
{
	
	int modifierFlags = 0
		| UIKeyModifierAlphaShift     /* = 1 << 16 */  /* This bit indicates CapsLock */
		| UIKeyModifierShift          /* = 1 << 17 */
		| UIKeyModifierControl        /* = 1 << 18 */
		| UIKeyModifierAlternate      /* = 1 << 19 */
		| UIKeyModifierCommand        /* = 1 << 20 */
		| UIKeyModifierNumericPad     /* = 1 << 21 */ ;
	
	int base = UIKeyModifierAlphaShift; // 64 combinations per key?
	
	
	NSMutableArray *commands = [[NSMutableArray alloc] init]; // \b backspace, \r enter, \t tab
    NSString *characters = @"\b\r\t`1234567890-=qwertyuiop[]asdfghjkl;'zxcvbnm,./\\ ";
	
	[self addKeyCommandsToArray:commands key:@""]; // Bug-fix, right?
	for (NSInteger i = 0; i < characters.length; i++) {
		NSString *keychar = [characters substringWithRange:NSMakeRange(i, 1)];
		[self addKeyCommandsToArray:commands key:keychar];
	}
	
	[self addKeyCommandsToArray:commands key:UIKeyInputUpArrow];
	[self addKeyCommandsToArray:commands key:UIKeyInputDownArrow];
	[self addKeyCommandsToArray:commands key:UIKeyInputLeftArrow];
	[self addKeyCommandsToArray:commands key:UIKeyInputRightArrow];
	[self addKeyCommandsToArray:commands key:UIKeyInputEscape];
	
	mcommands = commands.copy; // commands goes out of scope in arc and goes bye bye
	NSLog (@"Initialed keys");
}

bool keyDown;


- (NSArray *)keyCommands
{
	vid.mobile_keyup ++;
	//NSLog (@"Key EVENT ? %d", vid.mobile_keyup);
	// Gayness about keydown
	if ([mcommands count] == 0) {
		NSLog (@"Initializing keys ...");
		[self init_keycommands];
	}
	
	//NSLog (@"keyCommands");
    return mcommands;
}

- (void)keyCommandTriggered:(UIKeyCommand *)command
{
    UIKeyModifierFlags modifierFlags = command.modifierFlags;
    NSString *input = command.input;
	
	vid.mobile_keyup --;
	//NSLog (@"Keydown %d", vid.mobile_keyup);
	
    //NSMutableString *modifierSymbols = [[NSMutableString alloc] init];
    //NSMutableString *inputCharacters = [[NSMutableString alloc] init];
	
	char asciirep[16];
	c_strlcpy (asciirep, TO_CSTRING(input)  );
	int shiftoid = 0;
	
    if ((modifierFlags & UIKeyModifierAlphaShift) == UIKeyModifierAlphaShift) {
        //[modifierSymbols appendString:@"CAPSLOCK "];
		shiftoid = 0;
    }
    if ((modifierFlags & UIKeyModifierShift) == UIKeyModifierShift) {
        //[modifierSymbols appendString:@"SHIFT "];
		shiftoid = K_LSHIFT;
    }
    if ((modifierFlags & UIKeyModifierControl) == UIKeyModifierControl) {
        //[modifierSymbols appendString:@"CTRL "];
		shiftoid = K_LCTRL;
    }
    if ((modifierFlags & UIKeyModifierAlternate) == UIKeyModifierAlternate) {
        //[modifierSymbols appendString:@"OPTION "];
		shiftoid = K_LALT;
    }
    if ((modifierFlags & UIKeyModifierCommand) == UIKeyModifierCommand) {
        //[modifierSymbols appendString:@"COMMAND "];
		shiftoid = K_LWIN;
    }

	if (shiftoid) {
		int scancode = shiftoid;
		// We aren't doing shift support right now so no SHIFT, CTRL, COMMAND ... yet
		Key_Event_Ex (NO_WINDOW_NULL, scancode, true,  ASCII_0 , UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
		// Down comes later!  Should allow limited shifting.
	}


	int ascii = asciirep[0];
	if (!in_range(1, ascii, 127)) {
		//NSLog (@"Ascii was out range %d", ascii);
		ascii = 0;
	}

	
//    if ([input isEqualToString:@"\b"]) {
//        [inputCharacters appendFormat:@"%@", @"DEL"];
//    }
//    if ([input isEqualToString:@"\t"]) {
//        [inputCharacters appendFormat:@"%@", @"TAB"];
//    }
//    if ([input isEqualToString:@"\r"]) {
//        [inputCharacters appendFormat:@"%@", @"ENTER"];
//    }
    if (input == UIKeyInputUpArrow) {
        //[inputCharacters appendFormat:@"%@", @"UPARROW"];
		ascii = K_UPARROW;
    }
    if (input == UIKeyInputDownArrow) {
        //[inputCharacters appendFormat:@"%@", @"DOWNARROW"];
		ascii = K_DOWNARROW;
    }
    if (input == UIKeyInputLeftArrow) {
        //[inputCharacters appendFormat:@"%@", @"LEFTARROW"];
		ascii = K_LEFTARROW;
    }
    if (input == UIKeyInputRightArrow) {
        //[inputCharacters appendFormat:@"%@", @"RIGHTARROW"];
		ascii = K_RIGHTARROW;
    }
    if (input == UIKeyInputEscape) {
        //[inputCharacters appendFormat:@"%@", @"ESC"];
		ascii = K_ESCAPE;
    }
    
//    if (input.length > 0 && inputCharacters.length == 0) {
//        [inputCharacters appendFormat:@"%@", input.uppercaseString];
//    }
//    
//    if (modifierSymbols.length > 0) {
//        NSLog (@"Key event %@ (ascii %d)", [NSString stringWithFormat:@"%@ + %@", modifierSymbols, inputCharacters], ascii);
//    } else {
//        NSLog (@"Key event %@ (ascii %d)", [NSString stringWithFormat:@"%@", inputCharacters], ascii);
//    }
	
	if (ascii) {
		int scancode = toupper (ascii);
		// We aren't doing shift support right now so no SHIFT, CTRL, COMMAND ... yet

		if (scancode < SPACE_CHAR_32 && !isin4 (scancode, K_BACKSPACE, K_TAB, K_ENTER, K_ESCAPE)) {
			scancode = 0; // If isn't one of the above, just nuke it
		}
		else {
			Key_Event_Ex (NO_WINDOW_NULL, scancode, true,  ASCII_0 , UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
			
#if 0 // MULTIKEY
			Key_Event_Ex (NO_WINDOW_NULL, scancode, false, ASCII_0 , UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
#endif // MULTIKEY
		}

	}

#if 0 // MULTIKEY
	if (shiftoid) {
		int scancode = shiftoid;
		//Key_Event_Ex (NO_WINDOW_NULL, scancode, true,  ASCII_0 , UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
		Key_Event_Ex (NO_WINDOW_NULL, scancode, false, ASCII_0 , UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1, -1, -1);
	}
#endif
	
}

- (void) display
{
}



// Consider this static
-(void) touchGetCoord:(UITouch *) touch fullSet:(NSSet *)touches touchStamp:(void **)touch_stamp x:(int *)x y:(int *)y
{
	CGPoint _clickPoint 	= [touch locationInView:self.view];	// Extract the in-view coordinates of the touch location
	CGPoint clickPoint = [mine convertPoint:_clickPoint toWindow:nil];

	CGRect rect			= [self.view bounds]; 

	//Eek!  bounds and frame are not same.  Should we have used frame whole time?  Wouldn't have made clickpoint work though
	//But let's kill bounds, that's stupud

	float screen_height = rect.size.height; // This could be very specific to the ios version on this ipod touch.
	if (sysplat.iphone_device_type == device_type_ipodtouch)
		screen_height -= (sysplat.statusbar_height / 2 - 1);
	//logc ("%g", clickPoint.y);
	float final_point_y = screen_height - c_rint(clickPoint.y);
	
	// Not telling us anything useful ... NSLog(@"id %d Phase %d Tap count %d Type %d",
	// (int)touch_stamp, (int) touch.phase, (int) touch.tapCount, (int) touch.timestamp);
	
	// March 14 2018 - Potential improvement would be to iterate [[event allTouches]count]
	
	REQUIRED_ASSIGN (x, c_rint(clickPoint.x));
	REQUIRED_ASSIGN (y, rect.size.height - final_point_y - 1); // - 1?
	
	if (vid.is_screen_portrait) { // And it is ...
		int rawx_orig = (*x);
		(*x) = vid.desktop.height - (*y);
		(*y) = rawx_orig;
		
#ifdef WINQUAKE_RENDERER_SUPPORT
		// For WinQuake, we are running with a 320x240 ... convert the coords to that
		(*x) = (*x) * (320.0 / vid.desktop.height);
		(*y) = (*y) * (240.0 / vid.desktop.width);
#endif // WINQUAKE_RENDERER_SUPPORT
	}
	
	REQUIRED_ASSIGN (touch_stamp, (__bridge void *) touch);	
}

-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{	
	NSArray *allTouches = [touches allObjects];
    for (int n = 0; n < [touches count]; n++) {
		UITouch *toucho = allTouches[n];
		void *touch_stamp; int x, y; [self touchGetCoord:toucho fullSet:touches touchStamp:&touch_stamp x:&x y:&y];

        Touch_Action (touch_stamp, mouseaction_move, x, y);
	} // end for
}



-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event 
{
	NSArray *allTouches = [touches allObjects];
    for (int n = 0; n < [touches count]; n++) {
		UITouch *toucho = allTouches[n];
		void *touch_stamp; int x, y; [self touchGetCoord:toucho fullSet:touches touchStamp:&touch_stamp x:&x y:&y];

        Touch_Action (touch_stamp, mouseaction_down_0, x, y);
	} // end for
}

-(void) touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	NSArray *allTouches = [touches allObjects];
    for (int n = 0; n < [touches count]; n++) {
		UITouch *toucho = allTouches[n];
		void *touch_stamp; int x, y; [self touchGetCoord:toucho fullSet:touches touchStamp:&touch_stamp x:&x y:&y];
		
        Touch_Action (touch_stamp, /*mouseaction_cancelled*/ mouseaction_up, x, y);
	} // end for
}


-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event 
{
	NSArray *allTouches = [touches allObjects];
    for (int n = 0; n < [touches count]; n++) {
		UITouch *toucho = allTouches[n];
		void *touch_stamp; int x, y; [self touchGetCoord:toucho fullSet:touches touchStamp:&touch_stamp x:&x y:&y];

        Touch_Action (touch_stamp, mouseaction_up, x, y);
	} // End for
}






@end


@implementation CoreVidWindow

#define cw vcw
#define ptr vptr
void *cw;
void *ptr;

//- (id) init
//{
////    self = [super init];
////    
////    if (self != nil)
////    {
////        [self doesNotRecognizeSelector: _cmd];
////        [self release];
////    }
////    
////    return nil;
//}

//- (void) dealloc
//{
////	[mView release];
////    [super dealloc];
//}

- (void) swapBufferz
{
// This does nothing.  Rewrite time :(
//	[self.view.context presentRenderbuffer:GL_RENDERBUFFER]; // Does nothing?
//	[self.view display];
//	[((GLKView *) self.view) update];
//	[((GLKView *) self.view) swapBuffers];
// This will wait to finish
//NSLog (@"Done1");
//	dispatch_async(dispatch_get_main_queue(), ^{
//		// Update the UI on the main thread.
//	});
//	NSLog (@"Done2");

}

#pragma mark - GLKViewDelegate
 
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
 

//	session_frame(0);
 
}

#pragma mark - GLKViewControllerDelegate
 
- (void)glkViewControllerUpdate:(GLKViewController *)controller {

}



- (void) setOpenGLContext: (EAGLContext*) newOpenGLContext
{
	// Do nothing!  Right?
}


- (EAGLContext *) getOpenGLContext
{
    return mEAGLContext;
}

- (UIWindow *) window
{
	return mUIWindow;
}

- (GLKView *) view
{
	return mGLKView;

}



//// I don't think we ask for these notifications because we don't really care, at least not yet.
//-(void)windowDidMove:(NSNotification *)notification
//{ logd (__func__);
//	misMoving = NO;
//}
//
//- (void)windowWillMove:(NSNotification *)notification
//{ logd (__func__);
//
//    misMoving = YES;
//    
//}

//- (int) getIsMoving
//{
//    return misMoving;
//}
//
//
//
//- (BOOL) acceptsFirstResponder  {   return YES;     }
//- (BOOL) canBecomeMainWindow    {   return YES;     }
//- (BOOL) canBecomeKeyWindow     {   return YES;     }
//- (BOOL) canHide                {   return YES;     }

//- (void) windowWillClose:(NSNotification *)notification
//{ logd (__func__);
//    [[NSNotificationCenter defaultCenter] removeObserver: self name: NSWindowWillMoveNotification object: nil];
//	[[NSNotificationCenter defaultCenter] removeObserver: self name: NSWindowDidMoveNotification object: nil];
//	[[NSNotificationCenter defaultCenter] removeObserver: self name: NSWindowWillCloseNotification object: nil];
//}

//- (BOOL) windowShouldClose: (id) sender
//{
//    // Under what circumstances is this called? We allow the fullscreen to be closed, but not the windowed?  Ok, whatever.
////    const BOOL  shouldClose = mAmFullScreen; // Was [self isFullscreen] If mDisplay, we are fullscreen
//    
// //   if (shouldClose == NO)
////        [NSApp terminate: nil]; // Why are we terminating?
//    
//    return YES; //shouldClose;
//}















- (void) destroyWindow
{
// Kill all our observers
#pragma message ("Wouldn't it be nice if you killed your observers")
#pragma message ("Wouldn't it be nice if you killed your observers")
#pragma message ("Wouldn't it be nice if you killed your observers")
	mGLKView = nil;
//	[mUIWindow close];
	mUIWindow = nil;
}

//- (UIWindow *) window
//{
//	return mUIWindow;
//}





//
//
// Create Window
//
//
- (id) createWindow: (CGRect) clientRect multisamples: (int) multisamples style: (int) style withPtr: (void *)ptr
{
	int plat_style		= _Shell_Window_Style   (style);
	int plat_style_ex	= _Shell_Window_StyleEx (style);

//	[[UIApplication sharedApplication] setStatusBarHidden:YES];

	// We only care about fullscreen because of exclusive display, we use the size provided as is.
    mUIWindow = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]]; // initWithContentRect: clientRect
	
	CGRect  viewRect = CGRectMake(0, 0, clientRect.size.width, clientRect.size.height); // Frame is fullscreen whether we like or not
	mGLKView = [[GLKView alloc] initWithFrame:viewRect] ;//]clientRect ]; // So that's that!

	mGLKView.contentScaleFactor = sysplat.content_scale ? sysplat.content_scale : 1; // Doesn't affect mouse or touches right?
	
	EAGLContext *mOpenGLContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];

    mGLKView.context = mOpenGLContext;
    mGLKView.delegate = self;

	
    mGLKView.drawableDepthFormat = GLKViewDrawableDepthFormat24;
	mGLKView.drawableStencilFormat = GLKViewDrawableStencilFormat8;

	[EAGLContext setCurrentContext:mGLKView.context ]; // Make current equivalent?
	
    mGLKViewController = [[myView /*GLKViewController*/ alloc] initWithNibName:nil bundle:nil]; // 1
	mGLKView.multipleTouchEnabled = YES;
    mGLKViewController.view = mGLKView; // 2
    mGLKViewController.delegate = self; // 3
    mGLKViewController.preferredFramesPerSecond = sysplat.fps_desired; // c_rint(1.0 / sysplat.max_wait_seconds); // 4 mainus_t
	// Interestly, we set up a timer in UIDelegate and I bet it bumps heads?

	[mGLKViewController setUIWindow: mUIWindow];
	cw = (__bridge void *)(self);
	[mGLKViewController setProp: cw withPtr:ptr];
	
    mUIWindow.rootViewController = mGLKViewController; // 5
 
    mUIWindow.backgroundColor  =[UIColor blackColor];
	
	[mUIWindow makeKeyAndVisible]; // Show + Raise but whatever
    

    return self;
}


@end

#endif // PLATFORM_GUI_IOS
#endif // ! CORE_SDL


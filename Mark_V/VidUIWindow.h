// VidUIWindow.h - July 31-August 2 2015


#if 0
	//#import <UIKit/UIKit.h>
	//#import <QuartzCore/QuartzCore.h>

	@interface myView : UIView {
	@private
//		IApplicationEngine* m_applicationEngine;
//		IRenderingEngine* m_renderingEngine;
		EAGLContext* m_context;
//		float m_timestamp;
	}

	- (void) drawView: (CADisplayLink*) displayLink;
	- (void) setUIWindow: (UIWindow *) uiWindow;			// +Baker
	- (void) setProp:(void *)pcw withPtr:(void *)pptr;		// +Baker
	- (CGRect) contentFrame;								// +Baker

	@end
#else
	@interface myView : GLKViewController
	- (void) setUIWindow: (UIWindow *) uiWindow;
	- (void)setProp:(void *)pcw withPtr:(void *)pptr;
	- (CGRect) contentFrame;
	- (void) display;
	@property (strong, nonatomic) EAGLContext *context;

	@end
#endif

@interface CoreVidWindow : UIResponder <GLKViewDelegate, GLKViewControllerDelegate>
{

	
@private
	UIWindow			*mUIWindow;
    GLKView				*mGLKView;
	myView				*mGLKViewController;
	EAGLContext			*mEAGLContext;
}


- (id) createWindow: (CGRect) clientRect multisamples: (int) multisamples style: (int) style withPtr: (void *)ptr;
// There is a close method, but we don't need to do anything in particular AFAIK
//- (int) getIsMoving;
- (void) setOpenGLContext: (EAGLContext*) newOpenGLContext;
- (EAGLContext *) getOpenGLContext;
- (void) swapBufferz;
- (UIWindow *) window;
- (GLKView *) view;

- (void) destroyWindow;
//- (void) keyDown: (NSEvent *) event;

//- (void)windowWillMove:(NSNotification *)notification; // Start
//- (void)windowDidMove:(NSNotification *)notification;  // End


@end

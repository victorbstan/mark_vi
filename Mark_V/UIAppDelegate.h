// AppDelegate.h - July 31-August 2 2015


@interface AppDelegate : UIResponder <UIApplicationDelegate>
{
    NSTimer	*mFrameTimer; // Any reason for this to be exposed?
    BOOL    mSessionRunning; // For possible delayed execution app.  When would we use this?
}

//+ (void) initialize;
//- (id) init;
//- (void) dealloc;


//- (NSApplicationTerminateReply) applicationShouldTerminate: (NSApplication*) sender;
//
//- (void) applicationWillBecomeActive:(NSApplication *)app;
//- (void) applicationWillResignActive:(NSApplication *)app;
//
//- (BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)app;
//
//- (void) applicationDidFinishLaunching: (NSNotification*) notification;
//- (void) applicationWillTerminate: (NSNotification*) notification;
//- (void) applicationWillHide: (NSNotification *) notification;
//- (void) applicationWillUnhide: (NSNotification *) notification;


- (void) setSessionRunning: (BOOL) state;
- (BOOL) getSessionRunning;

@end




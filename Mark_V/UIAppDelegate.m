#ifndef CORE_SDL
// UIAppDelegate.m - July 31-August 2 2015

#include "environment.h"
#ifdef PLATFORM_GUI_IOS

#import "quakedef.h"
#import "core.h"
#import "UIAppDelegate.h" // Courtesy
#import "iphonequake.h"


@implementation AppDelegate


//static NSTimer*	mFrameTimer;
//static BOOL     mQuakeRunning;
static double	oldtime;




- (void)applicationDidEnterBackground:(UIApplication *)application
{
	// Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
	// If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
	NSLog (@"applicationDidEnterBackground");
	// Happens 2nd
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
	// Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
	NSLog (@"applicationWillEnterForeground");
	// Happens first
}


- (void) setSessionRunning: (BOOL) theState
{
    mSessionRunning = theState;
}


- (BOOL) getSessionRunning
{
    return mSessionRunning;
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	// Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
	NSLog (@"applicationDidBecomeActive"); // Hit
	// Happens 2nd.
}

- (void)applicationWillResignActive:(UIApplication *)application
{
	// Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
	// Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
	NSLog (@"applicationWillResignActive");
	if (vid.mobile_keyup && host_initialized) {
		// Deactivate event ... although there is not much we must do.
		Key_Release_Keys(NULL);  // Define the order
	}
	// Happens first
}



// See main_central.  Mac equivalent is DoFrame in QController.m
- (void) mainCentralFrame: (NSTimer*) timer
{
    // if ([NSApp isHidden] == NO) ... no that's a Mac thing
	
	double  newtime = System_DoubleTime();
	double  timeslice = newtime - oldtime;

	if (vid.mobile_keyup) {
//		NSLog (@"Keyup count %d", vid.mobile_keyup);
		if (host_initialized) {
			Key_Release_Keys(NULL);
			if (vid.mobile_keyup > 1) // If 1, then it is very likely a proper key release
				S_LocalSound ("hknight/hit.wav"); // Sound hint
			vid.mobile_keyup = 0;
		}
	}

	Host_Frame (timeslice);

	oldtime = newtime;
}


// mainCentralEnd
- (void)applicationWillTerminate:(UIApplication *)application
{
//	// Remember a pure "exit" won't touch this, only an object oriented exit
//    if ([self getSessionRunning])
//        session_end ();
}

//void testme (void) {
////App Directory & Directory Contents
//   NSString *appFolderPath = [[NSBundle mainBundle] resourcePath];
//   NSString *pappFolderPath = [appFolderPath stringByDeletingLastPathComponent];
//	
//	
//   NSFileManager *fileManager = [NSFileManager defaultManager];  
//   NSLog(@"App Directory is: %@", appFolderPath);
//   NSLog(@"Directory Contents:\n%@", [fileManager directoryContentsAtPath: appFolderPath]);
//
//	
//	NSLog(@"PAre Directory is: %@", pappFolderPath);
//   NSLog(@"Directory Contents:\n%@", [fileManager directoryContentsAtPath: pappFolderPath]);
//	
//   NSString *s = [pappFolderPath stringByAppendingString:@"/Documents"];
//	NSLog(@"doc Directory is: %@", s);
//	   NSLog(@"Directory Contents:\n%@", [fileManager directoryContentsAtPath: s]);
//	
//	NSLog(@"%@",[[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject]);
//	
//	const char *ss = TO_CSTRING(s);
//	NSLog(@"test file path Directory is: %s", ss);
//	const char *sx2 = va("%s/testotum.txt", ss);
//	NSLog(@"test file to write: %s", sx2);
//	NSLog(@"Wrote ok = %s %i", sx2, File_String_To_File (sx2,"test"));
//	
//	NSLog(@"doc Directory is: %@", s);
//	   NSLog(@"Directory Contents:\n%@", [fileManager directoryContentsAtPath: s]);
//	
//}
const char *File_Binary_URL (void);
const char *File_iPhone_Documents_URL (void);
const char *File_iPhone_BundlePath_URL (void);
const char *File_iPhone_AppFolder_URL (void);

- (NSURL *)applicationDocumentsDirectory
{
    NSLog(@"%@",[[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory  inDomains:NSUserDomainMask] lastObject]);

    return [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject];
 }

- (void) mainCentralStart: (id) sender
{
//	testme ();
	
	NSLog (@"binary url %s", File_Binary_URL ());
	NSLog (@"documents %s", File_iPhone_Documents_URL ());
	NSLog (@"bindle path %s", File_iPhone_BundlePath_URL ());
	NSLog (@"appl folder %s", File_iPhone_AppFolder_URL ());
	
	// If documents folder doesn't contain pak0, copy the bundle version.
	const char *pak0_docs_url = va ("%s/id1/pak0.pak", File_iPhone_Documents_URL() );
	NSLog (@"Pak check location %s", pak0_docs_url);
	if (!File_Exists(pak0_docs_url)) {
		// Need this function to do Core_Init to enable core file write functionality.
		void Main_Central_LT (char *cmdline, sys_handle_t *pmain_window_holder_addr, cbool do_loop);
		Main_Central_LT (sysplat.lpCmdLine, &sysplat.CoreVidWindowObj, false /* we perform loop ourselves */);
		//#pragma message ("&sysplat.CoreVidWindowObj ?  Why the '&'?  Are we trying to track it?") .. March 21 2018 - YES

		const char *pak0_app_bundle_url = va ("%s/pak0.pak", File_iPhone_BundlePath_URL());
		NSLog (@"Unable to locate shareware at %s", pak0_docs_url);
		NSLog (@"Attempt to copy %s to %s", pak0_app_bundle_url, pak0_docs_url);
		
		if (!File_Copy (pak0_app_bundle_url, pak0_docs_url))
			System_Error ("Able to copy pak0.pak into place %s to %s", pak0_app_bundle_url, pak0_docs_url);
		
		NSLog (@"Copied pak0.pak bundle into place %s!", pak0_docs_url);
	}
	
 #ifdef DEBUG
//	c_strlcpy (sysplat.lpCmdLine, "+map e1m1 -noudp6 +viewsize 120"); // Right?
//	c_strlcpy (sysplat.lpCmdLine, "+scr_showpos 1"); // Right?
	NSLog (@"set command line");
 #else
//	c_strlcpy (sysplat.lpCmdLine, "Quake"); // Right?
	NSLog (@"set command line");
 #endif
	
	Main_Central (sysplat.lpCmdLine, &sysplat.CoreVidWindowObj, false /* we perform loop ourselves */);

#if 0
	if (isDedicated)
		System_Error ("Please use Mark V dedicated server build which uses a Terminal interface");
#endif
			
	oldtime = System_DoubleTime ();
	
    
    [self setSessionRunning: YES];

//  Seems to indicate services for other apps.
//    [NSApp setServicesProvider: self]; // What's this do ?  Is this related to the timer?

	
	mFrameTimer = [NSTimer scheduledTimerWithTimeInterval: 1/sysplat.fps_desired /*  0.0003f*/
                                                       target: self
                                                     selector: @selector (mainCentralFrame:)
                                                     userInfo: nil
                                                      repeats: YES];
        


	// We have a possible object instance collision going on here.
	[[NSRunLoop currentRunLoop] addTimer: mFrameTimer forMode: UITrackingRunLoopMode];
	
	oldtime = System_DoubleTime ();
#if 0
	mainus.realtime = Platform_MachineTime (); // Except this shouldn't be instanced.  -- Dude's its the app controller.  It ain't instanced!
	session_frame (mainus.time_slice); // Note that this time slice will be 0
#endif
}

static float mStatusBarHeight()
{
    CGSize statusBarSize = [[UIApplication sharedApplication] statusBarFrame].size;
	CGRect statusBarFrame = [[UIApplication sharedApplication] statusBarFrame];
	
    return MIN(statusBarSize.width, statusBarSize.height); // This works if status bar isn't hidden.
}

//
//static const char *iphone_device_description (device_type_e device_type)
//{
//	if (device_type == device_type_ipad)		return "iPad";
//	if (device_type == device_type_ipodtouch)	return "iPod Touch";
//	if (device_type == device_type_iphone)		return "iphone";
//	return "Unknown device type";
//}

int main(int argc, char * argv[]); // Our exclusive caller.
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	vid.is_mobile				= true;		// Probably prevent from vid_restart?
///	vid.mobile_interface		= true;		// Use the mobile interface.
	vid.is_screen_portrait		= true;		// iPad is portrait
	vid.is_mobile_ios_keyboard	= true;
	
	sysplat.content_scale		= [[UIScreen mainScreen] scale]; // iPad2 is not retina
	sysplat.fps_desired			= 72;		// I guess
	
	// Determine device type
	c_strlcpy (sysplat.phone_device_description, TO_CSTRING([UIDevice currentDevice].model));
	sysplat.iphone_device_type	= UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? device_type_ipad : device_type_iphone;					// Let's face it.
	if (sysplat.iphone_device_type == device_type_iphone && strcasestr(sysplat.iphone_device_type, "pod"))
		sysplat.iphone_device_type = device_type_ipodtouch;
	
	if (sysplat.iphone_device_type == device_type_ipad)
		vid.is_mobile = 2;
	
	sysplat.statusbar_height			= mStatusBarHeight();

#ifdef _DEBUG
	NSLog (@"didFinishLaunching");
#endif
	
	signal (SIGFPE, SIG_IGN); // Baker: Ignore floating point exceptions
	
	// Hide the status bar.  We already took measurements.
	[UIApplication sharedApplication].statusBarHidden = YES;

	// Note: applicationWillBecomeActive hasn't happened yet.
    [NSTimer scheduledTimerWithTimeInterval: 0.001f
                                     target: self
                                   selector: @selector (mainCentralStart:)
                                   userInfo: nil
                                    repeats: NO];


	// If there is a URL in launchOptions and you return NO, 
	// then the operating system will know that you cannot open the 
	// provided URL for some reason.
    return YES;
}


@end


#endif // PLATFORM_GUI_IOS
#endif // ! CORE_SDL

#ifndef CORE_SDL
#include "environment.h"
#ifdef PLATFORM_GUI_IOS

/*
Copyright (C) 2013-2014 Baker

*/
// sys_ios.m -- system



#include "quakedef.h"
#include "iphonequake.h"
#include "sys.h"	// Courtesy

sysplat_t sysplat;



///////////////////////////////////////////////////////////////////////////////
//  FILE IO: Baker ... I'd love to kill these, but it can wait - 2016 Dec
///////////////////////////////////////////////////////////////////////////////


#define	MAX_HANDLES		100 //johnfitz -- was 10
FILE	*sys_handles[MAX_HANDLES];

int findhandle (void)
{
	int		i;

	for (i = 1 ; i < MAX_HANDLES ; i++)
		if (!sys_handles[i])
			return i;

	System_Error ("out of handles");
	return -1;
}


int System_FileOpenRead (const char *path_to_file, int *pHandle)
{
	FILE	*f;
	int		i, retval;

	i = findhandle ();

	f = FS_fopen_read(path_to_file, "rb");

	if (!f)
	{
		*pHandle = -1;
		retval = -1;
	}
	else
	{
		sys_handles[i] = f;
		*pHandle = i;
		//retval = (int) FileHandle_GetLength (f);
		retval = (int) File_Length (path_to_file);
	}

	return retval;
}



int System_FileOpenWrite (const char *path_to_file)
{
	FILE	*f;
	int		i;

	i = findhandle ();

	f = FS_fopen_write(path_to_file, "wb");
	if (!f)
		System_Error ("Error opening %s: %s", path_to_file, strerror(errno));
	sys_handles[i] = f;

	return i;
}

void System_FileClose (int handle)
{
	FS_fclose (sys_handles[handle]);
	sys_handles[handle] = NULL;
}


void System_FileSeek (int handle, int position)
{
	fseek (sys_handles[handle], position, SEEK_SET);
}


int System_FileRead (int handle, void *dest, int count)
{
	return fread (dest, 1, count, sys_handles[handle]);
}

int System_FileWrite (int handle, const void *pdata, int numbytes)
{
	return fwrite (pdata, 1, numbytes, sys_handles[handle]);
}

#include <sys/sysctl.h>
int ourclock (void)
{
  struct timeval boottime;
    int mib[2] = {CTL_KERN, KERN_BOOTTIME};
    size_t size = sizeof(boottime);
    time_t now;
    time_t uptime = -1;
    (void)time(&now);
    if (sysctl(mib, 2, &boottime, &size, NULL, 0) != -1 && boottime.tv_sec != 0)
    {
        uptime = now - (boottime.tv_sec);
    }

    return uptime;
}


///////////////////////////////////////////////////////////////////////////////
//  SYSTEM IO
///////////////////////////////////////////////////////////////////////////////


#if id386

/*
================
System_MakeCodeWriteable
================
*/
void System_MakeCodeWriteable (unsigned long startaddr, unsigned long len)
{
//	DWORD  flOldProtect;
//
//	if (!VirtualProtect((LPVOID)startaddr, len, PAGE_READWRITE, &flOldProtect))
//   		System_Error("Protection change failed");
}
#endif // id386




///////////////////////////////////////////////////////////////////////////////
//  SYSTEM ERROR: Baker
///////////////////////////////////////////////////////////////////////////////


int System_Error (const char *fmt, ...)
{
//	static int	in_sys_error0 = 0;
//	static int	in_sys_error1 = 0;
//	static int	in_sys_error2 = 0;
//	static int	in_sys_error3 = 0;
//
//	VA_EXPAND (text, SYSTEM_STRING_SIZE_1024, fmt);
//
//	if (!in_sys_error3) in_sys_error3 = 1;
//
//	switch (isDedicated)
//	{
//	case true:
//		ConProc_Error (text);
//		break;
//
//	case false:
//
//#ifdef DIRECT3DX_WRAPPER // dx8 + dx9 - Temp maybe for Direct3D 9.  Possibly.  Relates to TOPMOST.  Must determine if we can detect scenario
//		// Baker: Direct3D hides popups, although this assumes window is setup which might be bad assumption
//		if (vid.initialized && vid.screen.type == MODESTATE_FULLSCREEN)
//		{
//			VID_Shutdown ();
//			Input_Shutdown ();
//			System_Process_Messages_Sleep_100 ();
//		}
//#endif // DIRECT3DX_WRAPPER ... TopMost interferes with msgbox
//
//		switch (in_sys_error0)
//		{
//		case true: // Recursive error, like occurred during shutdown
//			MessageBox(NULL, text, "Double Quake Error", MB_OK | MB_SETFOREGROUND | MB_ICONSTOP);
//			break;
//
//		case false:
//			in_sys_error0 = 1;
//			Input_Shutdown ();
//			MessageBox(NULL, text, "Quake Error", MB_OK | MB_SETFOREGROUND | MB_ICONSTOP);
//			break;
//		}
//		break;
//	} // End of dedicated vs. non-dedicated switch statement
//
//	if (!in_sys_error1)
//	{
//		in_sys_error1 = 1;
//		Host_Shutdown ();
//	}
//
//// shut down QHOST hooks if necessary
//	if (!in_sys_error2)
//	{
//		in_sys_error2 = 1;
//		DeinitConProc ();
//	}

	System_Exit (1);
#ifndef __GNUC__ // Return silence
	return 1; // No return as an attribute isn't universally available.
#endif // __GNUC__	// Make GCC not complain about return
}



///////////////////////////////////////////////////////////////////////////////
//  SYSTEM EVENTS
///////////////////////////////////////////////////////////////////////////////

// Baker: Used for Windows video mode switch
void System_Process_Messages_Sleep_100 (void)
{
//	MSG				msg;
//	while (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
//	{
//	    TranslateMessage (&msg);
//	    DispatchMessage (&msg);
//	}
//
//	Sleep (100);
}

//
//
//
//
//
//
// Called by Modal Message, Download, Install, NotifyBox
void System_SendKeyEvents (void)
{
	Input_Local_SendKeyEvents ();
}


///////////////////////////////////////////////////////////////////////////////
//  SYSTEM QUIT, INIT
///////////////////////////////////////////////////////////////////////////////


void System_Quit (void)
{
	Host_Shutdown();

#ifdef SUPPORTS_NEHAHRA
	Nehahra_Shutdown ();
#endif // SUPPORTS_NEHAHRA

	System_Exit (0);
}



// Main_Central calls us
void System_Init (void)
{
	// Nothing?
}


//
//
//
//
//
//
//
//

//
//
//
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////////
//  SYSTEM MAIN LOOP
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//  PLATFORM: MAIN
///////////////////////////////////////////////////////////////////////////////


#import "UIAppDelegate.h"

// Baker: Say what happens and where.
// 1) didFinishLaunchingWithOptions -> mainCentralStart
// 2) mainCentralFrame --> session frame


int main(int argc, char * argv[])
{
// Baker: Too early for main central.  ios isn't ready yet.
// We will do that in mainCentralStart
	

	@autoreleasepool {
	    return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
	}
}

///////////////////////////////////////////////////////////////////////////////
//  SYSTEM DISPATCH
///////////////////////////////////////////////////////////////////////////////
//
//
//
//

//
//
//
//

int dispatch (int msg)
{


}


// Main_Central calls us.  But SDL does not currently use that
void System_SleepUntilInput (int time)
{
}

//
//
//
//
//
//
//
//
//
//
//

//
//
//
//
//
//
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////////
//  CONSOLE:
///////////////////////////////////////////////////////////////////////////////

const char *Dedicated_ConsoleInput (void)
{
    char *pText = NULL;

    return pText;
}

#endif // PLATFORM_GUI_IOS
#endif // ! CORE_SDL

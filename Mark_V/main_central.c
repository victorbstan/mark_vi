/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2012 John Fitzgibbons and others
Copyright (C) 2009-2014 Baker and others

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// sys.c -- system

#include "quakedef.h"



int Main_Central_Loop ()
{
#ifndef CORE_SDL
	double oldtime = System_DoubleTime ();

    /* main window message loop */
	while (1)
	{
		double time, newtime;

		switch (isDedicated)
		{
		case true: // Dedicated server
			newtime = System_DoubleTime ();
			time = newtime - oldtime;

			while (time < sys_ticrate.value )
			{
				System_Sleep_Milliseconds (QUAKE_DEDICATED_SLEEP_TIME_MILLISECONDS_1 /* 1 */);
				newtime = System_DoubleTime ();
				time = newtime - oldtime;
			}
			break;

		case false:

			// yield the CPU for a little while when paused, minimized, or not the focus
#define PAUSE_SLEEP		50 // sleep time on pause or minimization
#define NOT_FOCUS_SLEEP	20 // sleep time when not focus
			if ((cl.paused && !vid.ActiveApp ) || vid.Minimized)
			{
				System_SleepUntilInput (PAUSE_SLEEP);
				scr_skipupdate = 1;		// no point in bothering to draw
			}
			else if (!vid.ActiveApp )
			{
				System_SleepUntilInput (NOT_FOCUS_SLEEP);
			}

			newtime = System_DoubleTime ();
			time = newtime - oldtime;
			break;
		}

		Host_Frame (time);
		oldtime = newtime;
	}
#endif // !CORE_SDL

    /* return success of application */
    return 0; // Baker: unreachable
}

void Main_Central_LT (char *cmdline, sys_handle_t *pmain_window_holder_addr, cbool do_loop)
{
	Core_Init (ENGINE_FAMILY_NAME, &qfunction_set, pmain_window_holder_addr);
}

int Main_Central (char *cmdline, sys_handle_t *pmain_window_holder_addr, cbool do_loop)
{
	const char	*executable_directory = Folder_Binary_Folder_URL ();
	char		*argv[MAX_NUM_Q_ARGVS_50 + 30] = { "" }; // Set first one to empty string
	char		cmdline_fake[MAX_CMD_256]; // for quake_command_line.txt if we use

	Core_Init (ENGINE_FAMILY_NAME, &qfunction_set, pmain_window_holder_addr);


	//
	// Determine basedir
	//
	// Windows/Mac/Linux - If pak0.pak isn't in current dir, 
	//                     check the executable dir and if found use instead.
	// iPhone/iPad (IOS) - Documents directory for our application.
	// Android           - "/sdcard/Quake"

#if defined(PLATFORM_IOS)
	{
		const char *File_iPhone_Documents_URL (void);
		c_strlcpy (host_parms._basedir, File_iPhone_Documents_URL() ); // Uses current working directory.
	}
#elif defined(PLATFORM_ANDROID)
	// March 21 2018 - Temporary cheat

//#define SDCARD_DATA "/Download/Quake"
	//c_strlcpy (host_parms._basedir, "/sdcard/data" ); // Uses current working directory.
	c_strlcpy (host_parms._basedir, ANDRROID_SDCARD_DATA); // Uses current working directory.

#else // End Android ... all other platforms except Android/IOS ...
	c_strlcpy (host_parms._basedir, File_Getcwd()); // Uses current working directory.

	// Baker: On Windows if a user makes a shortcut and doesn't set the "Start In" directory, it won't find the pak files
	// we will help by silently checking for situation and correcting the directory
	//	alert ("Current basedir is " QUOTED_S ".", host_parms._basedir);

	if (!File_Exists (va ("%s/id1/pak0.pak", host_parms._basedir)) && File_Exists (va ("%s/id1/pak0.pak", executable_directory)) ) {
		// Copy exe_dir to cwd}
		c_strlcpy (host_parms._basedir, executable_directory); 
	}  // Right?
#endif // End not IOS and not Android
	
//	alert ("Current basedir is " QUOTED_S ".", host_parms._basedir);

	host_parms.basedir = host_parms._basedir;
	host_parms.argc = 1;
	host_parms.argv = argv; // Null out of the executable name
	host_parms.first_time_init = !File_Exists (va("%s/%s", host_parms.basedir, HISTORY_FILE_NAME));

	//
	// Download shareware on Android at least, for now.
	//

#ifdef PLATFORM_ANDROID // March 25 2018
	{
		char pakfp[MAX_OSPATH];
		
		const char *pak0_download_url = "http://quakeone.com/quakedroid/shareware/pak0.pak";
		int pak0_download_size_18276119 = 18276119;
		int mem_size_int, exit_code_int = 0; 

		c_snprintf1 (pakfp, "%s/id1/pak0.pak", host_parms._basedir);

		if (!File_Exists (pakfp)) {
			// Fresh install.  Doesn't even have shareware.
			void Downloader_Start (int total_bytes);
			void Downloader_Finish (void);
			cbool Downloader_Progress (void *id, int old_total, int new_total);
			msgbox (NULL, "No Quake data files found.  Click ok to download Quake shareware pak0.pak." 
				NEWLINE NEWLINE "(If you own registered Quake, you will need to manually copy pak1.pak, see http://quakeone.com/markv for more help)" NEWLINE NEWLINE "Click OK to download Quake shareware.");

			

			Downloader_Start (pak0_download_size_18276119); // I guess.

			// Create Window
			{
				void *mem = Download_To_Memory_Alloc ("User agent", pak0_download_url, Downloader_Progress, NULL, &mem_size_int, &exit_code_int);
				cbool ok2 = (mem_size_int == pak0_download_size_18276119);

				Downloader_Finish ();

				if (!ok2) {
					msgbox (NULL, "Couldn't download pak0.pak.  You will need to install it manually or try again.");
					free (mem);
					System_Exit (0);
				}
				else {
					// Downloaded ok
					cbool wrote_ok = File_Memory_To_File (pakfp, mem, mem_size_int);
					if (!wrote_ok) {
						msgbox (NULL, "Downloaded shareware pak but couldn't write to file.");
						free (mem);
						System_Exit (0);
					}
				}
			 
				//alert ("Downloaded Quake shareware pak successfully!  Restart Quake!"); // Guess not!
		
				free (mem);
			}
		} // End if no
	}
#endif // PLATFORM_ANDROID

	//
	// Reconstruct the argc/argv from the cmdline
	// quake_command_line.txt may override an empty cmdline
	//

	if (cmdline[0] == 0) {
		if (File_Exists (va("%s/quake_command_line.txt", host_parms.basedir))) {
			size_t s_temp_size = 0; const char *s_temp = File_To_String_Alloc (va("%s/quake_command_line.txt", host_parms.basedir), &s_temp_size);
			if (s_temp)
				c_strlcpy (cmdline_fake, s_temp);
			else cmdline_fake[0] = 0; // Enough?
			String_Edit_Whitespace_To_Space (cmdline_fake); // Make tabs, newlines into spaces.
			cmdline = cmdline_fake;
			freenull (s_temp);
		}
	}

	String_Command_String_To_Argv (cmdline, &host_parms.argc, argv, MAX_NUM_Q_ARGVS_50);

	COM_InitArgv (host_parms.argc, host_parms.argv);

	//	alert ("Current final is " QUOTED_S ".", host_parms.basedir);


	Memory_Init ();

	//
	// Parameters and memory initialized
	//

	System_Init (); // Initializes time, sets some things for WinQuake asm id386 and checks Windows version.  Floating point exceptions, ..

// because sound is off until we become active
	S_BlockSound ();

	Host_Init ();

	if (!do_loop)
		return 0; // Mac uses a frame timer

	return Main_Central_Loop();
}

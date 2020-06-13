/*
Copyright (C) 2013-2014 Baker

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// utilities.c -- Baker: utilities with a layer of separation


#include "utilities.h" // This will include quakedef.h for us if applicable

#ifdef QUAKE_GAME
#include "quakedef.h"
#include "utilities.h"

#endif // QUAKE_GAME

void Utilities_Init (void)
{
//	Pak_Init (qfunction_set);

#if 0
	Wad2_Init (qfunction_set);
	Downloads_Init (qfunction_set);
	Zip_Init (qfunction_set);
#endif

	Cmd_AddCommands (Utilities_Init);
}

const char *cmds[] = {"add", "compress", "extract", "help", "list", "rem", "replace", "unzip", "zip", NULL};
enum arg_e {arg_add, arg_compress, arg_extract, arg_help, arg_list, arg_rem, arg_replace, arg_unzip, arg_zip, max_args};

const char *arg_msg[] =
{
	"<file.pak> <file in pak> <source url>", // add
	"<file.pak>", // compress
	"<file.pak> <file in pak> <extract destination>", // extract
	"", // help
	"<file.pak>", // list
	"<file.pak> <file in pak>", // remove
	"<file.pak> <file in pak> <replace with file path>", // replace
	"<file.pak> <destination dir>", //unzip
	"<file.pak to create> <folder to pak>", // zip
	NULL,
};


int args_required[] =
{
	3, // add pakurl pak_file_name filenameurl
	1, // compress pakurl
	3, // extract pakurl filename
	0, // help
	1, // list pakurl
	2, // remove pakurl filename
	3, // replace pakurl filename filenameurl
	2, // unzip pakurl workingdir
	2, // zip pakurl compressdir
	0,
};


// compress, extract, rem, replace will remain unlisted as to not confuse users.
void Pak_Command_f (lparse_t *line)
{
	// replace file, delete file, compress


	if (line->count >= 2)
	{
		const char *parm2 = line->args[1];
		const char *_pakurl = line->count >= 3 ? line->args[2] : NULL;
		const char *arg1 = line->count >= 4 ? line->args[3] : NULL;
		const char *arg2 = line->count >=5 ? line->args[4] : NULL;

		char pakurl[MAX_OSPATH];
		char fileurl[MAX_OSPATH];
		char destfolder[MAX_OSPATH];
		char srcfolder[MAX_OSPATH];
		char inside_pak_filename[MAX_OSPATH];

		int cmd_num = String_To_Array_Index (parm2, cmds);
		int numargs = line->count - 2;
		int i;
		cbool validnum = cmd_num < max_args;
		cbool meets_args = validnum ? args_required[cmd_num] == numargs : false;

		if (!validnum)
		{
			Con_PrintLinef ("Invalid pak command %s", parm2);
			return;
		}

		if (!meets_args)
		{
			Con_PrintLinef ("Wrong number of arguments for " QUOTED_S ": needs %d, have %d", parm2, args_required[cmd_num], numargs);
			Con_PrintLinef ("Usage: %s %s %s", line->args[0], parm2, arg_msg[cmd_num]);
			return;
		}

		c_strlcpy (pakurl, _pakurl);
		File_URL_Edit_SlashesForward_Like_Unix (pakurl);

		if (cmd_num != arg_zip && !File_Exists (pakurl))
		{
			Con_PrintLinef ("File " QUOTED_S " doesn't exist", _pakurl); // Print the unmodified version
			return;
		}

		switch (cmd_num)
		{
		case arg_add:  // sorts and rewrites (where nulled entry space is recovered) (can just sort the header)
	
			c_strlcpy (inside_pak_filename, arg1); File_URL_Edit_SlashesForward_Like_Unix (inside_pak_filename);
			c_strlcpy (fileurl, arg2); File_URL_Edit_SlashesForward_Like_Unix (fileurl);

			// Make sure the new file exists
			if (!File_Exists (fileurl))
			{
				Con_PrintLinef ("New file " QUOTED_S " does not exist", arg2);
				return;
			}

			if (Pak_Has_File(pakurl, inside_pak_filename))
			{
				Con_PrintLinef ("A file named " QUOTED_S " is already in the pak", inside_pak_filename);
				return;
			}

			// We already know the pak exists so lets get to it
			Pak_Add_File (pakurl, inside_pak_filename, fileurl /*source*/);

			Recent_File_Set_FullPath (pakurl);
			return;


		case arg_compress:  // sorts and rewrites (where nulled entry space is recovered) (can just sort the header)

			if (Pak_Compress (pakurl))
				Con_PrintLinef ("Pak compress successful");

			else Con_PrintLinef ("Pak compress failed");

			Recent_File_Set_FullPath (pakurl);
			return;

		case arg_extract:	// extract file to <dest> (final arg is not folder but entire path of file to write!!!!)

			//File_URL_Edit_SlashesForward_Like_Unix (pakurl);
			c_strlcpy (inside_pak_filename, arg1); File_URL_Edit_SlashesForward_Like_Unix (inside_pak_filename);
			c_strlcpy (fileurl, arg2); File_URL_Edit_SlashesForward_Like_Unix (fileurl);

			// Validate destination folder, making sure it exists
			c_strlcpy (destfolder, fileurl);
			File_URL_Edit_Reduce_To_Parent_Path (destfolder);

			if (!File_Exists (destfolder))
			{
				Con_PrintLinef ("Destination folder for " QUOTED_S " does not exist", arg2);
				return;
			}

			if (!File_Is_Folder(destfolder))
			{
				Con_PrintLinef ("Destination folder for " QUOTED_S " is a file", arg2);
				return;
			}

			// Validate file to extract exists
			c_strlcpy (inside_pak_filename, arg1);
			File_URL_Edit_SlashesForward_Like_Unix (inside_pak_filename);

			if (!Pak_Has_File (pakurl, inside_pak_filename))
			{
				Con_PrintLinef ("File " QUOTED_S " does not exist in pak " QUOTED_S, inside_pak_filename, pakurl);
				return;
			}

			Pak_Extract_File (pakurl, inside_pak_filename, fileurl /*dest*/);

			Recent_File_Set_FullPath (fileurl);
			return;

		case arg_rem:		// remove a file <delete a file inside pak by zeroing out entry>

			// Validate file to zero out inside pak exists
			c_strlcpy (inside_pak_filename, arg1);
			File_URL_Edit_SlashesForward_Like_Unix (inside_pak_filename);

			if (!Pak_Has_File (pakurl, inside_pak_filename))
			{
				Con_PrintLinef ("File " QUOTED_S " does not exist in pak " QUOTED_S, inside_pak_filename, pakurl);
				return;
			}

			Pak_Remove_File (pakurl, inside_pak_filename);
			return;

		case arg_replace:	return; // replace a file in pak <if larger, null out entry and write another one>

			// Validate source
			c_strlcpy (fileurl, arg2);
			File_URL_Edit_SlashesForward_Like_Unix (fileurl);

			if (!File_Exists (fileurl))
			{
				Con_PrintLinef ("File source " QUOTED_S " does not exist", fileurl);
				return;
			}

			// Validate requested file to replace exists
			c_strlcpy (inside_pak_filename, arg1);
			File_URL_Edit_SlashesForward_Like_Unix (inside_pak_filename);

			if (!Pak_Has_File (pakurl, inside_pak_filename))
			{
				Con_PrintLinef ("File " QUOTED_S " does not exist in pak " QUOTED_S, inside_pak_filename, pakurl);
				return;
			}
#pragma message ("Baker: Has pak replace ever been tested?")
			Pak_Replace_File (pakurl, inside_pak_filename /*replaced*/ , fileurl);
			return;

		case arg_unzip: // arg1 is destination

			// Validate destination
			c_strlcpy (destfolder, arg1);
			File_URL_Edit_SlashesForward_Like_Unix (destfolder);

			if (!File_Exists (destfolder))
			{
				char parentpath[MAX_OSPATH];
				c_strlcpy (parentpath, destfolder);
				File_URL_Edit_Reduce_To_Parent_Path (parentpath);

				if (!File_Exists(parentpath))
				{
					Con_PrintLinef ("Destination " QUOTED_S " does not exist", destfolder);
					return;
				}
				// Ok to make just a subfolder
			}

			i = Pak_Unzip (pakurl, destfolder);

			Con_PrintLinef ("%d files extracted", i);
			Recent_File_Set_FullPath (destfolder);
			return;

		case arg_zip: // arg1 is source

			// Validate source
			c_strlcpy (srcfolder, arg1);
			File_URL_Edit_SlashesForward_Like_Unix (srcfolder);

			if (!File_Exists (srcfolder))
			{
				Con_PrintLinef ("Source folder " QUOTED_S " does not exist", srcfolder);
				return;
			}

			Pak_Zip_Folder (pakurl, srcfolder);

			Recent_File_Set_FullPath (pakurl);
			return;

		case arg_help: // list current mappings

			Con_PrintLine ();
			Con_PrintLinef ("Usage: %s <0-99> <yourfile.mp3>", line->args[0]);
			Con_PrintLinef ("where yourfile.mp3 is in [gamedir]/music folder");
			Con_PrintLine ();
			Con_PrintLinef ("Usage: %s list  - print list of contents", line->args[0]);
			Con_PrintLinef ("Usage: %s zip <folder_to_zip> <output.pak> - resets everything", line->args[0]);
			Con_PrintLinef ("Usage: %s unzip <input.pak> <folder_unzip_location>", line->args[0]);
			Con_PrintLine ();
			Con_PrintLinef ("Examples:");
			Con_PrintLine ();
			Con_PrintLinef ("%s unzip c:\\quake\\id1\\pak.0.pak c:\\quake2", line->args[0]);
			Con_PrintLinef ("%s zip c:\\quake\\id1\\maps c:\\quake\\id1\\mymaps.pak", line->args[0]);
			Con_PrintLine ();
			Con_PrintLinef ("Filenames should avoid spaces and only use alphanumeric");
			Con_PrintLinef ("characters and the underscore '_'.");
			Con_PrintLine ();
			Con_PrintLinef ("Type 'folder' to access current gamedir.");
			Con_PrintLine ();

			return;

		case arg_list: // list current mappings

			Con_PrintLine ();
			Con_PrintLinef ("Files in pak " QUOTED_S ":", _pakurl);
			Con_PrintLine ();

			Pak_List_Print (pakurl);

			Con_PrintLine ();

			Con_PrintLinef ("Savings by compressing pak is %u", (unsigned int) Pak_Is_Compressable(pakurl));

			Recent_File_Set_FullPath (pakurl);
			return;

		} // End of switch

	} // End of args >=2

	// with no parameters or invalid parameters ends up displaying help
	Con_PrintLine ();
	Con_PrintLinef ("Usage: %s {help|list|unzip|zip}", line->args[0]);
	Con_PrintLinef ("Type '%s help' for examples and detail", line->args[0]);
	Con_PrintLine ();

}

// compress, extract, rem, replace will remain unlisted as to not confuse users.
// compress, extract, rem, replace will remain unlisted as to not confuse users.
void Zip_Command_f (lparse_t *line)
{
	// replace file, delete file, compress


	if (line->count >= 2)
	{
		const char *parm2 = line->args[1];
		const char *_zipurl = line->count >= 3 ? line->args[2] : NULL;
		const char *arg1 = line->count >= 4 ? line->args[3] : NULL;
		const char *arg2 = line->count >=5 ? line->args[4] : NULL;

		char zipurl[MAX_OSPATH];
		char fileurl[MAX_OSPATH];
		char destfolder[MAX_OSPATH];
		char srcfolder[MAX_OSPATH];
		char inside_zip_filename[MAX_OSPATH];

		int cmd_num = String_To_Array_Index (parm2, cmds);
		int numargs = line->count - 2;
		int i;
		cbool validnum = cmd_num < max_args;
		cbool meets_args = validnum ? args_required[cmd_num] == numargs : false;

		if (!validnum)
		{
			Con_PrintLinef ("Invalid zip command %s", parm2);
			return;
		}

		if (!meets_args)
		{
			Con_PrintLinef ("Wrong number of arguments for " QUOTED_S ": needs %d, have %d", parm2, args_required[cmd_num], numargs);
			Con_PrintLinef ("Usage: %s %s %s", line->args[0], parm2, arg_msg[cmd_num]);
			return;
		}

		c_strlcpy (zipurl, _zipurl);
		File_URL_Edit_SlashesForward_Like_Unix (zipurl);

		if (cmd_num != arg_zip && !File_Exists (zipurl))
		{
			Con_PrintLinef ("File " QUOTED_S " doesn't exist", _zipurl); // Print the unmodified version
			return;
		}

		switch (cmd_num)
		{
		case arg_add:  // sorts and rewrites (where nulled entry space is recovered) (can just sort the header)
			Con_PrintLinef ("Unsupported at this time");
			return;


		case arg_compress:  // sorts and rewrites (where nulled entry space is recovered) (can just sort the header)

			Con_PrintLinef ("Unsupported at this time");
			return;

		case arg_extract:	// extract file to <dest> (final arg is not folder but entire path of file to write!!!!)

			//File_URL_Edit_SlashesForward_Like_Unix (zipurl);
			c_strlcpy (inside_zip_filename, arg1); File_URL_Edit_SlashesForward_Like_Unix (inside_zip_filename);
			c_strlcpy (fileurl, arg2); File_URL_Edit_SlashesForward_Like_Unix (fileurl);

			// Validate destination folder, making sure it exists
			c_strlcpy (destfolder, fileurl);
			File_URL_Edit_Reduce_To_Parent_Path (destfolder);

			if (!File_Exists (destfolder))
			{
				Con_PrintLinef ("Destination folder for " QUOTED_S " does not exist", arg2);
				return;
			}

			if (!File_Is_Folder(destfolder))
			{
				Con_PrintLinef ("Destination folder for " QUOTED_S " is a file", arg2);
				return;
			}

			// Validate file to extract exists
			c_strlcpy (inside_zip_filename, arg1);
			File_URL_Edit_SlashesForward_Like_Unix (inside_zip_filename);

			if (!Zip_Has_File (zipurl, inside_zip_filename))
			{
				Con_PrintLinef ("File " QUOTED_S " does not exist in zip " QUOTED_S, inside_zip_filename, zipurl);
				return;
			}

			Zip_Extract_File (zipurl, inside_zip_filename, fileurl /*dest*/);

			Recent_File_Set_FullPath (fileurl);
			return;

		case arg_rem:		// remove a file <delete a file inside zip by zeroing out entry>

			Con_PrintLinef ("Unsupported at this time");
			return;

		case arg_replace:	return; // replace a file in zip <if larger, null out entry and write another one>

			Con_PrintLinef ("Unsupported at this time");
			return;

		case arg_unzip: // arg1 is destination

			// Validate destination
			c_strlcpy (destfolder, arg1);
			File_URL_Edit_SlashesForward_Like_Unix (destfolder);

			if (!File_Exists (destfolder))
			{
				char parentpath[MAX_OSPATH];
				c_strlcpy (parentpath, destfolder);
				File_URL_Edit_Reduce_To_Parent_Path (parentpath);

				if (!File_Exists(parentpath))
				{
					Con_PrintLinef ("Destination " QUOTED_S " does not exist", destfolder);
					return;
				}
				// Ok to make just a subfolder
			}

			i = Zip_Unzip (zipurl, destfolder);

			Con_PrintLinef ("%d files extracted", i);
			Recent_File_Set_FullPath (destfolder);
			return;

		case arg_zip: // arg1 is source

			// Validate source
			c_strlcpy (srcfolder, arg1);
			File_URL_Edit_SlashesForward_Like_Unix (srcfolder);

			if (!File_Exists (srcfolder))
			{
				Con_PrintLinef ("Source folder " QUOTED_S " does not exist", srcfolder);
				return;
			}

			Zip_Zip_Folder (zipurl, srcfolder);

			Recent_File_Set_FullPath (zipurl);
			return;

		case arg_help: // list current mappings

			Con_PrintLine ();
			Con_PrintLinef ("Usage: %s <0-99> <yourfile.mp3>", line->args[0]);
			Con_PrintLinef ("where yourfile.mp3 is in [gamedir]/music folder");
			Con_PrintLine ();
			Con_PrintLinef ("Usage: %s list  - print list of contents", line->args[0]);
			Con_PrintLinef ("Usage: %s zip <folder_to_zip> <output.zip> - resets everything", line->args[0]);
			Con_PrintLinef ("Usage: %s unzip <input.zip> <folder_unzip_location>", line->args[0]);
			Con_PrintLine ();
			Con_PrintLinef ("Examples:");
			Con_PrintLine ();
			Con_PrintLinef ("%s unzip c:\\quake\\id1\\zip.0.zip c:\\quake2", line->args[0]);
			Con_PrintLinef ("%s zip c:\\quake\\id1\\maps c:\\quake\\id1\\mymaps.zip", line->args[0]);
			Con_PrintLine ();
			Con_PrintLinef ("Filenames should avoid spaces and only use alphanumeric");
			Con_PrintLinef ("characters and the underscore '_'.");
			Con_PrintLine ();
			Con_PrintLinef ("Type 'folder' to access current gamedir.");
			Con_PrintLine ();

			return;

		case arg_list: // list current mappings

			Con_PrintLine ();
			Con_PrintLinef ("Files in zip " QUOTED_S ":", _zipurl);
			Con_PrintLine ();

			Zip_List_Print (zipurl);

			Con_PrintLine ();

			Recent_File_Set_FullPath (zipurl);
			return;

		} // End of switch

	} // End of args >=2

	// with no parameters or invalid parameters ends up displaying help
	Con_PrintLine ();
	Con_PrintLinef ("Usage: %s {help|list|unzip|zip}", line->args [0]);
	Con_PrintLinef ("Type '%s help' for examples and detail", line->args [0]);
	Con_PrintLine ();

}



void Dir_Command_f (lparse_t *line)
{
	const char *comparestring = (line->count == 2) ? line->args[1] : NULL;
	clist_t *dir_contents = File_List_Recursive_Alloc (com_gamedir, comparestring);
	clist_t *cursor;
	double filesizes;
	size_t cursize;
	

#if 0 // OLD - Dec 29 2016
	if (line->count == 2)
		comparestring = line->args[1];
#endif

	for (cursor = dir_contents, filesizes = 0; cursor; cursor = cursor->next)
	{
#if 0 // OLD - Dec 29 2016
		// Skip disqualified matches
		if (comparestring && !wildmultcmp (comparestring, File_URL_SkipPath (cursor->name)) )
			continue;
#endif
		cursize = File_Length(cursor->name),
		Con_PrintLinef ("%s %u", cursor->name, (unsigned int) cursize); // C89
		filesizes += cursize;
	}

	Con_PrintLinef ("Sizes combined %f", filesizes);
	List_Free (&dir_contents);
}




void Install_Download_Before (const char *filename_any)
{
	memset (&cls.download, 0, sizeof(cls.download));

	c_strlcpy (cls.download.name, File_URL_SkipPath(filename_any));
	cls.download.percent = 0;
	cls.download.total_bytes = -1;
	cls.download.is_blocking = true;

	S_ClearBuffer ();		// so dma doesn't loop current sound

	key_count = -1;		// wait for a key down and up

	SCR_UpdateScreen ();
}

static cbool Install_Command_Progress (void *id, int old_total, int new_total)
{
	float percent = cls.download.total_bytes > 0 ?  (float)new_total / cls.download.total_bytes : 0;
	float newpct = CLAMP(0, percent, 1);
	
	if (newpct - cls.download.percent > 0.005)
	{
		// Every once in a while update the screen
		cls.download.percent = newpct;

		System_SendKeyEvents ();	// Check key events
		SCR_UpdateScreen ();		// Hmmm.
		key_count = -1;				// Necessary?

		if (key_lastpress == K_ESCAPE)
			return (cls.download.user_cancelled = true); // Bail!
	}

	return false; // Returns true to cancel, right?
}

void Install_Download_After (void)
{
//	int sizer = sizeof(cls.download);
	
	memset (&cls.download, 0, sizeof(cls.download));

	// make sure we don't ignore the next keypress
	if (key_count < 0)
		key_count = 0;
}

void UnInstall_Command_f (lparse_t *line)
{
	const char *subdir = NULL;
	int fail_reason = 0;
	char game_folder_url[MAX_OSPATH];
	char library_url[MAX_OSPATH];
	cbool dofull = false;
	cbool ret;
	
// What if game is in use?
	if (cmd_from_server || cmd_source == src_client)
		return; // Not allowed (demoplayback?)

	if (cls.state == ca_connected || cls.demoplayback) // Demo playback + download screen updates causes R_RenderView to run out of stack space for WinQuake.
	{
#if 1
		//Kill the server
		CL_Disconnect ();
		Host_ShutdownServer (true);
#else
		Con_PrintLinef ("Disconnect first please");
#endif

	//	return;
	}

	if (strcasecmp(gamedir_shortname(), GAMENAME_ID1) || com_gametype != gametype_standard)
	{
		Con_PrintLinef ("Set " QUOTEDSTR("game id1") " first!  Currently in a gamedir");
		return;
	}


	dofull = (line->count == 3 && String_Does_Match_Caseless(line->args[2], "full"));

	if ( !(line->count == 2 || (line->count == 3 && dofull)))
	{
		Con_PrintLinef ("Need the game to install or the entire URL with the http:// in it");
		Con_PrintLinef ("Example: uninstall <modname> [full]");
		Con_PrintLinef ("Note: Using 'full' will also remove zip from library");
		return;
	}

	subdir = line->args[1];
	if (!fail_reason && cmd_source == src_client) fail_reason = 1;
	if (!fail_reason && strstr(subdir, "..")) fail_reason = 2;
	if (!fail_reason && strstr(subdir, "/")) fail_reason = 3;
	if (!fail_reason && strstr(subdir, "\\")) fail_reason = 3;
	if (!fail_reason && String_Does_Match_Caseless(subdir, "id1")) fail_reason = 4;

	
	if (fail_reason)
	{
		const char *msgs[] = {"uninstall isn't permitted remotely", "relative path not allowed", "no path separators allowed", "can't remove id1"};
		const char *fail_msg = msgs[fail_reason - 1];
		Con_PrintLinef ("%s: %s", line->args[0], fail_msg);
		return;
	}

	c_strlcpy (game_folder_url, basedir_to_url(subdir));

	if (File_Exists(game_folder_url) && !File_Is_Folder(game_folder_url))
	{
		Recent_File_Set_FullPath (game_folder_url);
		Con_PrintLinef ("%s: " QUOTED_S " is a file, you might consider removing it.", line->args[0], game_folder_url);
		Con_PrintLinef ("Type 'showfile' to browse to that file.");  
		return;
	}

	if (!File_Exists(game_folder_url))
	{
		Con_PrintLinef ("%s: folder " QUOTED_S " does not exist", line->args[0], game_folder_url);
	}
	else
	{
		clist_t *file_list = NULL;
		
		Con_PrintLinef ("Removing files in %s", subdir);
		file_list = File_List_Recursive_Alloc(game_folder_url, NO_PATTERN_NULL);
		File_Delete_List (file_list);
		List_Free (&file_list);		

		Con_PrintLinef ("Removing directories in %s", subdir);
		file_list = File_List_Dirs_Recursive_Alloc(game_folder_url, NO_PATTERN_NULL);
		File_Rmdir_List (file_list);
		List_Free (&file_list);		

		Con_PrintLinef ("Removing base folder in %s", subdir);

		ret = File_Rmdir (game_folder_url); 	
		if (!ret)
		{
			Recent_File_Set_FullPath (game_folder_url);				
			Con_WarningLinef ("Couldn't remove base folder %s", subdir);
			Con_WarningLinef ("Does the folder have other contents?");
			Con_WarningLinef ("Is a file opened in another application?");
			Con_WarningLinef ("Is the folder open in another window?");
			Con_PrintLinef ("Type 'folder' to explore contents.");
			Con_PrintLinef ("Try again when this is resolved.");
			return;
		} else Con_PrintLinef ("Removed base folder in %s", subdir);

	}

	if (dofull)
	{
		c_strlcpy (library_url, downloads_folder_url(subdir));
		if (!String_Does_End_With_Caseless (library_url, ".zip"))
			c_strlcat (library_url, ".zip");
		
		if (!File_Exists(library_url))
			Con_PrintLinef ("%s archive not present", library_url);
		else if (File_Is_Folder(library_url))
			Con_PrintLinef ("%s is a folder!", library_url);
		else
		{
			Con_PrintLinef ("Deleting archive %s", library_url);
			File_Delete (library_url);
		}
	}


//	
//	File_Mkdir_Recursive
//	File_Rmdir
	/*
	
	c_strlcat (".zip");
	clist_t * blah
	File_Delete_List
	File_Delete*/
// 
	Lists_Update_ModList ();
}

#if 0
void Http_Command_f (lparse_t *line)
{
//	char safedir_dem_url[MAX_OSPATH];	// c:\Users\ ....\Roaming\...\mydem.dem
	struct arg_buckets_64_s argbuckets = {0};
// Set user-agent
// URL
// Realloc buffer
// Timeout
// Fail how?
// Pass an update fn
	//const char *user_agent, update_fn, buffer, destfile, url

	c_snprintf1 (argbuckets.cmdline, "anything /h:%s", "http://www.quake-1.com/quakec-gallery/gyro2_21a.zip");
	String_To_Arg_Buckets (&argbuckets, argbuckets.cmdline);
	http_runner (argbuckets.argcount, argbuckets.argvs);

}
#endif



#define QUAKE_INJECTOR_USER_AGENT "(^Quakeinjector|^Java|DokuWiki HTTP Client)"
void Install_Command_f (lparse_t *line)
{
	#define QUOTH_2_NAME_QUOTH2PT2FULL "quoth2pt2full"

	char game_url[SYSTEM_STRING_SIZE_1024]; // http://www.quaddicted.com/filebase/travail.zip or such

	char install_game_folder_url[MAX_OSPATH];	// c:/quake/warpspasm
	char library_folder_zip_url[MAX_OSPATH];	// c:\quake\id1\_library\warpspasm.zip
	char download_cache_url[MAX_OSPATH];		// c:/Users/Main/AppData/Roaming/Mark V/caches/__tempfiles/warpspasm.zip

	cbool did_download = false;			// Used to determine if we are reading from cache or library.
	cbool game_is_quoth = false;		// We'll set this to quoth if quoth2pt2full
	const char *arg1 = line->args[1];
	const char *arg2 = line->args[2];
	cbool is_self_forced = line->count == 3 && String_Does_Match_Caseless (arg2, "self");
	cbool is_specific_forced = line->count == 3 && !is_self_forced;

#if 0 // Proposed
	if (cmd_from_server || cmd_source == src_client) {
		Con_PrintLinef ("Illegal install request source");
		return; // Not allowed
	}
#endif

	// Formats accepted "install travail" or "install grendel travail" or "install grendel self"
	if (!isin2 (line->count, 2, 3)) {
		Con_PrintLinef ("Need the game to install or the entire URL with the http:// in it");
		Con_PrintLinef ("Example: install travail or install [http://URL]");
		Con_PrintLinef ("The version of libcurl used does not support https:// at this time");
		return;
	}

	// Process a URL like "install http://somewhere.com/mymod.zip"
	if (String_Does_Contain (arg1, "/")) {

		// Make sure the url ends with a zip
		if (!String_Does_End_With_Caseless (arg1, ".zip")) {
			Con_PrintLinef ("Only .zip files are supported");
			return; // REJECTED
		}
		
		// SUCCESS
		c_strlcpy (game_url, arg1);
		goto download_it;
	}

	// Process a mod name "install travail"
	// http://www.quaddicted.com/filebase/ + travail --> http://www.quaddicted.com/filebase/travail
	c_snprintf2 (game_url, "%s/%s", install_depot_source.string, arg1);			// a. Cat depot + name

	if (String_Does_End_With_Caseless (game_url, ".zip") == false) {			// b. Cat .zip if it doesn't have it.
		c_strlcat (game_url, ".zip");
	}

	String_Edit_To_Lower_Case (game_url);										// c. Convert to lower case.

	if (String_Does_Match_Caseless (arg1, QUOTH_2_NAME_QUOTH2PT2FULL)) {
		game_is_quoth = true;
	}

	// ACCEPTED

download_it:

	// Downloads folder is typically c:\quake\yourquake\id1\_library - make the dir to ensure it exists.
	File_Mkdir_Recursive (downloads_folder_url(""));

	Con_DPrintLinef ("Source: %s", game_url);

	{ 
		
		// Build the places
		c_strlcpy (library_folder_zip_url, downloads_folder_url(File_URL_SkipPath(game_url)) /**/); 
		c_snprintf2 (download_cache_url, "%s/%s", com_safedir, File_URL_SkipPath(game_url));

		if (is_specific_forced) {
			c_strlcpy (install_game_folder_url,basedir_to_url (String_Edit_To_Lower_Case(arg2)));
		}
		else {
			c_strlcpy (install_game_folder_url, basedir_to_url (File_URL_SkipPath(game_url)) /**/);  File_URL_Edit_Remove_Extension (install_game_folder_url); // If it is a game
		}
		

		// If folder exists like (c:/quake/warpspasm), warn and refuse.
		if (File_Exists(install_game_folder_url) && !is_specific_forced) {
			// REJECTION ... c:\quake\warpspasm already exists ...
			Recent_File_Set_FullPath (install_game_folder_url);

			switch (File_Is_Folder(install_game_folder_url)) {
			default:				// Let the user examine the folder.
									Con_PrintLinef ("It	looks like this may already be installed.");
									Con_PrintLinef ("Type 'folder' to explore contents.");

			case_break false:		// Address only remotely possible extra stupid situation.  Delete file instead?
									Con_PrintLinef ("Will not be able to make folder " QUOTED_S " because a file by that name exists", install_game_folder_url);
									Con_PrintLinef ("Type 'showfile' to browse to that file.");  
			}
			return; // REJECTED
		}

		// If the archive exists, make sure it is valid -- (must be able to open it, it must contain files)
		if (File_Exists(library_folder_zip_url)) {		
			clist_t *file_list_alloc = Zip_List_Alloc (library_folder_zip_url);
			
			if (file_list_alloc == NULL) {
				Recent_File_Set_FullPath (library_folder_zip_url);
				Con_PrintLinef ("There is an archive, but it contains no files or an invalid zip");
				Con_PrintLinef ("Type 'showfile' to browse to that file.");
				return; // REJECTED (we also do not have anything to free
			}
			List_Free (&file_list_alloc); // Cannot pass NULL to this to free.

			Con_PrintLinef ("This archive exists in the library, no download required");
		}

		// If running something, kill the server or demo or disconnect.  Then continue with the install ...
		if (cls.state == ca_connected || cls.demoplayback) { 
			CL_Disconnect ();
			Host_ShutdownServer(true); // Continue the install ...
		}

		// ONLY download the file if c:\quake\id1\_library\warpspasm.zip doesn't exist.
		if (File_Exists(library_folder_zip_url) == false) {
			cbool is_success;					// Used.
			cbool was_cancelled = false;		// Used.  To indicate fail because of CANCEL vs. fail because of ERROR.
			int error_code;
			Con_DPrintLinef ("Download: to %s", download_cache_url);
	
			
			// Set download params for http://www.quaddicted.com/filebase/travail.zip
			Install_Download_Before (game_url); // set the cls.download stuff, stops sound and updates screen.

			is_success = Download_To_File(QUAKE_INJECTOR_USER_AGENT, 
				game_url,					// http://www.quaddicted.com/filebase/travail.zip
				download_cache_url,			// c:/Users/Main/AppData/Roaming/Mark V/caches/__tempfiles/warpspasm.zip
				Install_Command_Progress,	// The bar draw
				NULL,						// Print function, we don't want one
				&cls.download.total_bytes, 
				&error_code
			);


//			{ const char *mem = Download_To_Memory_Alloc (QUAKE_INJECTOR_USER_AGENT, game_url, Install_Command_Progress, NULL, &cls.download.total_bytes, &errorcode);
//				if (mem) { File_Memory_To_File (download_cache_url, mem, cls.download.total_bytes); mem = core_free (mem); }
//			} // Memory way ^^ that we aren't using.

			// Install_Command_Progress sets cls.download.user_cancelled if ESC was pressed
			was_cancelled = cls.download.user_cancelled;

			Install_Download_After ();  // clears cls.download stuff
			
			// FAILURE - Remove the file and get out.  (Download to file will set is_success to false if the entire zip didn't download.)
			if (!is_success) {
				Con_PrintLinef ("download %s", was_cancelled ? "cancelled by user" : "failed or incomplete");
				File_Delete (download_cache_url); // If it is a partial, delete it.
				// FAILURE
				return; // Get out
			}
			
			Con_PrintLinef ("Download complete");
			did_download = true;
		}

		// If we are here, we have a zip to decompress
	}

	// EXAMINE THE ZIP
	{ 
		typedef enum {
			ENUM_FORCE_INT_GCC_ (found_)
			found_none_0 = 0, 
			found_map,
			found_progs,
			found_pak,
			found_other,
		} found_e;

		// The zip we are checking might be in /Users ... caches or quake\id1\_library
		const char		*zip_check_url			= did_download ? download_cache_url : library_folder_zip_url;

		clist_t			*file_list_alloc		= Zip_List_Alloc (zip_check_url);
		found_e			found_what				= found_none_0;
		const char 		*basis_fp				= NULL;
		const char		*basis_filename			= NULL;
		int				count; 
		int				skipcount				= -1;
		cbool			is_maps_only_extract	= false;

		if (!file_list_alloc) {
			// FAILURE
			Con_PrintLinef ("No files in the archive");
			return; // Get out.  file_list_alloc does not need freed because is null
		}

		{ clist_t *cur; for (cur = file_list_alloc, count = 0; cur; cur = cur->next, count ++) {
			const char *filename_only = File_URL_SkipPath(cur->name);

			// Ignore if no length.  Can this be a folder or not?
			if (filename_only[0] == 0) 
				continue; // No name.

			Con_DPrintLinef ("File %d: %s", count, cur->name);

			if (String_Does_Match_Caseless (filename_only, DEFAULT_PROGS_DAT_NAME)) { found_what = found_progs; basis_fp = cur->name; basis_filename = filename_only; break; }
			if (String_Does_End_With_Caseless (filename_only, ".pak"))				{ found_what = found_pak;   basis_fp = cur->name; basis_filename = filename_only; break; }
			if (String_Does_End_With_Caseless (filename_only, ".bsp")) /*continue*/ { found_what = found_map;   basis_fp = cur->name; basis_filename = filename_only; continue; }

			// Keep going if found something miscellaneous ...
			if (found_what == found_none_0)  found_what = found_other;
		}} // End for

		if (!basis_fp) {
			Con_PrintLinef ("This archive has no maps, no paks and no progs.dat so appears to have no playable content.");
			switch (did_download) {
			default /*true*/:	Con_PrintLinef ("As we just downloaded this archive, removing the file from the library to prevent any confusion in the future.");
								File_Delete (download_cache_url);

			case_break false:	Recent_File_Set_FullPath (library_folder_zip_url);
								Con_PrintLinef ("File " QUOTED_S " should not be in the library because of this and may cause interference.", File_URL_SkipPath(library_folder_zip_url));
								Con_PrintLinef ("Type 'showfile' to browse to that file.");  
			}

			goto cleanup;
		}

		skipcount = basis_filename - basis_fp;
		is_maps_only_extract = false;

		// See if it is a maps directory extract.
		if (found_what == found_map) {
			// something/maps/mybsp.bsp ---------------- normal
			// maps/mybsp.bsp -------------------------- normal
			// something/mymaps/mybsp.bsp <--- fail.  slashcount > 1 and preceded by something other than maps/
			// something/mybsp.bsp <-------------------- to maps/

			// ARE WE PREFIXED WITH (START OF STRING)maps/ or /maps/
			const char *maps_slash = "maps/";			int maps_slash_slen_5 = strlen(maps_slash);
			const char *maps_slash2 = "/maps/";			int maps_slash2_slen_6 = strlen(maps_slash2);
			
			if (skipcount == maps_slash_slen_5 && String_Does_Start_With_Caseless (basis_fp, maps_slash)) {
				// This is maps/mybsp.bsp so bare basepath and normal install.
				is_maps_only_extract = false;
				found_what = found_pak; // Lazy but whatever May 2 2018
				skipcount = skipcount - maps_slash_slen_5;
				goto maps_done; // Just use skipcount
			}

			if (skipcount > maps_slash_slen_5 && String_Does_Start_With_Caseless (&basis_filename[-maps_slash2_slen_6], maps_slash2)) {
				is_maps_only_extract = false;
				found_what = found_pak; // Lazy but whatever May 2 2018
				skipcount = skipcount - maps_slash_slen_5;
				goto maps_done; // Just use this new skip count
			}

			is_maps_only_extract = true; // Use current skipcount, but dest write is mygame/maps
			
		}
maps_done:

		// If downloaded move it to the libary now.
		if (did_download)
			File_Rename (download_cache_url, library_folder_zip_url);

		if (found_what > found_map && game_is_quoth) { 
			Con_WarningLinef ("Assuming Quoth installation.");
			String_Edit_Replace (install_game_folder_url, sizeof(install_game_folder_url), QUOTH_2_NAME_QUOTH2PT2FULL, "quoth");
		}

		DEBUG_ASSERT (skipcount >= 0);

		// UNPAK TIME
		{
			char dest_base[MAX_OSPATH];
			char strip_prefix[MAX_OSPATH] = {0};
			switch (found_what != found_map  || is_specific_forced || is_self_forced) {
			default /*true*/:		
									c_strlcpy   (dest_base, install_game_folder_url);		
									
			case_break false:		// Maps and not forced = quake/id1
									c_snprintf1 (dest_base, "%s/" GAMENAME_ID1, com_basedir);
			
									
			}

			if (skipcount > 0) {
				c_strlcpy (strip_prefix, basis_fp);
				strip_prefix [skipcount] = 0;
				skipcount = skipcount;
			}

			if (is_maps_only_extract) {
				c_strlcat (dest_base, "/maps");
			}
		
			Con_PrintLinef ("Destination: " QUOTED_S, dest_base);

			// Unpak time
			// 
			{ clist_t *cur; int count; for (cur = file_list_alloc, count = 0; cur; cur = cur->next, count ++) {
				int curname_slen = strlen(cur->name);
				const char *filename_only = File_URL_SkipPath(cur->name);

				#pragma message ("Does this happen YES OR NO?")
				if (String_Does_End_With (cur->name, "/") ) { 
					continue; }	//Happens like "nehahra/"

				if (cur->name[0] == 0) {
					continue; }

				if (filename_only[0] == 0) {
					continue; }

				// Skip section
				if (String_Does_End_With_Caseless(cur->name, ".exe"))		{ Con_PrintLinef ("Note: Skip " QUOTED_S " (executable)", filename_only);  continue;  /* No thanks */ }
				if (String_Does_Match_Caseless(cur->name, "config.cfg"))	{ Con_PrintLinef ("Note: Skip " QUOTED_S " (config.cfg, inappropriate)", filename_only); continue;  /* No thanks */ }
				if (String_Does_End_With_Caseless(cur->name, ".dll"))		{ Con_PrintLinef ("Note: Skip " QUOTED_S " (.dll)", filename_only);  continue;  /* No thanks */ }

				if (curname_slen <= skipcount) {
					Con_PrintLinef ("Note: Skip unpack of " QUOTED_S " (dest above gamedir)", filename_only);  continue;  /* No thanks */
					continue; // Happens
				}

				if (curname_slen > skipcount && skipcount > 0 && String_Does_Start_With_Caseless(cur->name, strip_prefix) == false ) {
					Con_PrintLinef ("Note: Skip unpack of " QUOTED_S " (dest above gamedir)", filename_only);  continue;  /* No thanks */
					continue; 
				} // This happens especially for things intended for the Quake folder.

				// Make sure is right basedir and is caseless match on the path
				//if (found_map && match_maps) {
				//	if (!String_Does_Start_With_Caseless (cur->name, "maps/"))
				//		continue; // We only found a map.  And it is subdir.
				//}


				{
					const char *filename_only = NULL;
					char lower_cur_name[MAX_OSPATH];
					char *dest_name;
					
					c_strlcpy (lower_cur_name, cur->name); String_Edit_To_Lower_Case (lower_cur_name);
					
					filename_only = File_URL_SkipPath(lower_cur_name);
					
					// Let's have all extractions be lower case out of courtesy

					if (found_map && skipcount == 0)
						dest_name = va("%s/%s", dest_base, lower_cur_name); // Maps only
					else dest_name = va("%s/%s", dest_base, &lower_cur_name[skipcount]); // Other

					if (!Zip_Extract_File (library_folder_zip_url, cur->name, dest_name)) {
						Con_DPrintLinef ("Warning: Couldn't extract %s", cur->name);
						Con_PrintLinef ("Extracted %03d: %s (EXTRACTION FAILED)", count + 1, skipcount == 0 ? lower_cur_name : &lower_cur_name[skipcount]);
						continue;
					}	
					
					// Success!
					Con_DPrintLinef ("Extracted: %s to %s", cur->name, dest_name);
					Con_PrintLinef  ("Extracted %03d: %s", count + 1, skipcount == 0 ? lower_cur_name : &lower_cur_name[skipcount]);	
		//			#pragma message ("We have a name borkage problem with some zips where first n characters are not a match, example is Nehahra")
				}
			}} // End for

		} // END UNPAK TIME

		// Update the lists
		if (found_what == found_map)	Lists_Update_Maps ();
		else							Lists_Update_ModList ();

cleanup:
		if (file_list_alloc) List_Free (&file_list_alloc);
	} // EXAMINE THE ZIP DONE

}

#if 0
void OpenD_Command_f (void)
{
//	Folder_Open
	Con_PrintLinef ("%s", System_Dialog_Open_Type ("Open a file", "c:/quake/cda", ".pak,.png"));
}

void SaveD_Command_f (void)
{
}
#endif
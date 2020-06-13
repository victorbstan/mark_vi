/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2009 John Fitzgibbons and others
Copyright (C) 2009-2018 Baker and others

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

// mnu_singleplayer_load_and_save.c - COMBINER -- Save and load

#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_Save
#define LOCAL_EVENT(NAME)		void Mnu_Save_ ## NAME

#define local_menu				(&menux[LOCAL_MENU_STATE]) // /*static menux_t *menu = &menux[OUR_MENU_STATE]


//=============================================================================
/* SAVE MENU */

#define	MAX_SAVEGAMES_20 20

static char		msav_descriptions[MAX_SAVEGAMES_20][SAVEGAME_COMMENT_LENGTH_39 + 1];
static int		loadable[MAX_SAVEGAMES_20];
static int		numautosaves;

void Mnu_SaveLoad_ScanSaves (cbool exclude_autosaves)
{
	exclude_autosaves = true;	// if (!sv_autosave.value)  // ALWAYS exclude autosaves!
	numautosaves = 0;			// Won't happen now that we aren't doing autosaves

	if (!exclude_autosaves) {
		{ int nn; for (nn = 0; nn < AUTO_SAVE_COUNT; nn ++) {
			char	name[MAX_OSPATH];
			FS_FullPath_From_QPath (name, va("a%d.sav", nn));
			if (File_Exists (name))
				numautosaves ++;
		}} // End for
	}

	// For all files s0.sav to s19.sav ...
	{ int k; for (k = 0; k < MAX_SAVEGAMES_20 ; k++) {
		FILE	*f;
		cbool	is_this_an_autosave = false;
		char	file_full_path[MAX_OSPATH];
		
		// Slot will say "Unused Slot" unless we find "s3.sav" or whatever num we are on.
		c_strlcpy (msav_descriptions[k], "--- UNUSED SLOT ---");
		loadable[k] = false;

		//if (k >= (MAX_SAVEGAMES_20 - numautosaves)) {
		//	FS_FullPath_From_QPath (name, va("a%d.sav", n));
		//	is_this_an_autosave = true;
		//}
		//else
		FS_FullPath_From_QPath (file_full_path, va("s%d.sav", k)); // Create a "quake\id1\s3.sav" file url

		f = FS_fopen_read (file_full_path, "rb");
		if (f) {
			char	description_buf[MAX_OSPATH]; // MAX_OSPATH length just because Quake had it this length.
			int		version;

			fscanf (f, "%d\n", &version);
			fscanf (f, "%79s\n", description_buf); // Now we re-use the name field and read in 79 characters I think.

			switch (is_this_an_autosave) {
			default:			c_strlcpy (msav_descriptions[k], "Auto:"); // Auto save
								c_strlcat (msav_descriptions[k], description_buf);
								msav_descriptions[k][21] = ' ';
								strlcpy (&msav_descriptions[k][22], &description_buf[22], SAVEGAME_COMMENT_LENGTH_39 - 22);
			
			case_break false:	c_strlcpy (msav_descriptions[k], description_buf); // Normal save
			}

			// change _ back to space
			{ int j; for (j = 0; j < SAVEGAME_COMMENT_LENGTH_39; j++) {
				if (msav_descriptions[k][j] == '_')
					msav_descriptions[k][j] = ' ';
			}} // End for j
			loadable[k] = true;
			FS_fclose (f);
		} // end if (f)
		
	}} // for k

}







//
// Draw
//

LOCAL_EVENT (Draw) (void) // Save
{
	int		n;
	qpic_t	*p;

	p = Draw_CachePic ("gfx/p_save.lmp");
	M_DrawPicCentered (4, p);

	for (n = 0 ; n < MAX_SAVEGAMES_20; n++) {
		Hotspots_Add (16, 32 + M_CHAR_HEIGHT_8 * n, local_menu->colwidth /*hotwidth*/, M_HOTHEIGHT_8, 1, hotspottype_button);
		M_Print (16, 32 + M_CHAR_HEIGHT_8 * n, msav_descriptions[n]);
	}

// line cursor
	M_DrawCharacter (8, 32 + M_CHAR_HEIGHT_8 * local_menu->cursor, 12+((int)(realtime*4)&1));
}

//
// Key
//

// Since key can be upper or lower case it isn't quite a scancode
LOCAL_EVENT (KeyPress) (key_scancode_e key, int hotspot) // Save
{
	int dir = 0; cbool did_move = false;
	switch (key) {
	default:					// Nothing
	case_break K_ESCAPE:		Mnu_SinglePlayer_Enter_f (NULL);
	case_break K_HOME:			local_menu->cursor = 0; did_move = true;
	case_break K_PAGEUP:		local_menu->cursor = 0; did_move = true;
	case_break K_PAGEDOWN:		local_menu->cursor = local_menu->cursor_solid_count - 1; did_move = true;
	case_break K_END:			local_menu->cursor = local_menu->cursor_solid_count - 1; did_move = true;
	case_break K_UPARROW:		dir = -1;
	case_break K_DOWNARROW:		dir = 1;
	case_break K_ENTER:			Key_SetDest (key_game);
								Cbuf_AddTextLinef ("save s%d", local_menu->cursor);
	} // End switch

	if (dir)		{ cursor_wrap_increment_0_count (local_menu->cursor, +dir, local_menu->cursor_solid_count); did_move = true; }
	if (did_move)	{ MENU_ROW_CHANGE_HARD_SOUND(); }
}

//
// Enter
//

LOCAL_EVENT (Enter_f) (lparse_t *unused) // Save
{
	// Baker: Deny save access if sv not active, intermission, multiplayer.  Remember, you can't save a demo!
	if (!sv.active) {
		Mnu_Dialog_Modal_Alert (NULL /*ok*/, "Not hosting a game!");
		return;
	}

	if (cl.intermission) {
		Mnu_Dialog_Modal_Alert (NULL /*ok*/, "Can't save during intermission!");
		return;
	}

	//if (svs.maxclients_internal != 1)		return;
	
	Key_SetDest  (key_menu); Menu_SetDest (LOCAL_MENU_STATE);
	
	Mnu_SaveLoad_ScanSaves (true);
}



//
// InitOnce
//

LOCAL_EVENT (InitOnce) (menux_t *self) // Save
{
	self->cursor_solid_count = MAX_SAVEGAMES_20;
}

// End of SAVE section

#undef LOCAL_MENU_STATE
#undef LOCAL_EVENT
#undef local_menu

#define LOCAL_MENU_STATE		menu_state_Load
#define LOCAL_EVENT(NAME)		void Mnu_Load_ ## NAME

#define local_menu				(&menux[LOCAL_MENU_STATE]) // /*static menux_t *menu = &menux[OUR_MENU_STATE]

//=============================================================================
/* LOAD MENU */

//
// Draw
//

LOCAL_EVENT (Draw) (void) // Load
{
	int		n;
	qpic_t	*p;

	p = Draw_CachePic ("gfx/p_load.lmp");
	M_DrawPicCentered (4, p);

	for (n = 0 ; n < MAX_SAVEGAMES_20; n++) {
		Hotspots_Add (16, 32 + M_CHAR_HEIGHT_8 * n, local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_button);
		M_Print (16, 32 + M_CHAR_HEIGHT_8 * n, msav_descriptions[n]);
	}

// line cursor
	M_DrawCharacter (8, 32 + local_menu->cursor * 8, 12+((int)(realtime*4)&1));
}

//
// Key
//

// Since key can be upper or lower case it isn't quite a scancode
LOCAL_EVENT (KeyPress) (key_scancode_e key, int hotspot) // Load
{
	int dir = 0; cbool did_move = false;

	switch (key) {
	default:					// Nothing
	case_break K_ESCAPE:		Mnu_SinglePlayer_Enter_f (NULL);
	case_break K_UPARROW:		dir = -1;
	case_break K_HOME:			local_menu->cursor = 0; did_move = true;
	case_break K_PAGEUP:		local_menu->cursor = 0; did_move = true;
	case_break K_PAGEDOWN:		local_menu->cursor = local_menu->cursor_solid_count - 1; did_move = true;
	case_break K_END:			local_menu->cursor = local_menu->cursor_solid_count - 1; did_move = true;
	case_break K_DOWNARROW:		dir = 1;
	case_break K_ENTER:			
		MENU_ENTER_LOUD_SOUND();
		if (!loadable[local_menu->cursor])
			break; // Get out
		
		// Host_Loadgame_f can't bring up the loading plaque because too much
		// stack space has been used, so do it now
		SCR_BeginLoadingPlaque ();
		/* no ... sMenu.menu_state_reenter = sMenu.menu_state; */ 
		sMenu.menu_state_reenter = sMenu.menu_state; Key_SetDest (key_game);
		console1.visible_pct = 0;
		
		// issue the load command
		if (local_menu->cursor >= MAX_SAVEGAMES_20 - numautosaves)
			Cbuf_AddTextLinef ("load a%d", local_menu->cursor - (MAX_SAVEGAMES_20 - numautosaves) );
		else Cbuf_AddTextLinef ("load s%d", local_menu->cursor);
	} // end switch (key)

	if (dir)		{ cursor_wrap_increment_0_count (local_menu->cursor, +dir, MAX_SAVEGAMES_20); did_move = true; }
	if (did_move)	{ MENU_ROW_CHANGE_HARD_SOUND(); }
}

//
// Enter
//

LOCAL_EVENT (Enter_f) (lparse_t *unused) // Load
{
	Key_SetDest	 (key_menu);  Menu_SetDest (LOCAL_MENU_STATE);

	Mnu_SaveLoad_ScanSaves (false);
}



//
// InitOnce
//

LOCAL_EVENT (InitOnce) (menux_t *self) // Load
{
	self->cursor_solid_count = MAX_SAVEGAMES_20;
}

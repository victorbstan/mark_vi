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

// mnu_multiplayer_demos.c
 
#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_Demos
#define LOCAL_EVENT(NAME)		void Mnu_Demos_ ## NAME
#define LOCAL_FN(NAME)			Mnu_Demos_ ## NAME

#define local_menu				(&menux[LOCAL_MENU_STATE]) // static menux_t *menu = &menux[OUR_MENU_STATE]

//=============================================================================
/* DEMOS MENU */


typedef struct {
	char demoname[MAX_QPATH_64];
	char age[5];					// Days old?
	char kilobytes[5];				// Demo size in kilobytes
	char mapstart[MAX_QPATH_64];
} demo_item_t;

static demo_item_t *m_items_list;

static int m_menu_line_count; 	// Varies depending how many are found
static int m_view_first_row;		// Top line # in the view
static int m_items_read;
static int m_items_count;

#define M_ROWS_VISIBILE_COUNT_17 17 // demos and levels
#define M_SCROLLBAR_IDX_17 17

const char *demos_tips[] = {"left arrow = demo rewind",  /*24*/ "press \\ to explore" /*20*/, "press first letter"};
const char *demos_tip;

// Baker: Staggered read.  Read 17 in at a time every frame
// for user convenience, instead of stalling for several seconds
// if there are a lot of maps.
static void Mnu_Demos_Read_ (void)
{
	int count;
	clist_t	*cursor;
	demo_item_t *item;

	if (!(m_items_list && m_items_read < m_items_count))
		return;

			// Now walk the tree again for last time
	for (
		cursor = (&list_info[list_type_demos_menu])->plist, count = 0;
		cursor && count < m_items_read + M_ROWS_VISIBILE_COUNT_17 /* do 17 at a time */;
		cursor = cursor->next, count ++
	)
	{
		item = &m_items_list[count];

		if (count < m_items_read)
			continue;  // Already did this

		c_strlcpy (item->demoname, cursor->name);

		// Now find the map name
		{
			int h;
			char qpath[MAX_QPATH_64];
			//cbool is_dzip = false;  Mar 6 2018 - Bye dzip

			//if (String_Does_End_With_Caseless (item->demoname, ".dz")) {
			//	c_strlcpy (qpath, item->demoname);
			//	is_dzip = true;
			//}
			//else 
				c_snprintf1 (qpath, "%s.dem", item->demoname);

			if (COM_OpenFile (qpath, &h) == -1)
				System_Error ("Can't open %s", item->demoname);

			if (!com_filesrcpak) {
				char namebuf[MAX_OSPATH];
				double filetime, nowtime, age;
				FS_FullPath_From_QPath (namebuf, qpath);
				filetime = File_Time (namebuf);
				nowtime = Time_Now ();
				age = (nowtime-filetime)/(24 * 60 * 60 /*seconds in a day */);

				c_snprintf1 (item->age, "%d", (int)age);
			} else  c_strlcpy (item->age, "pak");

			c_snprintf1 (item->kilobytes, "%d", com_filesize / 1024);

			if (1) {
				byte readbuf[300];
				byte *look;
				byte *lastslash;
				byte *bspstart;
				// Read the header
				System_FileRead (h, readbuf, sizeof(readbuf)-1);

				for (look = readbuf, bspstart = NULL, lastslash = NULL; look < readbuf + sizeof(readbuf) -4; look++) {
					if (look[0] == '.' && look[1] == 'b' && look[2] == 's' && look[3] == 'p' && look[4] == 0) {
						bspstart = look;
						break;
					} else if (look[0] == '/') lastslash = look;
				}

				if (bspstart) {
					File_URL_Copy_StripExtension (item->mapstart, (char*)&lastslash[1], sizeof(item->mapstart));
				}
				else c_strlcpy (item->mapstart, "??");

			}
			else c_strlcpy (item->mapstart, "[dzip]");


			// Find .bsp + NULL
			// Scan backwards to /
			// Get map name

			COM_CloseFile (h);

		} // End of "now find the level
	} // End of for loop #2

	m_items_read = count;
}


//
// Draw
//

LOCAL_EVENT (Draw) (void)
{
	hotspot_menu_group_t *f = &hotspot_menu_group;
	int num_items_drawn;

	int row;
	int yofs;

	const char *title1 =  "  name         age   kb     map"; // 38 chars
	const char *_title2 = "<============><====><=====><=========>"; // 38 chars
	static char title2[39] = {0};

	// Once per session we create the static title2 replacing
	// "<", "=" and ">" with special Quake characters.
	if (title2[0] == 0) {
		const char *src;
		char *dst;
		for (src = _title2, dst = title2; *src; src++, dst++) {
			     if (src[0] == '<') dst[0] = '\35';
			else if (src[0] == '=') dst[0] = '\36';
			else if (src[0] == '>') dst[0] = '\37';
			else dst[0] = src[0];
		}
		dst[0] = 0;
	}

	Mnu_Demos_Read_ (); // Staggered read


	if (f->focus && f->focus_event && f->focus_event == focus_event_thumb_position) {
		float pct = f->focus_event_thumb_top / (f->focus_down_track_usable - 1.0);
		int new_first_row = c_rint (pct * (f->focus_down_possibles - 1.0)); // Problem getting to 100?
		if (f->focus_idx != M_SCROLLBAR_IDX_17) Con_PrintLinef ("Levels hotspot not 17");
		if (f->focus_menu_state != LOCAL_MENU_STATE) Con_PrintLinef ("Menu state focus item not Levels Menu");
		// f->focus_event_thumb_top // Relative!
		m_view_first_row = new_first_row;
		m_view_first_row = CLAMP (0, m_view_first_row, m_menu_line_count < M_ROWS_VISIBILE_COUNT_17 ? 0 : m_menu_line_count - M_ROWS_VISIBILE_COUNT_17);

		// We need to bump the cursor into view.
		while (local_menu->cursor < m_view_first_row)
			local_menu->cursor ++;

		while (local_menu->cursor > m_view_first_row + (M_ROWS_VISIBILE_COUNT_17 - 1) )
			local_menu->cursor --;

		// Clear the message.
		f->focus_event = focus_event_none_0;
	}

	if (f->focus && f->focus_part && f->focus_event_msgtime_ext >= 0 && f->focus_event_msgtime_ext < realtime) {
		float repeat_time = 0;
		switch (f->focus_part) {
		default:	goto no_action; // Thumb or something.  Do nothing.
		case_break focus_part_button1:	LOCAL_FN (KeyPress) (K_MOUSEWHEELUP, NO_HOTSPOT_HIT_NEG1);		repeat_time = (1/32.0); // 16 per sec
		case_break focus_part_button2:	LOCAL_FN (KeyPress) (K_MOUSEWHEELDOWN, NO_HOTSPOT_HIT_NEG1);	repeat_time = (1/32.0); // 16 per sec
		case_break focus_part_track1:	LOCAL_FN (KeyPress) (K_PAGEUP, NO_HOTSPOT_HIT_NEG1);			repeat_time = 0; // Don't
		case_break focus_part_track2:	LOCAL_FN (KeyPress) (K_PAGEDOWN, NO_HOTSPOT_HIT_NEG1);			repeat_time = 0; // Don't
		}
		f->focus_event_msgtime_ext = repeat_time ? realtime + repeat_time : -1; // -1 = never.

no_action:
		(0); // Dummy statement :(
	}

	while (local_menu->cursor < m_view_first_row)
		m_view_first_row = local_menu->cursor;

	while (local_menu->cursor > (m_view_first_row + (M_ROWS_VISIBILE_COUNT_17 - 1)))
		m_view_first_row = local_menu->cursor - (M_ROWS_VISIBILE_COUNT_17 - 1);

	// Setup
	Draw_SetCanvas (CANVAS_MENU);

	Draw_Fill (0, 0, 320, 200, QUAKE_BLACK_0, 0.25);
	Draw_Fill (8, 8, 304, 184, QUAKE_BLACKISH_16, 0.25);

#ifdef GLQUAKE_ALPHA_DRAWING
	// Baker: we draw it slightly differently in WinQuake (lack of alpha drawing)
	Draw_Fill (16, 16, 288, 184, QUAKE_BLACKISH_16, 0.25);
#endif // GLQUAKE_ALPHA_DRAWING

	M_DrawTransPic (8, 8, Draw_CachePic ("gfx/demos.lmp"), NO_HOTSPOTS_0, USE_IMAGE_SIZE_NEG1, USE_IMAGE_SIZE_NEG1);

	M_Print ( (320 - 12) - strlen(demos_tip) * M_CHAR_WIDTH_8, 12, demos_tip);
	M_Print (8, 28, title1);
	M_Print (8, 36, title2);

	for (row = m_view_first_row, yofs = 52, num_items_drawn = 0; row < m_view_first_row + M_ROWS_VISIBILE_COUNT_17; row ++, yofs += 8) {
		if (row == local_menu->cursor)
			Draw_Character ( 8, yofs, 12+((int)(realtime*4)&1) );

		if 	(0 <= row && row < m_menu_line_count) {
			hotspot_menu_item_t *hs = Hotspots_Add (24, yofs, 34 * 8, 8, 1, hotspottype_listitem);
			hs->listindex = row;

			M_Print ( 24, yofs,
				va("%-11s  %4s  %5s  %-9s",
					va("%.11s", m_items_list[row].demoname),
					va("%.4s", m_items_list[row].age),
					va("%.5s", m_items_list[row].kilobytes),
					va("%.9s", m_items_list[row].mapstart)
				)  );
			num_items_drawn ++;
		} // end if row in right range
	} // end for row

	// Baker: For this screen, we demand the scrollbar be index 17
	// So if we did less hotspots than that -- add some blanks
	// Since the level menu has blank rows.
	while (num_items_drawn < M_SCROLLBAR_IDX_17) {
		hotspot_menu_item_t *hs = Hotspots_Add (0,0,0,0, 1, hotspottype_inert);
		num_items_drawn ++;
	} // end while


	{
		int scrollbar_idx = Mnu_Part_DrawVScrollbar (300, 52, 8, 136, /*button size*/ 8, /*thumb size*/ (M_ROWS_VISIBILE_COUNT_17 - 1),
			m_view_first_row, M_ROWS_VISIBILE_COUNT_17, m_menu_line_count);		
		if (scrollbar_idx != M_SCROLLBAR_IDX_17)
			System_Error ("Levels scrollbar idx not 17 but %d", scrollbar_idx);
	}
}

//
// Key
//

// Since key can be upper or lower case it isn't quite a scancode
LOCAL_EVENT (KeyPress) (key_scancode_e key, int hotspot)
{
	// If we have a hotspot we can't use the cursor (a list's cursor is funky)
	if (HS_LIST_IS_LISTINDEX(hotspot)) { // listitem emission list index
		if (key == K_ENTER) {
			int list_index = HS_LIST_LISTINDEX_DECODE(hotspot); // Reverse it back
			//Con_PrintLinef ("Hotspot list_index is %d", list_index);
			if (!in_range_beyond (0, list_index, m_menu_line_count))
				System_Error ("Bad idx %d (valid %d to %d)", list_index, 0, m_menu_line_count-1);

			// We're ok!
			local_menu->cursor = list_index; // Right?
			sMenu.menu_state_reenter = sMenu.menu_state; Key_SetDest (key_game);
			Cbuf_AddTextLinef ("playdemo %s", m_items_list[list_index].demoname);
		}
		return;
	}

	switch (key) {
	default: // Try to find the first item starting with that letter/number
		if (isdigit(key) || isalpha(key)) {
			int cursor_offset = local_menu->cursor - m_view_first_row;
			
			int found = IDX_NOT_FOUND_NEG1;

			key = tolower(key);
			{ int n; for (n = local_menu->cursor + 1; n < m_menu_line_count; n ++) {
				demo_item_t *item = &m_items_list[n];

				if (item->demoname[0] && item->demoname[0] == key) { found = n; break; }
			}} // end for

			if (found == IDX_NOT_FOUND_NEG1) { // Not found yet -- Wrap
				{ int n; for (n = 0; n < local_menu->cursor + 1; n ++) {
					demo_item_t *item = &m_items_list[n];

					if (item->demoname[0] && item->demoname[0] == key) { found = n; break; }
				}} // end for
			}

			if (found != IDX_NOT_FOUND_NEG1) {
				// Found one
				local_menu->cursor = found;
				m_view_first_row = local_menu->cursor - cursor_offset;

				m_view_first_row = CLAMP (0, m_view_first_row, m_menu_line_count - M_ROWS_VISIBILE_COUNT_17);
				local_menu->cursor = CLAMP (0, local_menu->cursor, m_menu_line_count - 1);
			}

		}

	case_break '\\':
		if (local_menu->cursor < m_menu_line_count && m_items_list[local_menu->cursor].demoname[0] && m_items_list[local_menu->cursor].kilobytes[0]) {
			if (String_Does_Match(m_items_list[local_menu->cursor].age, "pak")) {
				Mnu_Dialog_Modal_Alert (NULL, "Can't explore to demo." NEWLINE NEWLINE "It is in a pak file!");
				// Dialog.  Return state.  Function which can be null.  Timeout?
				// There are only 4 uses of it.
				break;
			}

			if (vid.screen.type == MODESTATE_WINDOWED) {
				char filebuf [MAX_OSPATH];

				FS_FullPath_From_QPath (filebuf, m_items_list[local_menu->cursor].demoname);
				// if (!String_Does_End_With (filebuf, ".dz"))  Mar 6 2018 - Bye dzip
					c_strlcat (filebuf, ".dem");

				Folder_Open_Highlight (filebuf);
			} else Mnu_Dialog_Modal_Alert (NULL, "Can't explore to file" NEWLINE "in full-screen mode." NEWLINE "ALT-ENTER toggles full-screen" NEWLINE "and windowed mode.");
		}
		
	case_break K_ESCAPE:			Mnu_MultiPlayer_Enter_f (NULL);
	case_break K_UPARROW:			local_menu->cursor --;
									local_menu->cursor = CLAMP (0, local_menu->cursor, m_menu_line_count - 1);
									// Make sure cursor is in view
									//while (local_menu->cursor < m_view_first_row) local_menu->cursor ++;
									while (local_menu->cursor > m_view_first_row + (M_ROWS_VISIBILE_COUNT_17 - 1) ) local_menu->cursor --;

	case_break K_DOWNARROW:			local_menu->cursor ++;
									local_menu->cursor = CLAMP (0, local_menu->cursor, m_menu_line_count - 1);

	case_break K_HOME:				local_menu->cursor = 0;
	case_break K_END:				local_menu->cursor = m_menu_line_count - 1;
	case_break K_PAGEUP:			m_view_first_row -= M_ROWS_VISIBILE_COUNT_17;
									local_menu->cursor -= M_ROWS_VISIBILE_COUNT_17;
									m_view_first_row = CLAMP (0, m_view_first_row, m_menu_line_count < M_ROWS_VISIBILE_COUNT_17 ? 0 : m_menu_line_count - M_ROWS_VISIBILE_COUNT_17);
									local_menu->cursor = CLAMP (0, local_menu->cursor, m_menu_line_count - 1);

	case_break K_PAGEDOWN:			m_view_first_row += M_ROWS_VISIBILE_COUNT_17;
									local_menu->cursor += M_ROWS_VISIBILE_COUNT_17;
									m_view_first_row = CLAMP (0, m_view_first_row, m_menu_line_count < M_ROWS_VISIBILE_COUNT_17 ? 0 : m_menu_line_count - M_ROWS_VISIBILE_COUNT_17);
									local_menu->cursor = CLAMP (0, local_menu->cursor, m_menu_line_count - 1);
	case_break K_MOUSEWHEELUP:		m_view_first_row -= 1;
									m_view_first_row = CLAMP (0, m_view_first_row, m_menu_line_count < M_ROWS_VISIBILE_COUNT_17 ? 0 : m_menu_line_count - M_ROWS_VISIBILE_COUNT_17);
									// Make sure cursor is in view
									while (local_menu->cursor < m_view_first_row) local_menu->cursor ++;

									while (local_menu->cursor > m_view_first_row + (M_ROWS_VISIBILE_COUNT_17 - 1) ) local_menu->cursor --;

	case_break K_MOUSEWHEELDOWN:	m_view_first_row += 1;
									m_view_first_row = CLAMP (0, m_view_first_row, m_menu_line_count < M_ROWS_VISIBILE_COUNT_17 ? 0 : m_menu_line_count - M_ROWS_VISIBILE_COUNT_17);

									while (local_menu->cursor < m_view_first_row) local_menu->cursor ++;

									while (local_menu->cursor > m_view_first_row + (M_ROWS_VISIBILE_COUNT_17 - 1) ) local_menu->cursor --;

	case_break K_ENTER:
		if (local_menu->cursor < m_menu_line_count && m_items_list[local_menu->cursor].demoname[0] && m_items_list[local_menu->cursor].kilobytes[0]) {
			sMenu.menu_state_reenter = sMenu.menu_state; Key_SetDest (key_game);
			Cbuf_AddTextLinef ("playdemo %s", m_items_list[local_menu->cursor].demoname);
		}

	} // End switch (key)
}

//
// Enter
//

LOCAL_EVENT (Enter_f) (lparse_t *unused)
{
	//int rando = rand();
	// This has to be before Menu_SetDest because Menu_SetDest draws!!!  At least it fake draws.
	float my_rand = RANDOM_FLOAT_0_1; // Because Quake messes with the randomness -- this tends to be the same number every time per session.
	
	demos_tip = my_rand > 0.5 ? demos_tips[0] : demos_tips[1];
	
	Key_SetDest (key_menu); Menu_SetDest (LOCAL_MENU_STATE);

	// Assess Demos
	{
		Lists_Update_Demos_Menu (); // Baker: We must get only ones for current gamedir!

		do /*once*/ {
			// Walk the tree, determine amount of depth
			int count;
			clist_t	*cursor;
			demo_item_t *item;

			// Free if needed
			if (m_items_list) {
				free (m_items_list);
				m_items_list = NULL;
			}

			// Baker: Deal with a count of 0 situation
			if (  (&list_info[list_type_demos_menu])->plist == NULL) {
				// Add an item to say there is nothing
				m_menu_line_count = count = 1;
				m_items_list = (demo_item_t *) calloc (sizeof(demo_item_t), m_menu_line_count);

				item = &m_items_list[0];
				c_strlcpy (item->demoname, "No demos!");
				break; // Bail!
			}

			for (cursor = (&list_info[list_type_demos_menu])->plist, count = 0; cursor; cursor = cursor->next, count ++)
				; // Just getting count

			// Allocate
			m_items_read = 0;
			m_items_count = count;

			m_menu_line_count = count;
			m_items_list = (demo_item_t *) calloc (sizeof(demo_item_t), m_menu_line_count);


		} while (0);

	} // Of assess demos

//	Con_PrintLinef ("Out of there");
}

//
// New Game
//

LOCAL_EVENT (NewGame) (void)
{
	local_menu->cursor = m_view_first_row = 0;
}

//
// InitOnce
//

LOCAL_EVENT (InitOnce) (menux_t *self)
{
	self->cursor_solid_count = COUNT_FLUID_NEG1; // Can change upon gamedir change
}

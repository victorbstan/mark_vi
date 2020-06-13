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

// mnu_singleplayer_levels.c

#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_Levels
#define LOCAL_EVENT(NAME)		void Mnu_Levels_ ## NAME
#define LOCAL_FN(NAME)			Mnu_Levels_ ## NAME

#define local_menu				(&menux[LOCAL_MENU_STATE]) // static menux_t *menu = &menux[OUR_MENU_STATE]

//=============================================================================
/* LEVELS MENU */


typedef struct {
	int depth;	// 0, 1, 2 or 3 --- 3 = caption only
	char mapname[MAX_QPATH_64];
	char levelname[MAX_QPATH_64];
} level_item_t;

static level_item_t *m_items_list;

static int m_menu_line_count; 	// Varies depending how many are found
static int m_view_first_row;		// Top line # in the view
static int m_items_read;
static int m_items_count;

#define M_ROWS_VISIBILE_COUNT_17 17 // demos and levels
#define M_SCROLLBAR_IDX_17 17


// Baker: Staggered read.  Read 17 in at a time every frame
// for user convenience, instead of stalling for several seconds
// if there are a lot of maps.
static void Mnu_Levels_Read_ (void)
{
	int count;
	char last_depthchar;
	clist_t	*cursor;
	level_item_t *item;

	if (!(m_items_list && m_items_read < m_menu_line_count))
		return;

	//alert ("Populating list ... %d of %d", m_items_read, m_menu_line_count);

	// Now walk the tree again for last time
	for (
		cursor = (&list_info[list_type_levels_menu])->plist, count = 0, last_depthchar = 0;
		cursor && count < m_items_read + M_ROWS_VISIBILE_COUNT_17 /* do 17 at a time */;
		cursor = cursor->next, count ++
	)
	{
		char cur_depthchar = cursor->name[0];
		item = &m_items_list[count];

		if (last_depthchar && cur_depthchar != last_depthchar) {
			const char *caption;
			last_depthchar = cur_depthchar;

			// Insert 3 rows
			item->depth = 3; c_strlcpy (item->levelname, ""); count ++; item = &m_items_list[count];
			if (cur_depthchar == '1')
				caption = "custom quake levels";
			else caption = "original quake levels";
			item->depth = 3; c_strlcpy (item->levelname, caption); count ++; item = &m_items_list[count];
			item->depth = 3; c_strlcpy (item->levelname, ""); count ++; item = &m_items_list[count];
		}
		else if (!last_depthchar) {
			last_depthchar = cur_depthchar;
		}

		switch (cur_depthchar) {
		default: System_Error ("Invalid levels menu depth char");
		case_break '0': item->depth = 0;
		case_break '1': item->depth = 1;
		case_break 'q': item->depth = 2;
		} // end switch

		if (count < m_items_read)
			continue;  // Already did this

		c_strlcpy (item->mapname, &cursor->name[2]);

		// Now find the level name
		{
			FILE	*f = NULL;
			dheader_t  header;
			char qpath[MAX_QPATH_64];
			size_t filestart;

			c_snprintf1 (qpath, "maps/%s.bsp", item->mapname);
			COM_FOpenFile (qpath, &f);

			if (!f)
				System_Error ("Can't open %s", item->mapname);

			// Get the seek
			filestart = ftell (f);

			// Read the header
			fread (&header, sizeof(dheader_t), 1, f);

			// Find the offsets
			if (!isin4 (header.version, BSPVERSION, BSPVERSION_HALFLIFE, BSP2VERSION_2PSB, BSP2VERSION_BSP2)) {
				c_strlcpy (item->levelname, "(Unsupported map)"); // Something unusual like Quake 3 ?
			} else {
				// Quake compatible maps
				const char *map_title;
				char entities_buffer[1000]={0};

				size_t entityoffset = filestart + header.lumps[LUMP_ENTITIES].fileofs;
				size_t amount_to_read = c_min ((size_t) header.lumps[LUMP_ENTITIES].filelen, sizeof(entities_buffer));

				// Seek forward to entity data
				fseek (f, entityoffset, SEEK_SET);
				fread (entities_buffer, amount_to_read, 1, f);
				entities_buffer[sizeof(entities_buffer)-1]=0; // Null terminate just in case.

				// Seek what map title is
				map_title = COM_CL_Worldspawn_Value_For_Key (entities_buffer, "message" /* this is map title */);

				// If map has title
				if (map_title) {
					unsigned char *c;
					c_strlcpy (item->levelname, map_title);
					// De-bronze
					for (c = (unsigned char*)item->levelname; c[0]; c++) {
						if (c[0] > 128) c[0] -= 128;
					} // End for
				} // end if (map_title)
			}

			FS_fclose (f);
			// Read 300 chars or less at LUMP_
		} // End of "now find the level and end switch (header.version)
	} // End of for loop #2

	m_items_read = count;
	//alert ("Exit read %d of %d", m_items_read, m_menu_line_count);
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

	Mnu_Levels_Read_ (); // Staggered read

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
		case_break focus_part_button1: LOCAL_FN (KeyPress) (K_MOUSEWHEELUP, NO_HOTSPOT_HIT_NEG1);		repeat_time = (1/32.0); // 16 per sec
		case_break focus_part_button2: LOCAL_FN (KeyPress) (K_MOUSEWHEELDOWN, NO_HOTSPOT_HIT_NEG1);		repeat_time = (1/32.0); // 16 per sec
		case_break focus_part_track1:  LOCAL_FN (KeyPress) (K_PAGEUP, NO_HOTSPOT_HIT_NEG1);				repeat_time = 0; // Don't
		case_break focus_part_track2:  LOCAL_FN (KeyPress) (K_PAGEDOWN, NO_HOTSPOT_HIT_NEG1);			repeat_time = 0; // Don't
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

	Draw_Fill (0,   0, 320, 200, QUAKE_BLACK_0, 0.25);
	Draw_Fill (8,   8, 304, 184, QUAKE_BLACKISH_16, 0.25);

#ifdef GLQUAKE_ALPHA_DRAWING
	// Baker: we draw it slightly differently in WinQuake (lack of alpha drawing)
	Draw_Fill (16, 16, 288, 184, QUAKE_BLACKISH_16, 0.25);
#endif // GLQUAKE_ALPHA_DRAWING

	M_DrawTransPic (8, 8, Draw_CachePic ("gfx/levels.lmp"), NO_HOTSPOTS_0, USE_IMAGE_SIZE_NEG1, USE_IMAGE_SIZE_NEG1);

	M_Print (8, 36, Con_Quakebar (13));
	M_Print (112, 36, Con_Quakebar (25));

	for (row = m_view_first_row, yofs = 52, num_items_drawn = 0; row < m_view_first_row + M_ROWS_VISIBILE_COUNT_17; row ++, yofs += 8) {
		if (row == local_menu->cursor)
			Draw_Character ( 8, yofs, 12+((int)(realtime*4)&1) );

		if 	(0 <= row && row < m_menu_line_count) {
			if (m_items_list[row].mapname[0]) {
				hotspot_menu_item_t *hs = Hotspots_Add (24, yofs, 34 * 8, 8, 1, hotspottype_listitem);
				hs->listindex = row;
				M_Print ( 24, yofs, va("%-10s  %.22s", va("%.10s", m_items_list[row].mapname), m_items_list[row].levelname));
				num_items_drawn ++;
			} else if (m_items_list[row].levelname[0]) {
				M_PrintWhite ( 24, yofs, m_items_list[row].levelname);	
			} // end if mapname[0[
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
			if (sv.active && cl.maxclients > 1)	Cbuf_AddTextLinef ("changelevel %s", m_items_list[list_index].mapname);
			else								Cbuf_AddTextLinef ("map %s", m_items_list[list_index].mapname);
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
				level_item_t *item = &m_items_list[n];

				if (item->mapname[0] && item->mapname[0] == key) { found = n; break; }
			}} // end for

			if (found == IDX_NOT_FOUND_NEG1) { // Not found yet -- Wrap
				{ int n; for (n = 0; n < local_menu->cursor + 1; n ++) {
					level_item_t *item = &m_items_list[n];

					if (item->mapname[0] && item->mapname[0] == key) { found = n; break; }
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

	case_break K_ESCAPE:			Mnu_Main_Enter_f (NULL);
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
		if (local_menu->cursor < m_menu_line_count && m_items_list[local_menu->cursor].mapname[0]) {
			sMenu.menu_state_reenter = sMenu.menu_state; Key_SetDest (key_game);
			if (sv.active && cl.maxclients > 1)	Cbuf_AddTextLinef ("changelevel %s", m_items_list[local_menu->cursor].mapname);
			else								Cbuf_AddTextLinef ("map %s", m_items_list[local_menu->cursor].mapname);
		}

	} // End switch (key)
}

//
// Enter
//

LOCAL_EVENT (Enter_f) (lparse_t *unused)
{
	Key_SetDest (key_menu); Menu_SetDest (LOCAL_MENU_STATE);

	// Assess Levels
	{
		Lists_Update_Levels_Menu (); // Baker: We must get only ones for current gamedir!
		{
			// Walk the tree, determine amount of depth
			int count;
			char last_depthchar;
			int num_depths;
			clist_t	*cursor;

			for (cursor = (&list_info[list_type_levels_menu])->plist, count = 0, last_depthchar = 0, num_depths = 0; cursor; cursor = cursor->next, count ++) {
				char cur_depthchar = cursor->name[0];
				if (cur_depthchar != last_depthchar) {
					num_depths ++;
					last_depthchar = cur_depthchar;
				}
			}

			// Free if needed
			if (m_items_list) {
				free (m_items_list);
				m_items_list = NULL;
			}

			// Allocate
			m_items_read = 0;
			m_items_count = count;

			m_menu_line_count = count + ((num_depths - 1) * 3); // 3 spaces between depths
			m_items_list = (level_item_t*) calloc (sizeof(level_item_t), m_menu_line_count);
		}

	} // Of assess levels

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

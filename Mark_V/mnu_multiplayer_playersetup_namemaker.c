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

// mnu_multiplayer_playersetup_namemaker.c

#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_NameMaker
#define LOCAL_EVENT(NAME)		void Mnu_NameMaker_ ## NAME

#define local_menu				(&menux[LOCAL_MENU_STATE]) // static menux_t *menu = &menux[OUR_MENU_STATE]

//=============================================================================
/* NAME EDITOR MENU */    // From JoeQuake 0.15 Dev
//=============================================================================

#define	NAMEMAKER_TABLE_SIZE_16	16
static int namemaker_cursor_x, namemaker_cursor_y;

//
// Draw
//

#define NAME_ROW_16			16
#define HS_SHADOW_4			4
#define HS_OUTLINE_1		1
#define LOCAL_ROWS_COUNT	(NAMEMAKER_TABLE_SIZE_16 - 1) // 15
#define LOCAL_COLS_COUNT	(NAMEMAKER_TABLE_SIZE_16 - 0) // 16

LOCAL_EVENT (Draw) (void)
{
	int	x, y;

	M_Print (48, 16, "Your name");
	M_DrawTextBox (120, 8, NAME_ROW_16, 1, 0 /* no hotspot */);
	M_PrintWhite (128, NAME_ROW_16, cl_name.string);

	M_DrawCharacter (128 + M_CHAR_WIDTH_8 * strlen(cl_name.string), NAME_ROW_16, 11);


	// Sad thing is we could draw this in one shot.
	for (y = 1; y < NAMEMAKER_TABLE_SIZE_16; y++) {
		int top = 22 + 10 * y;
		for (x = 0; x < NAMEMAKER_TABLE_SIZE_16; x++) {
			int left = 32 + (16 * x);
			int ch = NAMEMAKER_TABLE_SIZE_16 * y + x;
			if (ch == 255)
				continue; // Quake totally hates that character.

			Hotspots_Add (left - HS_OUTLINE_1 - 4, top - HS_OUTLINE_1, 8 +  ( HS_OUTLINE_1 * 2) + 8, 8 + HS_OUTLINE_1 * 2, 1, hotspottype_button_line);
			M_DrawCharacter (left, top, NAMEMAKER_TABLE_SIZE_16 * y + x);
		}
	}

//	if (namemaker_cursor_y == NAMEMAKER_TABLE_SIZE_16 - 1)
//		M_DrawCharacter (128, 184, 12 + ((int)(realtime*4)&1));
//	else
		M_DrawCharacter (24 + 16 * namemaker_cursor_x, 22  + 10 + 10 * namemaker_cursor_y, 12 + ((int)(realtime*4)&1));

	{
		char s[] = "Press ESC to exit";

		Hotspots_Add (144 - HS_SHADOW_4, 188 - HS_SHADOW_4, ARRAY_STRLEN(s) * 8 + (HS_SHADOW_4 * 2), 8 + (HS_SHADOW_4 * 2), 1, hotspottype_button);
		M_Print (144, 188, s);
	}
}


//
// Key
//

// Since key can be upper or lower case it isn't quite a scancode
LOCAL_EVENT (KeyPress) (key_scancode_e key, int hotspot)
{
	int	lengthus;
#define NAMEMAKER_ESCAPE_HOTSPOT_239 239
	if (hotspot == NAMEMAKER_ESCAPE_HOTSPOT_239) {
		key = K_ESCAPE;
	}

	else if (hotspot >= 0) {
		lengthus = strlen (cl_name.string);
		if (lengthus < 15) {
			int newchar = hotspot + 16;  // We start after the first row to skip control characters

			SETBUF (cl_name.string); // Sets stringbuf, len = buflen
			stringbuf[buflen + 1] = 0;
//				if (newchar == 0 || (newchar <= 13))
//					newchar = ' ';  // These characters cause too much chaos.
			stringbuf[buflen] = newchar;
			Cbuf_AddTextLinef ("name " QUOTED_S, stringbuf);
		}
		
		return;
	}

	switch (key)
	{
	default:						if (key < SPACE_CHAR_32 || key > MAX_ASCII_DELETE_CHAR_127)
										break; // Don't accept non-printable characters

									lengthus = strlen (cl_name.string);
									if (isin2 (key, 'v', 'V') && Key_Ctrl_Down() ) {
										if (strlen (Clipboard_Get_Text_Line ()))
											Cbuf_AddTextLinef ("name " QUOTED_S, Clipboard_Get_Text_Line() );
									}
									else if (lengthus < 15) {
										// int newchar = NAMEMAKER_TABLE_SIZE_16 * namemaker_cursor_y + namemaker_cursor_x;
										SETBUF (cl_name.string); // Sets stringbuf, len = buflen
										stringbuf[buflen + 1] = 0;
										stringbuf[buflen] = Key_Alt_Down() ? key | 128 : key;
										Cbuf_AddTextLinef ("name " QUOTED_S, stringbuf);
									}

	case_break K_ESCAPE:			Mnu_PlayerSetup_Enter_f (NULL);
	case_break K_UPARROW:			cursor_wrap_increment_0_count (namemaker_cursor_y, -1, LOCAL_ROWS_COUNT);
	case_break K_DOWNARROW:			cursor_wrap_increment_0_count (namemaker_cursor_y, +1, LOCAL_ROWS_COUNT);
	case_break K_PAGEUP:			namemaker_cursor_y = 0;
	case_break K_PAGEDOWN:			namemaker_cursor_y = LOCAL_ROWS_COUNT - 1;
	case_break K_LEFTARROW:			cursor_wrap_increment_0_count (namemaker_cursor_x, -1, LOCAL_COLS_COUNT);
	case_break K_RIGHTARROW:		cursor_wrap_increment_0_count (namemaker_cursor_x, +1, LOCAL_COLS_COUNT);
	case_break K_HOME:				namemaker_cursor_x = 0;
	case_break K_END:				namemaker_cursor_x = LOCAL_COLS_COUNT - 1;
	case_break K_BACKSPACE:			SETBUF (cl_name.string); // Sets stringbuf, len = buflen
									stringbuf[buflen - 1] = 0;
									Cbuf_AddTextLinef ("name " QUOTED_S, stringbuf);
	// case_break K_SPACE:  Previously was simulating enter?
	case_break K_ENTER:
			lengthus = strlen (cl_name.string);
			if (lengthus < 15) {
				int newchar = NAMEMAKER_TABLE_SIZE_16 * (namemaker_cursor_y + 1) + namemaker_cursor_x;
				SETBUF (cl_name.string); // Sets stringbuf, len = buflen
				stringbuf[buflen + 1] = 0;
//				if (newchar == 0 || (newchar <= 13))
//					newchar = ' ';  // These characters cause too much chaos.
				stringbuf[buflen] = newchar;
				Cbuf_AddTextLinef ("name " QUOTED_S, stringbuf);
			}

		

	}
}

//
// Enter
//

LOCAL_EVENT (Enter_f) (lparse_t *unused)
{
	Key_SetDest (key_menu); Menu_SetDest (LOCAL_MENU_STATE);
	
}

//
// InitOnce
//

LOCAL_EVENT (InitOnce) (menux_t *self)
{
	self->cursor_solid_count = COUNT_FLUID_NEG1; // Name maker has 2-dimensional cursoring.  Don't even try -- ha!
}


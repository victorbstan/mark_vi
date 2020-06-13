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

// mnu_multiplayer_playersetup.c

#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_PlayerSetup
#define LOCAL_EVENT(NAME)		void Mnu_PlayerSetup_ ## NAME

#define local_menu				(&menux[LOCAL_MENU_STATE]) // /*static menux_t *menu = &menux[OUR_MENU_STATE]

// Define menu items
typedef enum { ENUM_FORCE_INT_GCC_ (opt)
	opt_host_name_0,
	opt_your_name_1,
	opt_shirt_color_2,
	opt_pants_color_3,
	opt_accept_4,
	opt_COUNT // Ender
} opt_e;

//=============================================================================
/* SETUP MENU */


int  setup_cursor_table[opt_COUNT] = {40, 56, 80, 104, 140};

#define SETUP_TOP_COLOR			(((int)cl_color.value) >> 4)  // Divide by 16
#define SETUP_BOTTOM_COLOR		(((int)cl_color.value) & 15)

#define QUAKE_MAX_ACCEPTABLE_COLOR_13 13

char stringbuf[16]; int buflen; // Stupidly shared amongst several menu items :(


//
// Draw
//

LOCAL_EVENT (Draw) (void)
{
	qpic_t	*p;

	M_DrawTransPic (16, 4, Draw_CachePic ("gfx/qplaque.lmp"), NO_HOTSPOTS_0, USE_IMAGE_SIZE_NEG1, USE_IMAGE_SIZE_NEG1);
	p = Draw_CachePic ("gfx/p_multi.lmp");
	M_DrawPicCentered (4, p);
	
	Hotspots_Add (local_menu->column1, 40, local_menu->colwidth, 8, 1, hotspottype_textbutton); // Formerly the inert hotspottype_text
	M_Print (64, 40, "Hostname");
	M_DrawTextBox (160, 32, 16, 1, NO_HOTSPOTS_0);
	M_PrintWhite (168, 40, hostname.string);

	Hotspots_Add (local_menu->column1, 56, local_menu->colwidth, 8, 1, hotspottype_textbutton);
	M_Print (64, 56, "Your name");
	M_DrawTextBox (160, 48, 16, 1, NO_HOTSPOTS_0);
	M_PrintWhite (168, 56, cl_name.string); // Baker 3.83: Draw it correctly

	Hotspots_Add (local_menu->column1, 80, local_menu->colwidth, 8, 1, hotspottype_toggle);
	M_Print (64, 80, "Shirt color");
	Hotspots_Add (local_menu->column1, 104, local_menu->colwidth, 8, 1, hotspottype_toggle);
	M_Print (64, 104, "Pants color");

	M_DrawTextBox (64, 140-8, 14, 1, 6 /* border 6 */);
	M_Print (72, 140, "Accept Changes");

	p = Draw_CachePic ("gfx/bigbox.lmp");
	M_DrawTransPic (160, 64, p, NO_HOTSPOTS_0, USE_IMAGE_SIZE_NEG1, USE_IMAGE_SIZE_NEG1);
	p = Draw_CachePic ("gfx/menuplyr.lmp");
#ifdef GLQUAKE_COLORMAP_TEXTURES
	M_DrawTransPicTranslate (172, 72, p, SETUP_TOP_COLOR, SETUP_BOTTOM_COLOR);
#else // WinQuake ...
	M_BuildTranslationTable(SETUP_TOP_COLOR * 16, SETUP_BOTTOM_COLOR * 16);
	M_DrawTransPicTranslate (172, 72, p);
#endif // GLQUAKE_COLORMAP_TEXTURES

	M_DrawCharacter (56, setup_cursor_table [local_menu->cursor], 12+((int)(realtime*4)&1));

	if (local_menu->cursor == opt_host_name_0)
		M_DrawCharacter (168 + 8 * strlen(hostname.string), setup_cursor_table [local_menu->cursor], 10+((int)(realtime*4)&1));

	if (local_menu->cursor == opt_your_name_1)
		M_DrawCharacter (168 + 8 * strlen(cl_name.string), setup_cursor_table [local_menu->cursor], 10+((int)(realtime*4)&1));

	{ // Help text honors hover
		int desc_idx = menux[menu_state_PlayerSetup].hover ? menux[menu_state_PlayerSetup].hover->idx : local_menu->cursor;
		if (desc_idx == opt_your_name_1)			
			Draw_StringEx (72, 156, "\bPress \bENTER\b for name editor");
	}
}


//
// Key
//

// Since key can be upper or lower case it isn't quite a scancode
LOCAL_EVENT (KeyPress) (key_scancode_e key, int hotspot)
{
	int setup_top = SETUP_TOP_COLOR, setup_bottom = SETUP_BOTTOM_COLOR; // Just to have them available.
	int	lengthus;

	switch (key) {
	default:
		if (!in_range (32, key, 127))
			break; // Not ascii

		if (local_menu->cursor == opt_host_name_0) {
			lengthus = strlen (hostname.string);

			if (isin2 (key, 'v', 'V') && Key_Ctrl_Down()) {
				if (strlen (Clipboard_Get_Text_Line ()))
					Cvar_SetQuick (&hostname, Clipboard_Get_Text_Line() );
			}
			else
			if (lengthus < 15) {
				SETBUF (hostname.string); // Sets stringbuf, len = buflen
				stringbuf[buflen + 1] = 0;
				stringbuf[buflen] = key;
				Cvar_SetQuick (&hostname, stringbuf);
			}
		}
		if (local_menu->cursor == opt_your_name_1) {
			lengthus = strlen (cl_name.string);

			if (isin2 (key, 'v', 'V') && Key_Ctrl_Down()) {
				if (strlen (Clipboard_Get_Text_Line ()))
					Cbuf_AddTextLinef ("name " QUOTED_S, Clipboard_Get_Text_Line() );
			}
			else if (lengthus < 15) {
				SETBUF (cl_name.string); // Sets stringbuf, len = buflen
				stringbuf[buflen + 1] = 0;
				stringbuf[buflen] = Key_Alt_Down() ? key | 128 : key;
				Cbuf_AddTextLinef ("name " QUOTED_S, stringbuf);
			}
		}
	case_break K_ESCAPE:			Mnu_MultiPlayer_Enter_f (NULL);

	case_break K_UPARROW:			MENU_ROW_CHANGE_HARD_SOUND(); local_menu->cursor--; if (local_menu->cursor < 0) local_menu->cursor = local_menu->cursor_solid_count - 1;
	case_break K_DOWNARROW:			MENU_ROW_CHANGE_HARD_SOUND(); local_menu->cursor++; if (local_menu->cursor >= local_menu->cursor_solid_count) local_menu->cursor = 0;


	case_break K_LEFTARROW:
		switch (local_menu->cursor) {
		default:						// Nothing
		case_break opt_shirt_color_2:	MENU_TOGGLE_SOFT_SOUND(); setup_top --; if (setup_top < 0) setup_top = QUAKE_MAX_ACCEPTABLE_COLOR_13;
										Cbuf_AddTextLinef ("color %d %d", setup_top, setup_bottom);
		case_break opt_pants_color_3:	MENU_TOGGLE_SOFT_SOUND(); setup_bottom --;if (setup_bottom < 0) setup_bottom = QUAKE_MAX_ACCEPTABLE_COLOR_13;
										Cbuf_AddTextLinef ("color %d %d", setup_top, setup_bottom);
		}

	case_break K_RIGHTARROW:

forward:
		switch (local_menu->cursor) {
		default:						// Nothing
		case_break opt_shirt_color_2:	MENU_TOGGLE_SOFT_SOUND(); setup_top ++; if (setup_top > QUAKE_MAX_ACCEPTABLE_COLOR_13) setup_top = 0;
										Cbuf_AddTextLinef ("color %d %d", setup_top, setup_bottom);
		case_break opt_pants_color_3:	MENU_TOGGLE_SOFT_SOUND(); setup_bottom ++; if (setup_bottom > QUAKE_MAX_ACCEPTABLE_COLOR_13) setup_bottom = 0;
										Cbuf_AddTextLinef ("color %d %d", setup_top, setup_bottom);
		}

	case_break K_BACKSPACE:
		if (local_menu->cursor == opt_host_name_0) {
			if (strlen (hostname.string)) { // Basically back off one character in a buffer and then Cvar_Set it
				SETBUF (hostname.string); // Sets stringbuf, len = buflen
				stringbuf[buflen - 1] = 0;
				Cvar_SetQuick (&hostname, stringbuf);
			}
		}

		if (local_menu->cursor == opt_your_name_1) {
			if (strlen (cl_name.string)) { // Basically back off one character in a buffer and then Cvar_Set it
				SETBUF (cl_name.string); // Sets stringbuf, len = buflen
				stringbuf[buflen - 1] = 0;
				Cbuf_AddTextLinef ("name " QUOTED_S, stringbuf);
			}
		}

	case_break K_ENTER:
		switch (local_menu->cursor) {
		default:						// The other options do nothing
		case_break opt_host_name_0:		// Typically we do nothing.
										if (vid.touch_screen_active) {
											Mnu_OnScreenKeyboard_PromptText (
												"Hostname", &hostname, /*no buffer*/ NULL, /*sizeof buf*/ 16,
												LOCAL_MENU_STATE
											);
										}

		case_break opt_your_name_1:		Mnu_NameMaker_Enter_f (NULL);
		case_break opt_shirt_color_2:	goto forward;
		case_break opt_pants_color_3:	goto forward;
		case_break opt_accept_4:		Mnu_MultiPlayer_Enter_f (NULL);
		} // End switch cursor K_ENTER
	} // End switch key

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
	DEBUG_COMPILE_TIME_ASSERT (opt_COUNT, opt_COUNT == 5);
	self->cursor_solid_count = opt_COUNT;
}


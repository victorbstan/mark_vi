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

// mnu_main.c

#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_Main
#define LOCAL_EVENT(NAME)		void Mnu_ ## Main ## _ ## NAME

#define local_menu				(&menux[LOCAL_MENU_STATE]) // /*static menux_t *menu = &menux[OUR_MENU_STATE]

// Define menu items
typedef enum { ENUM_FORCE_INT_GCC_ (opt)
	opt_single_player_0,
	opt_multiplayer_game_1,
	opt_options_2,
	opt_help_3,
	opt_quit_4, // Nehahra?  No we don't honor the demos menu here.
	opt_COUNT // Ender
} opt_e;

//=============================================================================
/* MAIN MENU */


//
// Draw
//

LOCAL_EVENT (Draw) (void)
{
	qpic_t	*p;
	M_DrawTransPic (16, 4, Draw_CachePic ("gfx/qplaque.lmp"), NO_HOTSPOTS_0, USE_IMAGE_SIZE_NEG1, USE_IMAGE_SIZE_NEG1);
	p = Draw_CachePic ("gfx/ttl_main.lmp");
		M_DrawPicCentered (4, p);

#ifdef SUPPORTS_NEHAHRA
	if (nehahra_active) // For Nehahra "gamemenu.lmp" is the normal looking one.  Nehahra "mainmenu.lmp" has a "Play Demo" menu.
		M_DrawTransPic (72, 32, Draw_CachePic ("gfx/gamemenu.lmp"), 5, USE_IMAGE_SIZE_NEG1, Q_FAT_ROW_SIZE_20);
	else
#endif // !SUPPORTS_NEHAHRA
	M_DrawTransPic (72, 32, Draw_CachePic ("gfx/mainmenu.lmp"), 5, USE_IMAGE_SIZE_NEG1, Q_FAT_ROW_SIZE_20);

	{  // The spinning Q cursor on this screen has 6 animation frames.  gfx/menudot1.lmp to gfx/menudot6.lmp
		int q_cursor_animation_frame = (int)(realtime * 10) % 6;
		M_DrawTransPic (54, 32 + local_menu->cursor * Q_FAT_ROW_SIZE_20, Draw_CachePic_Sfmt ("gfx/menudot%d.lmp", q_cursor_animation_frame + 1), NO_HOTSPOTS_0, USE_IMAGE_SIZE_NEG1, USE_IMAGE_SIZE_NEG1);
	}
}


//
// Key
//

// Since key can be upper or lower case it isn't quite a scancode
LOCAL_EVENT (KeyPress) (key_scancode_e key, int hotspot)
{
	switch (key) {
	default:				// Nothing
	case_break K_ESCAPE:		Key_SetDest (key_game);
	case_break K_DOWNARROW: 	MENU_ROW_CHANGE_HARD_SOUND (); if (++local_menu->cursor >= local_menu->cursor_solid_count) local_menu->cursor = 0;
	case_break K_UPARROW: 		MENU_ROW_CHANGE_HARD_SOUND (); if (--local_menu->cursor < 0) local_menu->cursor = local_menu->cursor_solid_count - 1;

	case_break K_ENTER:
		switch (local_menu->cursor) {
		default:							// Nothing
		case_break opt_single_player_0:		Mnu_SinglePlayer_Enter_f (NULL);
		case_break opt_multiplayer_game_1:	Mnu_MultiPlayer_Enter_f (NULL);
		case_break opt_options_2:			Mnu_Options_Enter_f (NULL);
		case_break opt_help_3:				Mnu_Help_Enter_f (NULL);
		case_break opt_quit_4:				Mnu_Quit_Enter_f (NULL);
		} // end switch (cursor)
	} // end switch (key)
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
	self->cursor_solid_count = opt_COUNT; // Immutable
}

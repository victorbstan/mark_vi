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

// mnu_multiplayer.c

#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_MultiPlayer
#define LOCAL_EVENT(NAME)		void Mnu_ ## MultiPlayer ## _ ## NAME

#define	LOCAL_ITEMS_COUNT		(normal_singleplayer_menu ? 4 : 3)
#define local_menu				(&menux[LOCAL_MENU_STATE]) // /*static menux_t *menu = &menux[OUR_MENU_STATE]

// Define menu items
typedef enum { ENUM_FORCE_INT_GCC_ (opt)
	opt_join_0,
	opt_new_game_1,
	opt_setup_2,
	opt_demos_3,
	opt_COUNT // Ender
} opt_e;

//=============================================================================
/* MULTIPLAYER MENU */


// Entering this menu.  Ideally would create hotspots here.
// Weakness: Ideally we would like gamedir change to occur asychronous which means it could change here.
// That being said, that is an unusual situation that probably isn't worthy of our time.
// Levels, menus, server setup have entry setup.  Perfect would be some sort of menu exit/refresh being triggered.

//
// Draw
//

LOCAL_EVENT (Draw) (void)
{
	qpic_t	*p;
	int spwidth;
	{
		// Baker: To peek at single player menu to see if it is "normal"
		qpic_t	*px = Draw_CachePic ("gfx/sp_menu.lmp");
		if (!px) System_Error ("Couldn't load sp_menu.lmp"); // Baker: Trick compiler to make sure above line is run
	}

	M_DrawTransPic (16, 4, Draw_CachePic ("gfx/qplaque.lmp"), NO_HOTSPOTS_0, USE_IMAGE_SIZE_NEG1, USE_IMAGE_SIZE_NEG1);

	p = Draw_CachePic ("gfx/p_multi.lmp"); // "Multiplayer"
		M_DrawPicCentered (4, p);

	p = Draw_CachePic ("gfx/mp_menu.lmp"); // "Join A Game, New Game, Setup"
		spwidth = p->width;

		M_DrawTransPic (72, 32, p, 3, USE_IMAGE_SIZE_NEG1, USE_IMAGE_SIZE_NEG1);
	
	// Draw the additive demos.lmp if the menu is the standard Quake menu.
	if (normal_singleplayer_menu)
		M_DrawTransPic (72, 34 + Q_FAT_ROW_SIZE_20 * 3, Draw_CachePic ("gfx/demos.lmp"), 1, spwidth, USE_IMAGE_SIZE_NEG1);


	// Baker: Interesting that we some bounds checking here.
	if (local_menu->cursor >= LOCAL_ITEMS_COUNT)
		local_menu->cursor = 0;

	{  // The spinning Q cursor on this screen has 6 animation frames.  gfx/menudot1.lmp to gfx/menudot6.lmp
		int q_cursor_animation_frame = (int)(realtime * 10) % 6;
		M_DrawTransPic (54, 32 + local_menu->cursor * Q_FAT_ROW_SIZE_20, Draw_CachePic_Sfmt ("gfx/menudot%d.lmp", q_cursor_animation_frame + 1), NO_HOTSPOTS_0, USE_IMAGE_SIZE_NEG1, USE_IMAGE_SIZE_NEG1);
	}

	if (!ipv4Available && !ipv6Available) {
		M_PrintWhite ((320/2) - ((27 * M_CHAR_WIDTH_8) / 2), 148, "No Communications Available");
	}

	if (!static_registered)
		M_Print (M_CANVAS_HALF_WIDTH_160 - (28 * M_CHAR_WIDTH_8)/2, 160, "\220 shareware maps available \221"); // The \220 is Octal of 144 which is a Quake "["


}


//
// Key
//

// Since key can be upper or lower case it isn't quite a scancode
LOCAL_EVENT (KeyPress) (key_scancode_e key, int hotspot)
{
	switch (key) {
	default:				// Nothing
	case_break K_ESCAPE:	Mnu_Main_Enter_f (NULL);
	case_break K_DOWNARROW:	MENU_ROW_CHANGE_HARD_SOUND (); if (++local_menu->cursor >= LOCAL_ITEMS_COUNT) local_menu->cursor = 0;
	case_break K_UPARROW:	MENU_ROW_CHANGE_HARD_SOUND (); if (--local_menu->cursor < 0) local_menu->cursor = LOCAL_ITEMS_COUNT - 1;
		
	case_break K_ENTER:		
		// Enter for what hotspot?
		switch (local_menu->cursor) {
		default:					// Nothing
		case_break opt_join_0:		if (ipv4Available || ipv6Available) Mnu_JoinGame_Enter_f (NULL);
		case_break opt_new_game_1:	if (ipv4Available || ipv6Available)	Mnu_HostGame_Enter_f (NULL);
		case_break opt_setup_2:		Mnu_PlayerSetup_Enter_f (NULL);
		case_break opt_demos_3:		Mnu_Demos_Enter_f (NULL);
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
	self->cursor_solid_count = COUNT_FLUID_NEG1; // Can change upon gamedir change
}

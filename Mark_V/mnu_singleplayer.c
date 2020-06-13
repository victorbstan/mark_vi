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

// mnu_singleplayer.c

#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_SinglePlayer
#define LOCAL_EVENT(NAME)		void Mnu_SinglePlayer_ ## NAME

#define local_menu				(&menux[LOCAL_MENU_STATE]) // /*static menux_t *menu = &menux[OUR_MENU_STATE]

// Define menu items
typedef enum { ENUM_FORCE_INT_GCC_ (opt)
	opt_new_game_0,
	opt_load_1,
	opt_save_2,
	opt_levels_3,
	opt_COUNT // Ender
} opt_e;

//=============================================================================
/* SINGLE PLAYER MENU */

#define	LOCAL_ITEMS_COUNT	(normal_singleplayer_menu && !COM_CheckParm ("-no_levels") ? 4: 3)

#ifdef SUPPORTS_LEVELS_MENU_HACK
	int normal_singleplayer_menu = 1;
	int normal_backtile = 1;
#endif // SUPPORTS_LEVELS_MENU_HACK


//
// Draw
//

LOCAL_EVENT (Draw) (void)
{
	qpic_t	*p;
	int spwidth;

	M_DrawTransPic (16, 4, Draw_CachePic ("gfx/qplaque.lmp"), NO_HOTSPOTS_0, USE_IMAGE_SIZE_NEG1, USE_IMAGE_SIZE_NEG1);

	p = Draw_CachePic ("gfx/ttl_sgl.lmp"); // "Single Player"
		M_DrawPicCentered (4, p);

	p = Draw_CachePic ("gfx/sp_menu.lmp"); // "New Game, Load, Save"
		spwidth = p->width;

	M_DrawTransPic (72, 32, p, 3, USE_IMAGE_SIZE_NEG1, USE_IMAGE_SIZE_NEG1);

#ifdef SUPPORTS_LEVELS_MENU_HACK
	// Draw the additive demos.lmp if the menu is the standard Quake menu.
	if (normal_singleplayer_menu && !COM_CheckParm ("-no_levels"))
		M_DrawTransPic (72, 32 + Q_FAT_ROW_SIZE_20 * 3, Draw_CachePic ("gfx/levels.lmp"), 1, spwidth, USE_IMAGE_SIZE_NEG1);
#endif // SUPPORTS_LEVELS_MENU_HACK

	// Baker: Interesting that we some bounds checking here.
	if (local_menu->cursor >= LOCAL_ITEMS_COUNT)
		local_menu->cursor = 0;

	{  // The spinning Q cursor on this screen has 6 animation frames.  gfx/menudot1.lmp to gfx/menudot6.lmp
		int q_cursor_animation_frame = (int)(realtime * 10) % 6;
		M_DrawTransPic (54, 32 + local_menu->cursor * Q_FAT_ROW_SIZE_20, Draw_CachePic_Sfmt ("gfx/menudot%d.lmp", q_cursor_animation_frame + 1 ), NO_HOTSPOTS_0, USE_IMAGE_SIZE_NEG1, USE_IMAGE_SIZE_NEG1);
	}

}

static void Mnu_SinglePlayer_Do_NewGame_ (void)
{
	if (sv.active)
		CL_Disconnect ();

	SCR_BeginLoadingPlaque_Force_NoTransition ();
	Key_SetDest (key_game);
	console1.visible_pct = 0;

	Cbuf_AddTextLine ("maxplayers 1");
	Cbuf_AddTextLine ("deathmatch 0"); //johnfitz
	Cbuf_AddTextLine ("coop 0"); //johnfitz
	Cbuf_AddTextLine ("resetcvar sv_progs"); //johnfitz

	Cbuf_AddTextLinef ("map %s", game_startmap);
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
	case_break K_DOWNARROW:	MENU_ROW_CHANGE_HARD_SOUND(); if (++local_menu->cursor >= LOCAL_ITEMS_COUNT) local_menu->cursor = 0;
	case_break K_UPARROW:	MENU_ROW_CHANGE_HARD_SOUND(); if (--local_menu->cursor < 0) local_menu->cursor = LOCAL_ITEMS_COUNT - 1;

	case_break K_ENTER:			
		// Enter for what hotspot?
		switch (local_menu->cursor) {
		default:					// Nothing
		case_break opt_new_game_0:	// If server is active, ask the question.  Otherwise just do it
									if (sv.active)	Mnu_Dialog_Modal_YesNo (Mnu_SinglePlayer_Do_NewGame_, NULL, "Are you sure you want to" NEWLINE "start a new game?");
									else 			Mnu_SinglePlayer_Do_NewGame_ ();
		case_break opt_load_1:		Mnu_Load_Enter_f (NULL);
		case_break opt_save_2:		Mnu_Save_Enter_f (NULL); // Remember this might get "blocked" if not running a single player game
		case_break opt_levels_3:	Mnu_Levels_Enter_f (NULL);
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


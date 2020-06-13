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

// mnu_help.c

#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_Help
#define LOCAL_EVENT(NAME)		void Mnu_ ## Help ## _ ## NAME

#define	LOCAL_ITEMS_COUNT		6 // # of help screens

//=============================================================================
/* HELP MENU */

static int		help_page;
static int		normal_tileclear = 1;
int				normal_help = 1; // This is actually by gl_draw.c and draw.c



//
// Draw
//

LOCAL_EVENT (Draw) (void)
{
	qpic_t *p = Draw_CachePic ("gfx/help0.lmp");
	
	if (!p) System_Error ("Can't find help0.lmp"); // Baker trick compiler into thinking variable "p" is used.

	Hotspots_Add (0, 0, p->width, p->height, 1, hotspottype_screen);
	
	M_DrawPic (0, 0, Draw_CachePic_Sfmt ("gfx/help%d.lmp", help_page)/**/ );
}


//
// Key
//

// Since key can be upper or lower case it isn't quite a scancode
LOCAL_EVENT (KeyPress) (key_scancode_e key, int hotspot)
{
	//if (hotspot == 0)
	//	key = K_ENTER;
	//else if (hotspot == 1)
	//	key = K_ESCAPE;
	int dir = 0;

	switch (key) {
	case_break K_ESCAPE:		Mnu_Main_Enter_f (NULL);
	case_break K_ENTER:			dir =  1;
	case_break K_LEFTARROW:		dir = -1;
	case_break K_RIGHTARROW:	dir =  1;
	case_break K_UPARROW:		dir = -1;
	case_break K_DOWNARROW:		dir =  1;
	} // end switch (key)

	if (dir) {
		sMenu.entersound = true; // Play page flipping sound
		cursor_wrap_increment_0_count (help_page, +dir, LOCAL_ITEMS_COUNT);
	} // end if (dir)

}

//
// Enter
//

LOCAL_EVENT (Enter_f) (lparse_t *unused)
{
	Key_SetDest (key_menu); Menu_SetDest (LOCAL_MENU_STATE);
	help_page = 0;
}


//
// InitOnce
//

LOCAL_EVENT (InitOnce) (menux_t *self)
{
	self->cursor_solid_count = COUNT_FLUID_NEG1; // No cursor!  So really doesn't matter does it?
}

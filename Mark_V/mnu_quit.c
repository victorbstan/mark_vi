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

// mnu_quit.c

#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_Quit
#define LOCAL_EVENT(NAME)		void Mnu_ ## Quit ## _ ## NAME

#define local_menu				(&menux[LOCAL_MENU_STATE]) // /*static menux_t *menu = &menux[OUR_MENU_STATE]

//=============================================================================
/* QUIT MENU */

int				msgNumber;
menu_state_e	m_quit_prevstate;	// Where we were before "Quit" dialog.
cbool			wasInMenus;


//
// Draw
//

LOCAL_EVENT (Draw) (void)
{
	if (wasInMenus) {
		// This is a bit fuct up, leave it unless you want to rewrite it.
		// This draws the previous menu state in the background.
		sMenu.menu_state = m_quit_prevstate;
		sMenu.recursiveDraw = true;
		M_Draw ();
		sMenu.menu_state = menu_state_Quit;
		Hotspots_Begin (sMenu.menu_state, 0);
	}

	Hotspots_Add (0, 0, 320, 240, 1, hotspottype_screen);

	M_DrawTextBox (0, -2, 38, 23, 0 /* no hotspot */);
	M_PrintWhite (16, 10,  "  Quake version 1.09 by id Software"); // These shouldn't need newlines, this isn't a print operation (there is no cursor involved, you set X, Y)
	M_PrintWhite (16, 26,  "Programming        Art ");
	M_Print (16, 34,  " John Carmack       Adrian Carmack");
	M_Print (16, 42,  " Michael Abrash     Kevin Cloud");
	M_Print (16, 50,  " John Cash          Paul Steed");
	M_Print (16, 58,  " Dave 'Zoid' Kirsch");
	M_PrintWhite (16, 66,  "Design             Biz");
	M_Print (16, 74,  " John Romero        Jay Wilbur");
	M_Print (16, 82,  " Sandy Petersen     Mike Wilson");
	M_Print (16, 90,  " American McGee     Donna Jackson");
	M_Print (16, 98,  " Tim Willits        Todd Hollenshead");
	M_PrintWhite (16, 106, "Support            Projects");
	M_Print (16, 114, " Barrett Alexander  Shawn Green");
	M_PrintWhite (16, 122, "Sound Effects");
	M_Print (16, 130, " Trent Reznor and Nine Inch Nails");
	M_PrintWhite (16, 138, "Quake is a trademark of Id Software,");
	M_PrintWhite (16, 146, "inc., (c)1996 Id Software, inc. All");
	M_PrintWhite (16, 154, "rights reserved. NIN logo is a");
	M_PrintWhite (16, 162, "registered trademark licensed to");
	M_PrintWhite (16, 170, "Nothing Interactive, Inc. All rights");
	M_PrintWhite (16, 178, "reserved. Press y to exit");
}


//
// Key
//

// Since key can be upper or lower case it isn't quite a scancode
LOCAL_EVENT (KeyPress) (key_scancode_e key, int hotspot)
{
	if (isin2 (key, 'Y', 'y')) key = K_ENTER;
	if (isin2 (key, 'N', 'n')) key = K_ESCAPE;
	
	switch (key) { // Y and N turned into ENTER or ESC
	default:				// Nothing
	case_break K_ESCAPE:	if (wasInMenus) Menu_SetDest (m_quit_prevstate);
							else			Key_SetDest (key_game);

	case_break K_ENTER:		Host_Quit();
	} // end switch (key)
}


//
// Enter
//

LOCAL_EVENT (Enter_f) (lparse_t *unused)
{
	if (sMenu.menu_state == menu_state_Quit)
		return;
	wasInMenus = (key_dest == key_menu);
	m_quit_prevstate = sMenu.menu_state;
	Key_SetDest (key_menu); Menu_SetDest (LOCAL_MENU_STATE);

	msgNumber = rand()&7;
}




//
// InitOnce
//

LOCAL_EVENT (InitOnce) (menux_t *self)
{
	self->cursor_solid_count = COUNT_FLUID_NEG1; // No cursor so really doesn't matter does it?
}


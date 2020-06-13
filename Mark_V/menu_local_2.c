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

// menu_local.c

#include "menu_local.h" // Special access

sMenu_t sMenu;

//typedef struct {
//	menu_state_e					mstate;		// Self
//	int								cursormax;
//	int								column1;
//	int								column2;
//	int								colwidth;
//	menu_initonce_fn_t				initonce_fn;
//	menu_enter_fn_t					enter_fn;
//	menu_draw_fn_t					draw_fn;
//	menu_key_fn_t					key_fn;
//	int								cursor;
//	struct _hotspot_menu_item_t_s	*hover;
//	int								hoverx;
//	int								hovery;	
//} menux_t;

menux_t	menux[menu_state_COUNT] = {
							// cursormax col1, col2, nwidth
	{ menu_state_None_0 },
	#define CMD_DEF(NAME, CURSOR_COUNT, COL1, COL2, NWIDTH) \
	{														\
		menu_state_ ## NAME,								\
		CURSOR_COUNT,										\
		COL1,												\
		COL2,												\
		NWIDTH,												\
		STRINGIFY(NAME),									\
		Mnu_ ## NAME ## _InitOnce,							\
		Mnu_ ## NAME ## _Enter_f,							\
		Mnu_ ## NAME ## _Draw,								\
		Mnu_ ## NAME ## _KeyPress,							\
	}, // Ender
	#include "menu_definitions_sheet.h"
};



// Fairly confident this is in all the right places.
void sClear_Hover (void)
{
	{ int n; for (n = 0; n < menu_state_COUNT; n++) {
		menux[n].hover					= NULL;
	}}

	// Kill the focus with the dispatcher.
	if (hotspot_menu_group.focus) {
		hotspot_menu_group.focus		= NULL;
		hotspot_menu_group.focus_idx	= -1;
		hotspot_menu_group.focus_event	= 0;
	}
}


// dir_e
dir_e Mnu_Nav_Dir (sMenu_t *ff, key_scancode_e keypressed, shiftbits_e shiftbits)
{
	dir_e dir = 0;
	
	// Process key to virtual command
	switch (keypressed) {
	default:	return false; // Not processed.
	case_break K_ESCAPE:		dir = dir_escape;
// TAB IS NATIVE HANDLING, LET IT REMAIN SO.
//	case_break K_TAB:			dir = Flag_Check_Bool(shiftbits, shiftbits_shift_1) ? dir_north : dir_south;
	case_break K_UPARROW:		dir = dir_north;
	case_break K_DOWNARROW:		dir = dir_south;
	case_break K_PAGEUP:		dir = dir_north_max;
	case_break K_PAGEDOWN:		dir = dir_south_max;
	}

	//switch (dir) {
	//default:	System_Error ("Unknown direction");
	//case_break dir_north_max:	QX_Form_Focus_Direction (f, -1, +1);
	//case_break dir_north:		if (f->focus) QX_Form_Focus_Direction (f, f->focus->parent.member_idx, -1);
	//case_break dir_south:		if (f->focus) QX_Form_Focus_Direction (f, f->focus->parent.member_idx, +1);
	//case_break dir_south_max:	QX_Form_Focus_Direction (f, -1, -1);
	//case_break dir_out:			if (MenuShared_Leave == NULL) 
	//								System_Error ("No MenuShared_Leave function");
	//							MenuShared_Leave (NULL); // Another way would be itemdata for the form.
	//							
	//}

	return true; // Handled
}

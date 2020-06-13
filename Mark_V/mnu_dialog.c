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

// mnu_dialog.c - No longer blocking

#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_Dialog
#define LOCAL_EVENT(NAME)		void Mnu_ ## Dialog ## _ ## NAME

typedef struct {
	char			message[SYSTEM_STRING_SIZE_1024];
	voidfunc_t		yes_function;		// What to do if YES pressed.	(ENTER triggers as does "Y")
	voidfunc_t		no_function;		// What to do if NO pressed.	(ESC triggers as does "N")
	keydest_e		source_key_dest;	// What was previous keydest
	menu_state_e	source_menu_state;	// What was previous menu state
	cbool			is_alert;			// If it is an informational alert, enter will clear it.  Usually it is not an alert.
} dialog_t;

static dialog_t dialogus;

//=============================================================================
/* DIALOG MENU */

// An alert is just information.  There is no choice to be made.

// Returns rowpixel
static int M_Dialog_Draw_DrawNotifyString_ (void)
{
	const char	*start;
	int			line_num;
	int			j;
	int			x, y;

	Draw_SetCanvas (CANVAS_MENU); //johnfitz

	start = dialogus.message;

	y = M_CANVAS_HEIGHT_200 * 0.35; //johnfitz -- stretched overlays

	do {
	// scan the width of the line
		for (line_num = 0; line_num < 40; line_num++)
			if (start[line_num] == '\n' || !start[line_num])
				break;
		x = (M_CANVAS_WIDTH_320 - line_num * M_CHAR_HEIGHT_8)/2; //johnfitz -- stretched overlays
		for (j = 0; j < line_num; j++, x += M_CHAR_WIDTH_8)
			Draw_Character (x, y, start[j]);

		y += M_CHAR_HEIGHT_8;

		while (*start && *start != '\n')
			start++;

		if (!*start)
			break;
		start++;		// skip the \n
	} while (1);
	return y;
}


typedef enum { ENUM_FORCE_INT_GCC_ (daction)
	daction_none_0		= 0,
	daction_yes_1		= 1,
	daction_no_2		= 2,
} daction_e;


//
// Draw: SCR_UpdateScreen -> M_Draw -> here
//

LOCAL_EVENT (Draw) (void)
{
	int y;
	Sbar_Draw ();
	Draw_FadeScreen ();
	y = M_Dialog_Draw_DrawNotifyString_ ();
	y += M_CHAR_WIDTH_8 * 2;

	if (dialogus.is_alert) {
		// Information only like "You couldn't connect to that server."
		const char *s	= " OK ";
		int slen		= strlen(s);
		int str_width   = slen * M_CHAR_WIDTH_8;
		int center_x	= M_CANVAS_HALF_WIDTH_160 - str_width / 2;

		crect_t rtext = { center_x, y, str_width, M_CHAR_HEIGHT_8};
		crect_t rbox =  { center_x - 4, y - 4, str_width + 4 * 2, M_CHAR_HEIGHT_8 + 4 * 2};
		Draw_Fill		(rbox.left, rbox.top, rbox.width, rbox.height, 16 /*brown ? */, 0.50);
		Hotspots_Add	(rbox.left, rbox.top, rbox.width, rbox.height, 1, hotspottype_button);
		M_Print			(rtext.left, rtext.top, s);
	} else {
		// "Do you really want to start a new game?"  YES NO
		const char *s_yes = " Yes ";	int s_yes_len = strlen(s_yes);	int s_yes_width_px = s_yes_len * M_CHAR_WIDTH_8;
		const char *s_no = " No  ";		int s_no_len = strlen(s_no);	int s_no_width_px = s_no_len * M_CHAR_WIDTH_8;

		int width = (s_yes_width_px + M_CHAR_WIDTH_8) + (s_no_width_px + M_CHAR_WIDTH_8) + M_CHAR_WIDTH_8;
		int center_x = M_CANVAS_HALF_WIDTH_160 - width / 2;
		{
			crect_t rtext1 =	{ center_x, y, s_yes_width_px, M_CHAR_HEIGHT_8};
			crect_t rbox1 =		{ center_x - 4, y - 4, s_yes_width_px + 4 * 2, M_CHAR_HEIGHT_8 + 4 *2};
			Draw_Fill			(rbox1.left, rbox1.top, rbox1.width, rbox1.height, 16 /*brown ? */, 0.50);
			Hotspots_Add		(rbox1.left, rbox1.top, rbox1.width, rbox1.height, 1, hotspottype_button);
			M_Print				(rtext1.left, rtext1.top, s_yes);

			{ // Position of "no box" is dependent on size of yes box.
				crect_t rtext2 =	{ rbox1.left + rbox1.width + 8 + 4, y, s_no_width_px, M_CHAR_HEIGHT_8};
				crect_t rbox2 =		{ rtext2.left - 4, y - 4, s_no_width_px + 4 * 2, M_CHAR_HEIGHT_8 + 4 *2};

				Draw_Fill			(rbox2.left, rbox2.top, rbox2.width, rbox2.height, 16 /*brown ? */, 0.50);
				Hotspots_Add		(rbox2.left, rbox2.top, rbox2.width, rbox2.height, 1, hotspottype_button);
				M_Print				(rtext2.left, rtext2.top, s_no);
			}
		}
	}
}


//
// Key - input.c various -> M_KeyPress -> here
//

// Since key can be upper or lower case it isn't quite a scancode
LOCAL_EVENT (KeyPress) (key_scancode_e key, int hotspot)
{
	int daction = daction_none_0;

	key = toupper (key); // Not a scancode, we get "n" or "N" ...

	     if (hotspot == 0) key = K_ENTER;  // Yes was clicked
	else if (hotspot == 1) key = K_ESCAPE; // No was clicked
	
	switch (key) {
	default:				
	case_break K_ESCAPE:	daction = daction_no_2;
	case_break 'Y':			daction = daction_yes_1;
	case_break 'N':			daction = daction_no_2;
	case_break K_ENTER:		daction = daction_yes_1;
	} // end switch

	if (daction) {
		// Run the no function if there is one.
		if (daction == daction_yes_1 && dialogus.yes_function) dialogus.yes_function ();
		if (daction == daction_no_2  && dialogus.no_function ) dialogus.no_function  ();
		
		// Return to previous location
		Key_SetDest (dialogus.source_key_dest);

		// If previous destination was a menu, set the previous menu state
		if (dialogus.source_key_dest == key_menu) {
			Menu_SetDest (dialogus.source_menu_state); //sMenu.menu_state = menu_state_Demos;
		}
			
		// Return to previous location
		Key_SetDest (dialogus.source_key_dest);

		// If previous destination was a menu, set the previous menu state
		if (dialogus.source_key_dest == key_menu) {
			Menu_SetDest (dialogus.source_menu_state); //sMenu.menu_state = menu_state_Demos;
		}
	} // end if

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
	// Dialog doesn't need to do anything here
	self->cursor_solid_count = COUNT_FLUID_NEG1; // Can change depending on dialog type (alert style vs yes/no style)
}


//////////////////////////////////////////////////////////////////////////////


//
// Dialog callers.
//

static void Mnu_Dialog_Start_ (const char *text, cbool is_alert, voidfunc_t yesfunction, voidfunc_t nofunction)
{
	if (sMenu.menu_state == menu_state_Dialog)
		System_Error ("Recursive Dialog");
	c_strlcpy (dialogus.message, text);
	dialogus.yes_function			= yesfunction;
	dialogus.no_function			= nofunction;
	dialogus.is_alert				= is_alert;
	dialogus.source_key_dest		= key_dest;
	dialogus.source_menu_state		= sMenu.menu_state;

	Mnu_Dialog_Enter_f (NULL);
}


void Mnu_Dialog_Modal_Alert (voidfunc_t yesfunction, const char *fmt, ...)
{
	VA_EXPAND (text, SYSTEM_STRING_SIZE_1024, fmt);
	Mnu_Dialog_Start_ (text, true /* alert */, yesfunction, yesfunction); // Set both to do same thing.  It is an alert
}

void Mnu_Dialog_Modal_YesNo (voidfunc_t yesfunction, voidfunc_t nofunction, const char *fmt, ...)
{
	VA_EXPAND (text, SYSTEM_STRING_SIZE_1024, fmt);
	Mnu_Dialog_Start_ (text, false /* alert */, yesfunction, nofunction);
}


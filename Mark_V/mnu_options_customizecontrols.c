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

// mnu_options_customizecontrols.c

#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_CustomizeControls
#define LOCAL_EVENT(NAME)		void Mnu_CustomizeControls_ ## NAME

#define local_menu				(&menux[LOCAL_MENU_STATE]) // /*static menux_t *menu = &menux[OUR_MENU_STATE]

// Define menu items
#define opt_COUNT ARRAY_COUNT(bindnames) // 19 right?

//=============================================================================
/* KEYS MENU */

const char *bindnames[][2] =
{
	{"+attack", 		"attack"},
	{"+jump", 			"jump"},
	{"+forward", 		"move forward"},
	{"+back", 			"move back"},
	{"+moveleft", 		"move left"},

	{"+moveright", 		"move right"},
	{"+left", 			"turn left"},
	{"+right", 			"turn right"},
	{"+moveup", 		"swim up"},
	{"+movedown", 		"swim down"},

	{"impulse 10", 		"next weapon"},
	{"impulse 12", 		"last weapon"},
	{"+speed", 			"run"},
	{"+lookup", 		"look up"},
	{"+lookdown", 		"look down"},

	{"+mlook", 			"mouse look"},
	{"+klook", 			"keyboard look"},
	{"+strafe",			"sidestep"},
	{"centerview",		"center view"}
};


static void Mnu_Keys_FindKeysForCommand_ (const char *command, int *threekeys)
{
	int		count;
	int		j;
	int		leng;
	const	char *b;
	int		maxxus = Input_Local_Is_Joystick () ? KEYMAP_Q_USABLE_MAX_494 : KEYMAP_Q_JOYLESS_COUNT_266;

	threekeys[0] = threekeys[1] = threekeys[2] = -1;
	leng = strlen(command);
	count = 0;

	for (j = 0; j < maxxus /*KEYMAP_Q_USABLE_MAX_494* /* Mar 13 2018 touch screen*/; j++) {
#ifdef SUPPORTS_KEYBIND_FLUSH
		// What we want to do here is show the current
		// effective key bindings
		// So if a server key bind is in effect, show that.
		b = Key_GetBinding (j);
#else // old way ...
		b = keybindings[j];
#endif // SUPPORTS_KEYBIND_FLUSH
		if (!b)
			continue;
		if (!strcmp (b, command/*, leng*/) ) // Formerly strncmp, caused "impulse 10" to match "impulse 101", etc.
		{
			threekeys[count] = j;
			count++;
			if (count == 3)
				break;
		}
	}
}


static void Mnu_Keys_UnbindCommand_ (const char *command)
{
	int		leng = strlen(command);
	int		j;
	const char *b;

	for (j = 0; j < KEYMAP_Q_USABLE_MAX_494 /* Mar 13 2018 touch screen*/ ; j ++) {
#ifdef SUPPORTS_KEYBIND_FLUSH
// We want the server and user binds unbound
// Key_SetBinding should handle this situation properly in all cases.
		b = Key_GetBinding (j);
#else // old way ...
		b = keybindings[j];
#endif // !SUPPORTS_KEYBIND_FLUSH
		if (!b)
			continue;

		if (!strcmp (b, command /*, leng*/) ) // Formerly strncmp, caused "impulse 10" to match "impulse 101", etc.
			Key_SetBinding (j, "");
	}
}


//
// Draw
//

LOCAL_EVENT (Draw) (void)
{
	int		x, y;
	int		keys[3];
	const char	*name;
	qpic_t	*p;

	p = Draw_CachePic ("gfx/ttl_cstm.lmp");
	M_DrawPicCentered (4, p);

	if (sMenu.keys_bind_grab)  	M_Print (12, 32, "Press a key or button for this action");
	else						M_Print (18, 32, "Enter to change, backspace to clear");

// search for known bindings
	{ int n; for (n = 0; n < local_menu->cursor_solid_count; n ++) {
		y = 48 + M_CHAR_HEIGHT_8 * n;

		Hotspots_Add	(local_menu->column1, y, local_menu->colwidth, 8, 1, hotspottype_button);
		M_Print			(local_menu->column1, y, bindnames[n][1]);
		Mnu_Keys_FindKeysForCommand_ (bindnames[n][0], keys);

		x = 0;
		if (keys[0] == -1) {
			M_Print (140 + x, y, "???");
			continue;
		}

		name = Key_KeynumToString (keys[0], key_local_name);

		
		M_Print (140 + x, y, name); x += strlen(name) * M_CHAR_WIDTH_8;

		if (keys[1] == -1)
			continue;
		else M_Print (140 + x + 8, y, "or"); x += 4 /*chars*/ * M_CHAR_WIDTH_8;

		name = Key_KeynumToString (keys[1], key_local_name);

		M_Print (140 + x, y, name); 	x += strlen(name) * M_CHAR_WIDTH_8;

		if (keys[2] == -1)
			continue;
		else M_Print (140 + x + 8, y, "or"); x += 4 /*chars*/ * M_CHAR_WIDTH_8;

		name = Key_KeynumToString (keys[2], key_local_name);
		M_Print (140 + x, y, name); 	x += strlen(name) * M_CHAR_WIDTH_8;
	}} // end for

	if (sMenu.keys_bind_grab)	M_DrawCharacter (130, 48 + local_menu->cursor * M_CHAR_HEIGHT_8, '=');
	else						M_DrawCharacter (130, 48 + local_menu->cursor * M_CHAR_HEIGHT_8, 12+((int)(realtime*4)&1));
}

//
// Key
//

// Since key can be upper or lower case it isn't quite a scancode
LOCAL_EVENT (KeyPress) (key_scancode_e key, int hotspot)
{
	char	cmd[80];
	int		keys[3];

	if (sMenu.keys_bind_grab) {	// defining a key
		MENU_ROW_CHANGE_HARD_SOUND();
		if (!isin2(key, K_ESCAPE, TILDE_CHAR_CODE_126) ) {
			c_snprintf2 (cmd, "bind " QUOTED_S " " QUOTED_S NEWLINE, Key_KeynumToString (key, key_local_name), bindnames[local_menu->cursor][0]);
			// Change me to not use the command buffer?  Shouldn't be necessary.

			Cbuf_InsertText (cmd);
		}

		sMenu.keys_bind_grab = false; sMenu.keys_bind_grab_off_time = realtime;
		return;
	}

	switch (key) {
	case_break K_ESCAPE:	Mnu_Options_Enter_f (NULL);
	case_break K_UPARROW:	MENU_ROW_CHANGE_HARD_SOUND(); local_menu->cursor--; if (local_menu->cursor < 0) local_menu->cursor = local_menu->cursor_solid_count - 1;
	case_break K_DOWNARROW:	MENU_ROW_CHANGE_HARD_SOUND(); local_menu->cursor++; if (local_menu->cursor >= local_menu->cursor_solid_count) local_menu->cursor = 0;
	case_break K_BACKSPACE:	MENU_ENTER_LOUD_SOUND(); Mnu_Keys_UnbindCommand_ (bindnames[local_menu->cursor][0]); // delete bindings
	case_break K_DELETE:	MENU_ENTER_LOUD_SOUND(); Mnu_Keys_UnbindCommand_ (bindnames[local_menu->cursor][0]); // delete bindings

	case_break K_ENTER:		// go into bind mode
		Mnu_Keys_FindKeysForCommand_ (bindnames[local_menu->cursor][0], keys);
		MENU_ENTER_LOUD_SOUND();
		if (keys[2] != -1)
			Mnu_Keys_UnbindCommand_ (bindnames[local_menu->cursor][0]);
		sMenu.keys_bind_grab = true;

	} // End of switch (key)
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
	DEBUG_COMPILE_TIME_ASSERT (/*hint word*/ opt_COUNT, opt_COUNT == 19);
	self->cursor_solid_count = opt_COUNT; // Immutable
}


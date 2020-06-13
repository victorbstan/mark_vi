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

// mnu_multiplayer_hostgame.c - port / ok button

#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_HostGame
#define LOCAL_EVENT(NAME)		void Mnu_HostGame_ ## NAME

#define local_menu				(&menux[LOCAL_MENU_STATE]) // /*static menux_t *menu = &menux[OUR_MENU_STATE]

// Define menu items
typedef enum { ENUM_FORCE_INT_GCC_ (opt)
	opt_port_0,
	opt_ok_1,	// Goes to the game options screen where you pick a map, coop vs. deathmatch, etc.
	opt_COUNT // Ender
} opt_e;

//=============================================================================
/* HOST GAME */


static int		lanConfig_cursor_table [] = {72, 92};

static int 		lanConfig_port;
static char		lanConfig_portname[6]; // Text box for port

//
// Draw
//

LOCAL_EVENT (Draw) (void)
{
	qpic_t	*p;
	int		basex;
	const char	*protocol = "TCP/IP";

	M_DrawTransPic (16, 4, Draw_CachePic ("gfx/qplaque.lmp"), NO_HOTSPOTS_0, USE_IMAGE_SIZE_NEG1, USE_IMAGE_SIZE_NEG1);
	p = Draw_CachePic ("gfx/p_multi.lmp");
 		// Baker ok, base is determined on a graphics width :( Grrr ...
		// The graphics is "MULTIPLAYER"
		// In standard Quake this graphic is 216 px
		// I'm fine with it want to center the "MULTIPLAYER"
		// But making the left column vary for rest of menu is silly but whatever ...
		basex = (320 - p->width)/2;
	M_DrawPic (basex, 4, p);

	M_Print (basex, 32, "New Game - TCP/IP");
	basex += M_CHAR_WIDTH_8;

	M_Print (basex, 52, "Address:");

	if (ipv4Available && ipv6Available) {
		M_Print (basex + 9 * M_CHAR_WIDTH_8, 52 - 4, my_ipv4_server_address);
		M_Print (basex + 9 * M_CHAR_WIDTH_8, 52 + 4, my_ipv6_address);
	}

	else if (ipv4Available)
		M_Print (basex + 9 * M_CHAR_WIDTH_8, 52, my_ipv4_server_address);
	else if (ipv6Available)
		M_Print (basex + 9 * M_CHAR_WIDTH_8, 52, my_ipv6_address);

	Hotspots_Add (basex - 8, lanConfig_cursor_table[opt_port_0], local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_textbutton);
	M_Print (basex, lanConfig_cursor_table[opt_port_0], "Port");
	M_DrawTextBox (basex + 8 * M_CHAR_WIDTH_8, lanConfig_cursor_table[opt_port_0] - 8, 6, 1, 0 /* no hotspot */); // Since we did it ourselves?
	M_Print (basex+9*8, lanConfig_cursor_table[opt_port_0], lanConfig_portname);

	// Host Game
	M_DrawTextBox (basex, lanConfig_cursor_table[opt_ok_1] - 8, 2, 1, 6 /* border */);
	M_Print (basex + 8, lanConfig_cursor_table[opt_ok_1], "OK");

	M_DrawCharacter (basex - 8, lanConfig_cursor_table [local_menu->cursor], 12+((int)(realtime*4)&1));

	if (local_menu->cursor == opt_port_0) // Editing port
		M_DrawCharacter (basex + 9 * 8 + 8 * strlen(lanConfig_portname), lanConfig_cursor_table [opt_port_0], 10+((int)(realtime*4)&1));


	if (*sMenu.return_reason)
		M_PrintWhite (basex, 148, sMenu.return_reason);
}


//
// Key
//

// Since key can be upper or lower case it isn't quite a scancode
LOCAL_EVENT (KeyPress) (key_scancode_e key, int hotspot)
{
	int		len, port;

	switch (key) {
	default:
		if (out_of_bounds(32, key, 127)) // non-ascii = get out
			break;


		// Allow paste into port
		if (local_menu->cursor == opt_port_0) {
			len = strlen(lanConfig_portname);

			if (isin2(key, 'v', 'V') && Key_Ctrl_Down()) {
				c_strlcpy (lanConfig_portname, Clipboard_Get_Text_Line ());
			}
			else if (len <  (int)sizeof(lanConfig_portname) - 1 /* room for null*/) {
				if (!isdigit(key))
					break;

				lanConfig_portname[len + 0] = key;
				lanConfig_portname[len + 1] = 0;
			}
		}
	case_break K_ESCAPE:						Mnu_MultiPlayer_Enter_f (NULL);
	case_break K_UPARROW:						MENU_ROW_CHANGE_HARD_SOUND (); local_menu->cursor--; if (local_menu->cursor < 0) local_menu->cursor = local_menu->cursor_solid_count - 1;
	case_break K_DOWNARROW:						MENU_ROW_CHANGE_HARD_SOUND (); local_menu->cursor++; if (local_menu->cursor >= local_menu->cursor_solid_count) local_menu->cursor = 0;
	case_break K_BACKSPACE:
		if (local_menu->cursor == opt_port_0) {
			if (strlen(lanConfig_portname))
				lanConfig_portname[strlen(lanConfig_portname)-1] = 0;
		}

	case_break K_ENTER:
		if (local_menu->cursor == opt_port_0) {
			if (local_menu->cursor != opt_port_0)
				local_menu->cursor = opt_port_0; // Let's set the focus if it doesn't have it.

			if (vid.touch_screen_active) {
				Mnu_OnScreenKeyboard_PromptText (
					"Port", NULL, /*buffer to fill*/ lanConfig_portname, /*sizeof buf*/ sizeof(lanConfig_portname),
					LOCAL_MENU_STATE
				);
			}
			return; // !!!!!
		}

		// OPT_OK_1: is only other option so ...
		// So we clicked ok and are heading into server options.
		
		// enable/disable net systems to match desired config
		Cbuf_AddTextLine ("stopdemo"); // Baker:  Ah, cute.
		net_hostport = lanConfig_port;
		Mnu_GameOptions_Enter_f (NULL);										// 1: Ok button
	} // End of switch (key) statement


	port =  atoi(lanConfig_portname);
	lanConfig_port = in_range (0, port, 65535) ? port : DEFAULT_QPORT_26000;

	c_snprintf1 (lanConfig_portname, "%d", lanConfig_port);
}


//
// Enter
//

LOCAL_EVENT (Enter_f) (lparse_t *unused)
{
	Key_SetDest (key_menu); Menu_SetDest (LOCAL_MENU_STATE);
	
	lanConfig_port = DEFAULTnet_hostport;
	c_snprintf1 (lanConfig_portname, "%d", (int)lanConfig_port);

	sMenu.return_onError = false;
	sMenu.return_reason[0] = 0;

	local_menu->cursor = opt_ok_1; // Start the cursor at a convenient "OK" since who would ever change the port.
}


//
// InitOnce
//

LOCAL_EVENT (InitOnce) (menux_t *self)
{
	DEBUG_COMPILE_TIME_ASSERT (opt_COUNT, opt_COUNT == 2); // We expect 2
	self->cursor_solid_count = opt_COUNT;
}


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

// mnu_multiplayer_joingame.c - port/search local/search public/join game at

#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_JoinGame
#define LOCAL_EVENT(NAME)		void Mnu_JoinGame_ ## NAME

#define local_menu				(&menux[LOCAL_MENU_STATE]) // /*static menux_t *menu = &menux[OUR_MENU_STATE]

// Define menu items
typedef enum { ENUM_FORCE_INT_GCC_ (opt)
	opt_port_0,
	opt_search_local_1,
	opt_search_public_2,
	opt_join_game_at_3,
	opt_COUNT // Ender
} opt_e;

//=============================================================================
/* JOIN GAME */


int		lanConfig_cursor_table [] = {72, 92, 100, 132};

static int 	lanConfig_port;
static char	lanConfig_portname[6];  // Text box for port like "26000"
static char	lanConfig_joinname[22]; // Text box for "server" joinname like "quake.myserver.com"

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

	M_Print (basex, 32, "Join Game - TCP/IP");
	basex += M_CHAR_WIDTH_8;

	M_Print (basex, 52, "Address:");

	switch (ipv4Available + ipv6Available * 2) {
	default:						// Nothing?
	case_break 3 /*both*/:			M_Print (basex + 9 * M_CHAR_WIDTH_8, 52 - 4, my_ipv4_server_address); // Print both
									M_Print (basex + 9 * M_CHAR_WIDTH_8, 52 + 4, my_ipv6_address); // Offsetting a little
	case_break 1 /*only ipv4*/:		M_Print (basex + 9 * M_CHAR_WIDTH_8, 52,     my_ipv4_server_address);
	case_break 2 /*only ipv6*/:		M_Print (basex + 9 * M_CHAR_WIDTH_8, 52,     my_ipv6_address);
	}
		
	Hotspots_Add	(basex - 8, lanConfig_cursor_table[opt_port_0], local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_textbutton);
	M_Print			(basex, lanConfig_cursor_table[opt_port_0], "Port");
	M_DrawTextBox	(basex + 8 * M_CHAR_WIDTH_8, lanConfig_cursor_table[opt_port_0] - 8, 6, 1, NO_HOTSPOTS_0); // Since we did it ourselves?
	M_Print			(basex + 9 * M_CHAR_WIDTH_8, lanConfig_cursor_table[opt_port_0], lanConfig_portname);
	
// Join Game
	Hotspots_Add	(basex - 8, lanConfig_cursor_table[opt_search_local_1], local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_button);
	M_Print			(basex, lanConfig_cursor_table[opt_search_local_1], "Search for local games...");

	Hotspots_Add	(basex - 8, lanConfig_cursor_table[opt_search_public_2], local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_button);
	M_Print			(basex, lanConfig_cursor_table[opt_search_public_2], "Search for public games...");
	M_Print			(basex, 108, "Join game at:");

	Hotspots_Add	(basex - 8, lanConfig_cursor_table[opt_join_game_at_3], local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_text);
	M_DrawTextBox	(basex + 8, lanConfig_cursor_table[opt_join_game_at_3] - 8, 22, 1, NO_HOTSPOTS_0);
	M_Print			(basex + 16, lanConfig_cursor_table[opt_join_game_at_3], lanConfig_joinname);

	M_DrawCharacter	(basex - 8, lanConfig_cursor_table [local_menu->cursor], 12+((int)(realtime*4)&1));

	if (local_menu->cursor == opt_port_0) // Editing port
		M_DrawCharacter (basex + 9 * M_CHAR_WIDTH_8 + M_CHAR_WIDTH_8 * strlen(lanConfig_portname), lanConfig_cursor_table [0], 10+((int)(realtime*4)&1));

	if (local_menu->cursor == opt_join_game_at_3) // "Join Game At" editing lan name.
		M_DrawCharacter (basex + 16 + M_CHAR_WIDTH_8 * strlen(lanConfig_joinname), lanConfig_cursor_table [3], 10+((int)(realtime*4)&1));

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

		// Allow paste into join server name
		if (local_menu->cursor == opt_join_game_at_3) {
			len = strlen(lanConfig_joinname);

			if (isin2(key, 'v', 'V') && Key_Ctrl_Down()) {
				c_strlcpy (lanConfig_joinname, Clipboard_Get_Text_Line ());
			}
			else if (len < (int)sizeof(lanConfig_joinname) - 1 /*room for null*/) {
				lanConfig_joinname[len + 0] = key; lanConfig_joinname[len + 1] = 0;
			}
		}
		// Allow paste into port
		if (local_menu->cursor == opt_port_0) {
			len = strlen(lanConfig_portname);

			if (isin2(key, 'v', 'V') && Key_Ctrl_Down()) {
				c_strlcpy (lanConfig_portname, Clipboard_Get_Text_Line ());
				// This is a way for non-digits to get into the field
			}
			else if (len < (int)sizeof(lanConfig_portname) - 1 /* room for null*/ && isdigit(key)) {
				lanConfig_portname[len + 0] = key; // Only accept digits
				lanConfig_portname[len + 1] = 0;
			}
		}
	case_break K_ESCAPE:						Mnu_MultiPlayer_Enter_f (NULL);
	case_break K_UPARROW:						MENU_ROW_CHANGE_HARD_SOUND (); local_menu->cursor--; if (local_menu->cursor < 0) local_menu->cursor = local_menu->cursor_solid_count - 1;
	case_break K_DOWNARROW:						MENU_ROW_CHANGE_HARD_SOUND (); local_menu->cursor++; if (local_menu->cursor >= local_menu->cursor_solid_count) local_menu->cursor = 0;
	case_break K_BACKSPACE:
		if (local_menu->cursor == opt_port_0		 && strlen(lanConfig_portname)) lanConfig_portname[strlen(lanConfig_portname)-1] = 0;
		if (local_menu->cursor == opt_join_game_at_3 && strlen(lanConfig_joinname)) lanConfig_joinname[strlen(lanConfig_joinname)-1] = 0;
		
	case_break K_ENTER:

		// Baker: At this point, all 3 cause a network operation to happen
		// enable/disable net systems to match desired config
		if (local_menu->cursor != opt_port_0) {
			Cbuf_AddTextLine ("stopdemo"); // Baker:  Ah, cute.
			net_hostport = lanConfig_port;
		}

		switch (local_menu->cursor) {
		default:							// Can this even happen?
		case_break opt_port_0:				// Port.  Enter doesn't do anything unless mobile.
											if (vid.touch_screen_active) {
												Mnu_OnScreenKeyboard_PromptText (
													"Port", NULL, lanConfig_portname, sizeof(lanConfig_portname),
													LOCAL_MENU_STATE
												);	
											}
											return;	 // Legacy code says we must get out.

		case_break opt_search_local_1:  	Mnu_Search_Enter_f ((lparse_t *)SLIST_LAN);				// 1: Search for local games
		case_break opt_search_public_2:  	Mnu_Search_Enter_f ((lparse_t *)SLIST_INTERNET);		// 2: Search for public games
		case_break opt_join_game_at_3:		sMenu.return_state = sMenu.menu_state;					// 3: Join Game At
											sMenu.return_onError = true;
											Key_SetDest (key_game);
											Cbuf_AddTextLinef ("connect " QUOTED_S, lanConfig_joinname);
		} // End cursor switch
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
	local_menu->cursor = opt_search_public_2; // Start the cursor at a convenient "Search for public servers"
}

//
// InitOnce
//

LOCAL_EVENT (InitOnce) (menux_t *self)
{
	DEBUG_COMPILE_TIME_ASSERT (opt_COUNT, opt_COUNT == 4); // We expect 4
	self->cursor_solid_count = opt_COUNT; // Immutable
}


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

// mnu_multiplayer_joingame_serverlist.c - COMBINER -- "Search" (waiting for server query to finish) and "Server List"

#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_Search
#define LOCAL_EVENT(NAME)		void Mnu_ ## Search ## _ ## NAME

#define local_menu				(&menux[LOCAL_MENU_STATE]) // /*static menux_t *menu = &menux[OUR_MENU_STATE]


//=============================================================================
/* SEARCH MENU */

cbool			searchComplete = false;
double			searchCompleteTime;
slistScope_e 	searchLastScope = SLIST_LAN;


//
// Draw
//

LOCAL_EVENT (Draw) (void) // Search
{
	int x;
	qpic_t	*p;
	
	p = Draw_CachePic ("gfx/p_multi.lmp");
		M_DrawPic ( (320-p->width)/2, 4, p);
		x = (320/2) - ((12*8)/2) + 4;
		M_DrawTextBox (x - M_CHAR_WIDTH_8, 32, 12, 1, 0 /* no hotspot */); // This isn't a hotspot.
		M_Print (x, 40, "Searching...");

	if(slistInProgress) {
		NET_Poll();
		return;
	}

	if (!searchComplete) {
		searchComplete = true;
		searchCompleteTime = realtime;
	}

	if (hostCacheCount) {
		Mnu_ServerList_Enter_f (NULL);
		return;
	}

	M_PrintWhite ((320/2) - ((22 * M_CHAR_WIDTH_8)/2), 64, va("No %s Quake servers found", (searchLastScope == SLIST_LAN ? "LAN" : (searchLastScope == SLIST_INTERNET) ? "internet" : "")));
	if ((realtime - searchCompleteTime) < 3.0)
		return;

	Mnu_JoinGame_Enter_f (NULL);
}

//
// Key
//

// Since key can be upper or lower case it isn't quite a scancode
LOCAL_EVENT (KeyPress) (key_scancode_e key, int hotspot) // Search
{
	// It is a transition screen while server are queried ...
	// Nothing to do ...

}

//
// Enter
//

LOCAL_EVENT (Enter_f) (lparse_t *unused) // Search
{
	slistScope_e scope = (slistScope_e)unused;
	Key_SetDest  (key_menu); Menu_SetDest (LOCAL_MENU_STATE);
	
	sMenu.entersound = false;		// Don't make noise

	slistSilent = true;
	slistScope = searchLastScope = scope;
	searchComplete = false;
	NET_Slist_f (NULL);
}

//
// InitOnce
//

LOCAL_EVENT (InitOnce) (menux_t *self) // Search
{
	self->cursor_solid_count = COUNT_FLUID_NEG1; // No cursor!!  So it doesn't really matter what we say here.
}


// End of Search section

#undef LOCAL_MENU_STATE
#undef LOCAL_EVENT
#undef local_menu

#define LOCAL_MENU_STATE		menu_state_ServerList
#define LOCAL_EVENT(NAME)		void Mnu_ ## ServerList ## _ ## NAME

#define local_menu				(&menux[LOCAL_MENU_STATE]) // /*static menux_t *menu = &menux[OUR_MENU_STATE]

//=============================================================================
/* SLIST MENU */

int		slist_cursor;
int		slist_first;
cbool 	slist_sorted;


//
// Draw
//

LOCAL_EVENT (Draw) (void) // ServerList
{	
	qpic_t	*p;
	int slist_num_shown;

	if (!slist_sorted) {
		slist_sorted = true;
		NET_SlistSort ();
	}

	slist_num_shown = hostCacheCount;
	if (slist_num_shown > (200-32)/8) // what are these magic number 200 and 32?
		slist_num_shown = (200-32)/8; // Baker: 200 is menu height less 4 rows (32) then div 8 = display rows
	if (slist_first + slist_num_shown - 1 < slist_cursor)
		slist_first = slist_cursor - (slist_num_shown - 1);
	if (slist_first > slist_cursor)
		slist_first = slist_cursor;

	p = Draw_CachePic ("gfx/p_multi.lmp");
	M_DrawPic ( (320-p->width)/2, 4, p);
	{ int n; for (n = 0; n < slist_num_shown; n ++) {
		crect_t r = {16, 32 + 8 * n, 37 * 8, 8};
		hotspot_menu_item_t *hs = Hotspots_Add (RECT_SEND(r), 1, hotspottype_listitem);
		hs->listindex = slist_first + n;

		M_Print (RECT_SEND_LEFTTOP(r), NET_SlistPrintServer (slist_first + n));
	}} // end for
	M_DrawCharacter (0, 32 + (slist_cursor - slist_first)*8, 12+((int)(realtime*4)&1));

	if (*sMenu.return_reason)
		M_PrintWhite (16, 148, sMenu.return_reason);
}

//
// Key
//

// Since key can be upper or lower case it isn't quite a scancode
LOCAL_EVENT (KeyPress) (key_scancode_e key, int hotspot) // ServerList
{
	if (HS_LIST_IS_LISTINDEX(hotspot)) /* listitem emission list index*/
	{
		if (key == K_ENTER) {
			int list_index = HS_LIST_LISTINDEX_DECODE(hotspot); // Reverse it back
			MENU_ENTER_LOUD_SOUND();
			sMenu.return_state = sMenu.menu_state;
			sMenu.return_onError = true;
			slist_sorted = false;
			sMenu.menu_state_reenter = sMenu.menu_state; Key_SetDest (key_game); // ENTRANCE EXIT POINT
			
			Cbuf_AddTextLinef ("connect " QUOTED_S, NET_SlistPrintServerName(list_index) );
		}
		return;
	}

	switch (key) {
	default:
	case_break K_ESCAPE:		Mnu_JoinGame_Enter_f (NULL);
	case_break K_SPACE:			Mnu_Search_Enter_f ((void *)searchLastScope);
	case_break K_UPARROW:		MENU_ROW_CHANGE_HARD_SOUND (); slist_cursor--; if (slist_cursor < 0) slist_cursor = hostCacheCount - 1;
	case_break K_DOWNARROW:		MENU_ROW_CHANGE_HARD_SOUND (); slist_cursor++; if (slist_cursor >= hostCacheCount) slist_cursor = 0;
	
	case_break K_ENTER:
		MENU_ENTER_LOUD_SOUND();
		sMenu.return_state = sMenu.menu_state;
		sMenu.return_onError = true;
		slist_sorted = false;
		sMenu.menu_state_reenter = sMenu.menu_state; Key_SetDest (key_game);
		Cbuf_AddTextLinef ("connect " QUOTED_S, NET_SlistPrintServerName(slist_cursor) );
	

	} // End switch key

}

//
// Enter
//

LOCAL_EVENT (Enter_f) (lparse_t *unused) // ServerList
{
	Key_SetDest	 (key_menu);  Menu_SetDest (LOCAL_MENU_STATE);

	slist_cursor = 0;
	slist_first = 0;
	sMenu.return_onError = false;
	sMenu.return_reason[0] = 0;
	slist_sorted = false;
}


//
// InitOnce
//

LOCAL_EVENT (InitOnce) (menux_t *self)
{
	self->cursor_solid_count = COUNT_FLUID_NEG1; // Can change readily
}


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

// menu_.c

#include "menu_local.h" // Special access




void Menu_SetDest (menu_state_e new_menu_state)
{
	
	Key_SetDest (key_menu);
	sMenu.menu_state = new_menu_state;

	sClear_Hover ();

	// Refresh hotspots.

#ifdef GLQUAKE_RENDERER_SUPPORT
	eglColorMask(0, 0, 0, 0); // Prevent from drawing.  Doesn't stop software renderer though.
#endif // GLQUAKE_RENDERER_SUPPORT
	M_Draw (); sMenu.entersound = false;

#ifdef GLQUAKE_RENDERER_SUPPORT
	eglColorMask(1, 1, 1, 1);
#endif // GLQUAKE_RENDERER_SUPPORT

	// Refresh hover
	{
		int rawx, rawy;
		Input_Local_Mouse_Cursor_GetPos (&rawx, &rawy, true /*convert to window*/);

		menux[sMenu.menu_state].hover = Menu_Hotspot_Refresh_For_Mouse (rawx, rawy, &menux[sMenu.menu_state].hoverx, &menux[sMenu.menu_state].hovery);
	}
	


	sMenu.entersound = true;
}


void M_Exit (void)
{
	sMenu.menu_state = menu_state_None_0;
}

/*
================
M_ToggleMenu_f
================
*/
void M_ToggleMenu_f (lparse_t *unused)
{
	//sMenu.entersound = true;

	if (key_dest == key_menu)
	{
		if (sMenu.menu_state != menu_state_Main && !sMenu.menu_state_reenter)
		{
			Mnu_Main_Enter_f (NULL);
			return;
		}

		Key_SetDest (key_game);
		return;
	}
	if (key_dest == key_console)
	{
		Con_ToggleConsole_f (NULL);
	}
	else
	{
		if (sMenu.menu_state_reenter) {
			Key_SetDest (key_menu);
			Menu_SetDest (sMenu.menu_state_reenter); // sMenu.menu_state = ;
			sMenu.menu_state_reenter = 0;
		} else
			Mnu_Main_Enter_f (NULL);
	}
}


void cursor_dec (menu_state_e menustate)
{
	menux[menustate].cursor --;  
	if (menux[menustate].cursor < 0)
		menux[menustate].cursor = menux[menustate].cursor_solid_count - 1;
}

void cursor_inc (menu_state_e menustate)
{
	menux[menustate].cursor ++;  
	if (menux[menustate].cursor >= menux[menustate].cursor_solid_count)
		menux[menustate].cursor = 0;
}

hotspot_menu_item_t *Menu_Hotspot_Refresh_For_Mouse (int mousex, int mousey, reply int *left, reply int *top)
{	
	float x, y, z;

	Mat4_UnProject_Smart (mousex, mousey,  /* near */ 0, &focus0.menu_modelview, &focus0.menu_projection, focus0.menu_viewport, clheight, &x, &y, &z);
	//__android_log_print(ANDROID_LOG_INFO, CORE_ANDROID_LOG_TAG, "menu in %d %d out %d %d", (int)mousex, (int)mousey, (int)x, (int)y);
	NOT_MISSING_ASSIGN (left, (int) x);
	NOT_MISSING_ASSIGN (top,  (int) y);
	
	return Hotspots_Hit (x, y);
}



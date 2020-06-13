/*
Copyright (C) 2009-2013 Baker

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
// menu_draw_obs.c -- Loads locations from a text file (ProQuake format)

#include "quakedef.h"
#include "menu_local.h"	// Courtesy
#include "menu.h"		// Related

hotspot_menu_group_t hotspot_menu_group;
hotspot_menu_item_t hotspot_menu_item[MAX_HOTSPOTS_512]; //

//void M_Init (void); // Caller

void Hotspots_Init (void)
{
	hotspot_menu_group.rect_offset	= 0; //offsetof(hotspot_menu_item, rect); // Should be zero
	hotspot_menu_group.itemsize		= sizeof(hotspot_menu_item_t);
}

// M_Draw -- Caller?
void Hotspots_Begin (menu_state_e my_mstate, int cursor) // Function is called
{
	hotspot_menu_group.mstate			= my_mstate;
	hotspot_menu_group.is_refreshing	= true; // Not used at this time
	hotspot_menu_group.count			= 0;
//	hotspot_menu_group.cursor			= cursor;   We don't care where the cursor is.
}

hotspot_menu_item_t *Hotspots_Hit (int x, int y)  // Function is called
{
// How to adjust x and y to canvas?.  Probably subtract something and then divide using the modelview.
// Something like glUnProject?
	int n; for (n = 0; n < hotspot_menu_group.count; n ++) {
		hotspot_menu_item_t *cur = &hotspot_menu_item[n];
		// !cur->hidden && !cur->disabled && 
		if (in_range_beyond (cur->rect.left, x, cur->rect.left + cur->rect.width) && in_range_beyond (cur->rect.top, y, cur->rect.top + cur->rect.height))
			return cur;
	}
	return NULL;
}

void Hotspots_Finish (menu_state_e my_mstate) // Function is not called.
{
	hotspot_menu_group.is_refreshing	= false;
}


// I guess we return the last one
hotspot_menu_item_t *Hotspots_Add (int left, int top, int width, int height, int count_, hotspottype_e hotspottype)
{
	hotspot_menu_item_t *ret = NULL;
	int count	= count_ ? count_ : 1;
	int nheight = count_ ? height / count : height;

	int n; for (n = 0; n < count; n ++) {
		int idx = hotspot_menu_group.count;
		ret = &hotspot_menu_item[idx];
		ret->idx			= idx;
		ret->hotspottype	= hotspottype;
		ret->rect.left		= left;
		ret->rect.top		= top;		top += nheight;
		ret->rect.width		= width;
		ret->rect.height	= nheight;
//		Con_PrintLinef ("Hotspot %d = %d, %d %d x %d", hotspot_menu_item[idx].idx, hotspot_menu_item[idx].rect.left, hotspot_menu_item[idx].rect.top, hotspot_menu_item[idx].rect.width, hotspot_menu_item[idx].rect.height);
		hotspot_menu_group.count ++;
	}
	return ret;
}

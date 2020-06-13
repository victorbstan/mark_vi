/*
Copyright (C) 2017 Baker

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
// menu_draw_obs.h

#ifndef __MENU_DRAW_OBS_H__
#define __MENU_DRAW_OBS_H__

void M_Draw (void);
void M_DrawCharacter (int cx, int line, int num);

void M_DrawPic (int x, int y, qpic_t *pic);
void M_DrawPicCentered (int y, qpic_t *pic);

//void M_DrawTransPic (int x, int y, qpic_t *pic);
//void M_DrawCheckbox (int x, int y, int on);

void M_Init (void);
void M_Draw (void);
void M_KeyPress (key_scancode_e key, int hotspot);


int Mnu_Part_DrawVScrollbar (int left, int top, int width, int height, int buttonsize, int thumbsize, int firstrow, int pagerows, int totalrows);
void Mnu_Part_DrawSlider (int x, int top, float _range_clampf, int numticks, cbool is_gamma);

typedef enum { ENUM_FORCE_INT_GCC_ (hotspottype)
	hotspottype_none_0 = 0,
	hotspottype_inert,			// It's a hotspot.  It's entirely inert.  Used to fill to keep indexes static.
	hotspottype_toggle,			// More than 2 choices.		Responds to left and right.
	hotspottype_slider,			// Slider.					Responds to left and right.
	hotspottype_button,			// Execute.					Example: Single Player, Reset Defaults
	hotspottype_button_line,	// Execute.					Namemaker.  Same as button but little extra trim of what is highlighted for clarity.
//  hotspottype_emitkey,		// Emitter.					Fake key emission
	hotspottype_listitem,		// A list item				Demos and levels and serverlist.
	hotspottype_text,			// Text						// ?
	hotspottype_textbutton,		// Text button.  Enter fires.
	hotspottype_screen,			// Help uses this for next.  Remember, it is not acceptable for a K_MOUSE1 with no hotspot to do something.
	hotspottype_vscroll,		// 
	hotspottype_hscroll,		// 
} hotspottype_e;

#define HS_LIST_LISTINDEX_ENCODE(x) (-(10 + x))
#define HS_LIST_LISTINDEX_DECODE(x) (-x - 10)
#define HS_LIST_IS_LISTINDEX(x) (x <= -10)

typedef struct _hotspot_menu_item_t_s {
	crect_t			rect;				// Rectangle
	int				idx;				// Our number (cursor).
	hotspottype_e	hotspottype;

	int				listindex;			// Our list number.
//	key_scancode_e	key_emit;			// Unused
	
	int				possible_first_rows;// If there are 16 list items on a page and 16 items, 1 possible.  At 17, there are 2 possible.
	int				first_row;			// What does each pixel represent?
	
	crect_t			r_track;			// Helper.  Track.height.
	crect_t			r_button1;
	crect_t			r_track1;
	crect_t			r_thumb;			// Thumb top.
	crect_t			r_track2;
	crect_t			r_button2;

	crect_t			r_trackuse;			// The range that the thumb top might be placed. 16 height, 32 track 0 to 16 = 17 places.

	cbool			is_gamma;			// Sigh ... May 7 2018 - But the results are NICE!
} hotspot_menu_item_t;

typedef enum { ENUM_FORCE_INT_GCC_ (focus_event)
	focus_event_none_0 = 0,
	focus_event_thumb_position,
} focus_event_e;

typedef enum { ENUM_FORCE_INT_GCC_ (focus_part)
	focus_part_none,
	focus_part_button1,
	focus_part_track1,
	focus_part_thumb,
	focus_part_track2,
	focus_part_button2,
} focus_part_e;

// Ironically, we don't use the following at all.
typedef struct {
	menu_state_e		mstate;				// Menu idx?
	size_t				selfsize;			// UNUSED
	size_t				rect_offset;		// Wanted offset_t as type but whatever.
	size_t				itemsize;			// UNUSED
	
	cbool				is_refreshing;		// UNUSED  When it change
	int					count;				// USED		hotspot count.
//	int					cursor;				// I think unused.

// Because hotspots are cleared each draw.
	hotspot_menu_item_t	*focus;						// Not enough to identify the item.
	menu_state_e		focus_menu_state;			// Better.
	int					focus_idx;					// Better.  Clear to -1.
	focus_part_e		focus_part;					// Part of the control hit.

// If we are pushing an event .. event vars.
	focus_event_e		focus_event;
	ticktime_t			focus_event_msgtime_ext;	// Throttle?  We can always remove it.
	int					focus_event_thumb_top;		// How do we know if this changed?  We don't need to know.

// Checking vars
	int					focus_down_y;
	int					focus_down_thumb_top;		// thumb top message
	int					focus_down_thumb_offset;	// thumb top message
	//int				focus_down_thumb_height;	// thumb top message
	int					focus_down_track_top;		// thumb top message
	int					focus_down_track_usable;	// thumb top message
	int					focus_down_possibles;		// Possibles

	int					focus_move_y;
	int					focus_move_thumb_offset;			// Ok, if I click the thumb 3 pixels down that's +3.  The thumb is at Y - 3.
	int					focus_move_thumb_top;			
	

} hotspot_menu_group_t; // Some of these have a ton.  Like the name maker.

#define MAX_HOTSPOTS_512 512
extern hotspot_menu_group_t hotspot_menu_group;
extern hotspot_menu_item_t hotspot_menu_item[MAX_HOTSPOTS_512]; //

void Hotspots_Init (void);
void Hotspots_Begin (menu_state_e my_mstate, int cursor);
void Hotspots_Finish (menu_state_e my_mstate);
hotspot_menu_item_t *Hotspots_Add (int left, int top, int width, int height, int count_, hotspottype_e hotspottype);
hotspot_menu_item_t *Hotspots_Hit (int x, int y);



#endif // ! __MENU_DRAW_OBS_H__
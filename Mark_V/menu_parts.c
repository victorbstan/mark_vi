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

// menu_parts.c

#include "menu_local.h" // Special access

// Page count is 1 less than number visible, because 1 item must always be seen.

int Mnu_Part_DrawVScrollbar (int left, int top, int width, int height, int buttonsize, int thumbsize, int firstrow, int pagerows, int totalrows) //, int navcount)
{
	// Baker: If total rows <= 17, a scroll bar is drawn and is active but doesn't do much.
	crect_t					r_whole		= {left, top, width, height};
	hotspot_menu_item_t		*hs			= Hotspots_Add (RECT_SEND(r_whole), 1, hotspottype_vscroll);

	int possible_first_rows = hs->possible_first_rows = totalrows - pagerows + 1; // 17 - 16 (2 possible first rows) + 1
	float pct				= (hs->first_row = firstrow) / (float)(possible_first_rows - 1);
	int trackheight			= height - (buttonsize * 2);
	int thumb_num_pos		= trackheight - thumbsize + 1;	// Number of possible thumb positions
	int thumb_offset		= c_rint (pct * (thumb_num_pos - 1.)); // -1?

	RECT_SET(hs->r_button1,	left,			top,								width,					buttonsize);
	RECT_SET(hs->r_button1,	left,			top,								width,					buttonsize);
	RECT_SET(hs->r_track,   left,			RECT_BOTTOMOF(hs->r_button1),		width,					trackheight);
	RECT_SET(hs->r_thumb,   left,			hs->r_track.top + thumb_offset,		width,					thumbsize);
	RECT_SET(hs->r_trackuse,left,			RECT_BOTTOMOF(hs->r_button1),		width,					hs->r_track.height - thumbsize + 1);
	RECT_SET(hs->r_button2, left,			RECT_BOTTOMOF(hs->r_track),			width,					buttonsize);
	
	RECT_SET(hs->r_track1,  left,			hs->r_track.top,					width,					thumb_offset);
	RECT_SET(hs->r_track2,  left,			RECT_BOTTOMOF(hs->r_thumb),			width,					hs->r_button2.top - RECT_BOTTOMOF(hs->r_thumb));
	
	Draw_Fill (RECT_SEND(hs->r_button1), QUAKE_BROWN_172, 0.25);
	Draw_Fill (RECT_SEND(hs->r_button2), QUAKE_BROWN_172, 0.25);
	Draw_Fill (RECT_SEND(hs->r_thumb),   QUAKE_GRAY_6,	  0.50);	// Drawing the thumb last, we can see if it overlaps when it shouldn't.

	//if (RECT_BOTTOMOF(hs->r_thumb) -1 >= hs->r_button2.top) {
	//	alert ("Trouble thumb bottom %d below or = %d to button2", RECT_BOTTOMOF(hs->r_thumb) -1, hs->r_button2.top);
	//	left = left;
	//}

	return hs->idx;
}

// Return offset.
void Mnu_Part_DrawSlider (int x, int top, float _range_clampf, int numticks, cbool is_gamma)
{
	const int SLIDER_RANGE_10 = 10;
	float range = CLAMP(0, _range_clampf, 1);
	int thumb_offset = (int) ((SLIDER_RANGE_10 - 1) * 8 * range);
	
	int buttonsize = 8;
	int thumbsize = 8;
	int left	= x - 8; // Silly but whatever.
	int width	= (SLIDER_RANGE_10 + 2) * 8; // 12 characters wide
	int height	= 8 ; // 1 character tall

	crect_t		r_whole		= {left, top, width, height};

	hotspot_menu_item_t *hs = Hotspots_Add (RECT_SEND(r_whole), 1, hotspottype_hscroll);

	RECT_SET(hs->r_button1,		left,								top,		buttonsize,							height);
	RECT_SET(hs->r_track,		RECT_RIGHTOF(hs->r_button1),		top,		SLIDER_RANGE_10 * 8,				height);
	RECT_SET(hs->r_thumb,		hs->r_track.left + thumb_offset,	top,		thumbsize,							height);
	RECT_SET(hs->r_trackuse,	RECT_RIGHTOF(hs->r_button1),		top,		hs->r_track.height - thumbsize + 1,	height);
	RECT_SET(hs->r_button2,		RECT_RIGHTOF(hs->r_track),			top,		buttonsize,							height);
	RECT_SET(hs->r_track1,		hs->r_track.left,					top,		thumb_offset,						height);
	RECT_SET(hs->r_track2,		RECT_RIGHTOF(hs->r_thumb),			top,		hs->r_button2.left - RECT_RIGHTOF(hs->r_thumb), height);

	hs->is_gamma = is_gamma;
	hs->possible_first_rows = numticks;

#if 0
	Draw_Fill (RECT_SEND(r_whole),		 QUAKE_BLACK_0,       1);
	Draw_Fill (RECT_SEND(hs->r_button1), QUAKE_BROWN_169, 0.25);
	Draw_Fill (RECT_SEND(hs->r_button2), QUAKE_BROWN_169, 0.25);
	Draw_Fill (RECT_SEND(hs->r_thumb),   QUAKE_GRAY_6,	  0.50);	// Drawing the thumb last, we can see if it overlaps when it shouldn't.
#endif
	//
	//// Draw 128, repeat 129 10 times, then draw 130.  Draw 131 at a range percent.
	{
		int column;
		M_DrawCharacter (x - 8, top, 128);
	 
		for (column = 0; column < SLIDER_RANGE_10; column++)
			M_DrawCharacter (x + column * M_CHAR_WIDTH_8, top, 129);
		M_DrawCharacter (x + SLIDER_RANGE_10 * 8, top, 130);
		M_DrawCharacter (x + ((SLIDER_RANGE_10 - 1) * 8 - 1) * range /* <--- there is the x --> */, top, 131);
	}
}

// This is also used by video.
void Mnu_Part_DrawCheckbox (int x, int y, int on)
{
#if 0
	if (on)
		M_DrawCharacter (x, y, 131);
	else
		M_DrawCharacter (x, y, 129);
#endif
	if (on)
		M_Print (x, y, "on");
	else
		M_Print (x, y, "off");
}


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

#ifndef __MENU_LOCAL_H__
#define __MENU_LOCAL_H__

#include "quakedef.h"
#include "menu.h"			// Courtesy
//#include "menu_ex.h"		// Only select files may access menu_ex.h
#include "menu_draw_obs.h"	// Obsolete

#define M_CHAR_HEIGHT_8			8
#define M_CHAR_WIDTH_8			8
#define M_CANVAS_WIDTH_320		320
#define M_CANVAS_HALF_WIDTH_160	160
#define M_CANVAS_HEIGHT_200		200
#define Q_FAT_ROW_SIZE_20		20		// Fat row size that uses spinning "Q" cursor (main, single player, multiplayer)

#define M_CURSOR(_my_menu_state)		menux[menu_state_ ## _my_menu_state].cursor
#define M_CURSORMAX(_my_menu_state)		menux[menu_state_ ## _my_menu_state].cursormax
#define M_HOTHEIGHT_8 8					// almost always 8

// Everything below this like I hate and time permitting would like to kill or make nice.  Feb 26 2018 

void cursor_inc (menu_state_e menustate);
void cursor_dec (menu_state_e menustate);

void M_DrawTextBox (int x, int y, int width, int lines, int hotspot_emit_border);

#define SERVEROPTIONS_CURSOR_TABLE_COUNT_10 10

void Mnu_Part_DrawCheckbox (int x, int y, int on);

extern char stringbuf[16];
extern int buflen;
#define SETBUF(_x) c_strlcpy (stringbuf, _x); buflen = strlen (stringbuf);


#ifdef GLQUAKE_COLORMAP_TEXTURES
	void M_DrawTransPicTranslate (int x, int y, qpic_t *pic, int top, int bottom); // multiplayer_player_setup
#else
	void M_DrawTransPicTranslate (int x, int y, qpic_t *pic); // multiplayer_player_setup
	void M_BuildTranslationTable(int top, int bottom);
#endif

#define M_CHAR_HEIGHT_8 8
#define M_CHAR_WIDTH_8 8

#define FUHQUAKE_POSITION_1_2X (1.0 + 13.0/64.0) //  1.203125 .. should be a nice and solid binary-friendly number for floating point storage

// Example

#define cursor_wrap_increment_0_count(_var_integer, _increment, _count) \
	(_var_integer) += _increment; if ( (_var_integer) >= (_count)) (_var_integer) = 0; else if ( (_var_integer) < 0) (_var_integer) = ((_count) - 1); // Ender

#define cursor_wrap_set_min_max(_var_integer, _newval, _min, _max) \
	(_var_integer) = (_newval); if ( (_var_integer) > (_max)) (_var_integer) = (_min); else if ( (_var_integer) < (_min)) (_var_integer) = (_max); // Ender

#define MENU_ROW_CHANGE_HARD_SOUND()	S_LocalSound ("misc/menu1.wav") // Soft changed a value sound
#define MENU_ENTER_LOUD_SOUND()			S_LocalSound ("misc/menu2.wav") // Clicking into a menu
#define MENU_TOGGLE_SOFT_SOUND()		S_LocalSound ("misc/menu3.wav") // Move up or down a row

#define MENU_DELIM_COMMA_CHAR	','
void sClear_Hover (void);

#endif // ! __MENU_LOCAL_H__
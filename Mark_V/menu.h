/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2009 John Fitzgibbons and others
Copyright (C) 2009-2014 Baker and others

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

#ifndef __MENU_H__
#define __MENU_H__

#define NO_HOTSPOT_HIT_NEG1 -1 // Global.

typedef enum { ENUM_FORCE_INT_GCC_ (menu_state)
	menu_state_None_0 = 0,
	#define CMD_DEF(NAME, CURSOR_COUNT, COL1, COL2, NWIDTH) menu_state_ ## NAME, // Like: menu_state_Main,
		#include "menu_definitions_sheet.h"
	menu_state_COUNT, // How many.
} menu_state_e;


struct _menux_t_s; // Forward declare ... fixes gcc warning

// We know menu_key_fn_t doesn't take ascii because it accept arrows.
typedef void (*menu_initonce_fn_t)(struct _menux_t_s *self);
typedef void (*menu_enter_fn_t) (lparse_t *unused);
typedef void (*menu_draw_fn_t)(void);
typedef void (*menu_key_fn_t)(key_scancode_e ascii, int hotspot /* or -1 */); // Because has -1 in the enum, even gcc will make an int32

#define COUNT_UNINIT_NEG2 -2
#define COUNT_FLUID_NEG1 -1
typedef struct _menux_t_s {
	menu_state_e					mstate;					// Self
	int								cursor_solid_count;		// COUNT_FLUID_NEG1 if count can change during session (levels, menu)
	int								column1;
	int								column2;
	int								colwidth;
	char							name[MAX_QPATH_64];

	menu_initonce_fn_t				InitOnce_Function;
	menu_enter_fn_t					Enter_Function;
	menu_draw_fn_t					Draw_Function;
	menu_key_fn_t					Key_Function;

	int								cursor;
	struct _hotspot_menu_item_t_s	*hover;
	int								hoverx;
	int								hovery;
	int								cursor_fluid_count;
} menux_t;

extern menux_t menux[menu_state_COUNT]; // Super-global
#define menux_count menu_state_COUNT

#define CMD_DEF(NAME, CURSOR_COUNT, COL1, COL2, NWIDTH) \
	void Mnu_ ## NAME ## _InitOnce (struct _menux_t_s *self);		/*Like: void Mnu_Main_InitOnce (menux_t self) */					\
	void Mnu_ ## NAME ## _Enter_f (lparse_t *unused);				/*Like: void Mnu_Main_Enter_f (lparse_t *unused) */					\
	void Mnu_ ## NAME ## _Draw (void);								/*Like: void Mnu_Main_Draw (void) */								\
	void Mnu_ ## NAME ## _KeyPress (key_scancode_e ascii, int hotspot);	/*Like: void Mnu_Main_Key (key_scancode_e ascii, int hotspot) */	\
	// Ender
	#include "menu_definitions_sheet.h"


void Mnu_Levels_NewGame (void);
void Mnu_Demos_NewGame (void);
void Mnu_OnScreenKeyboard_PromptText (const char *prompt, cvar_t *cvar_to_set, char *buffer_to_fill, int buffer_sizeof, menu_state_e return_dest);

#define QUAKE_BLACK_0		0
#define QUAKE_GRAY_6		6
#define QUAKE_BLACKISH_16	16
//#define QUAKE_COLOR_17	17
#define QUAKE_COLOR_19 		19
#define QUAKE_TAN_125		125
#define QUAKE_YELLOW_111	111
#define QUAKE_BROWN_169		169
#define QUAKE_COLOR_171 	171
#define QUAKE_BROWN_172		172
#define QUAKE_RED_251		251
#define QUAKE_WHITE_254		254

typedef struct {
	menu_state_e	menu_state;
	menu_state_e	menu_state_reenter;
	cbool			entersound;					// play after drawing a frame, so caching won't disrupt the sound
	cbool			recursiveDraw;
	menu_state_e	return_state;				// Extern
	cbool			return_onError;				// Extern
	char			return_reason [32];
	cbool			keys_bind_grab;
	ticktime_t		keys_bind_grab_off_time;	// Disallow the binding of K_MOUSE2 to also cause us problems in customize controls.
} sMenu_t; // This is a super-global.

extern sMenu_t sMenu; // This is a super-global.


void Menu_SetDest (menu_state_e new_menu_state); // Almost exclusive to menu.c, but used a couple other places.

// input.c uses the following
struct _hotspot_menu_item_t_s *Menu_Hotspot_Refresh_For_Mouse (int mousex, int mousey, reply int *left, reply int *top);





//
// menus
//

void M_Init (void);
void M_Draw (void);
void M_KeyPress (key_scancode_e key, int hotspot);
void M_ToggleMenu_f (lparse_t *unused);
void M_Exit (void); // Exiting the menu, sets sMenu.menu_state = menu_state_None_0;

void M_Print (int cx, int cy, const char *str);
void M_PrintWhite (int cx, int cy, const char *str);



void VID_Menu_Init (void);	// Still exists!


typedef struct
{
	const char	*name;
	const char	*description;
} level_t;

extern level_t levels[];
extern const int num_quake_original_levels;
#define NO_HOTSPOTS_0 0
#define USE_IMAGE_SIZE_NEG1 -1


void Mnu_Dialog_Modal_Alert (voidfunc_t yesfunction, const char *fmt, ...) __core_attribute__((__format__(__printf__,2,3)));
void Mnu_Dialog_Modal_YesNo (voidfunc_t yesfunction, voidfunc_t nofunction, const char *fmt, ...) __core_attribute__((__format__(__printf__,3,4)));

void M_DrawTransPic (int x, int y, qpic_t *pic, int count, int _colsize, int _rowsize);
void M_DrawPic (int x, int y, qpic_t *pic);
void M_DrawPicCentered (int y, qpic_t *pic);

extern int normal_singleplayer_menu;
#define QUAKE_MENU_CURSOR_STRING		"\x0D" // 13 in hex.
#define QUAKE_TEXT_CURSOR_STRING		"\x0A" // 11 in hex.

#endif // ! __MENU_H__


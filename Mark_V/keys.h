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
// keys.h

#ifndef __KEYS_H__
#define __KEYS_H__

#ifdef PLATFORM_OSX // Crusty Mac
	#include "keys_mac_old.h"
#endif // Crusty Mac

#ifdef QUAKE_GAME



enum keyname_s {key_local_name, key_export_name};
typedef enum { ENUM_FORCE_INT_GCC_ (keydest) key_game = 1, key_console = 2, key_message = 3, key_menu = 4} keydest_e;
typedef enum { ENUM_FORCE_INT_GCC_ (cursor) cursor_reset, cursor_reset_abs, select_clear, cursor_select, cursor_select_all} cursor_t;
void Key_Console_Cursor_Move(int netchange, cursor_t action);

void Key_Console_Exo_Submit (void);

extern keydest_e	key_dest;


#define KEYMAP_Q_USABLE_MAX_494		494							// Touchscreen fake keys.  Ignore for customize controls. Replaces KEYMAP_COUNT_512 in some functions.
//#define KEYMAP_Q_JOYLESS_MAX_265	K_MOUSEWHEELDOWN /*265*/	// 
#define KEYMAP_Q_JOYLESS_COUNT_266	K_MOUSEWHEELDOWN + 1 /*265*/	// 

#ifdef SUPPORTS_KEYBIND_FLUSH
typedef struct {
	char	*real;		// Real
	char	*server;	// Server
} bind_t;

void Key_Flush_Server_Binds (void);
const char *Key_GetBinding (int keynum);

extern bind_t		keybindings[KEYMAP_COUNT_512];

#else // old way ...

extern char			*keybindings[KEYMAP_COUNT_512];
#endif // SUPPORTS_KEYBIND_FLUSH

extern int			key_repeats[KEYMAP_COUNT_512];
extern int			key_count;			// incremented every key event
extern int			key_lastpress;

extern int			key_linepos;
extern int 			key_sellength;
extern int			key_insert;
extern double		key_blinktime;



void Key_Init (void);
void Key_Release_Keys (cvar_t* var);
void Key_Release_Mouse_Buttons (void);

void Key_Message (int key);

#define K_SHIFT    K_LSHIFT
#define K_CTRL     K_LCTRL
#define K_ALT      K_LALT
#define K_WINDOWS  K_LWIN
//#define K_COMMAND  K_COMMAND // Hmmmm.  What about the Windows key and the Windows content menu key?
//#define K_KP_ENTER	384	// I guess		Sorry charlie, not going to that level of effort right now.  If I recall, cannot be detected on a Mac?  At least not without resorting to something dumb like a Carbon library?
	


#define INSTRUCTION_NONE 0 


#define ASCII_0 	0
#define UNICODE_0	0
#define SCANCODE_0	0
#define NO_WINDOW_NULL NULL
	

// Not fond of the name.
typedef enum { ENUM_FORCE_INT_GCC_(dir)
    dir_none_0,
    dir_home,
    
    dir_west,			// Presumably -1 or 1
    dir_north,
    dir_east,
    dir_south,

    dir_west_page,		// Presumably increment a page
    dir_north_page,
    dir_east_page,
    dir_south_page,

    dir_west_max,		// To max or min
    dir_north_max,
    dir_east_max,
    dir_south_max,
    
    dir_end,

	dir_escape,
	dir_enter,
	dir_MAXNUM,
} dir_e; // Not fond of the name.


typedef enum { ENUM_FORCE_INT_GCC_(textaction)
	__textaction_begin = dir_MAXNUM + 1,
	textaction_emit = __textaction_begin,

	textaction_copy,
	textaction_cut,
	textaction_delete_backwards,
	textaction_delete_forwards,
	textaction_east_word,
	textaction_extend_east,
	textaction_extend_east_max,
	textaction_extend_east_word,
	textaction_extend_west,
	textaction_extend_west_max,
	textaction_extend_west_word,
	textaction_insert,
	textaction_paste,
	textaction_redo,
	textaction_select_all,
	textaction_undo,
	textaction_west_word,

// Extend left?  Extend right?
	textaction_MAXNUM,
	textaction_none_0 = 0,
	textaction_ignore_0 = 0,
} textaction_e;


//#pragma message ("mouseaction_e defined")

//void Key_Mouse_Button (void *ptr, key_scancode_e scancode, mouseaction_e mouseaction, int ascii, int unicode, int shift, cbool havemouse, int rawx, int rawy);

#ifdef PLATFORM_OSX // Crusty Mac
	int Key_Event (int key, cbool down, int special);
#else // Crusty Mac ^^
	void Key_Event_Ex (void *ptr, key_scancode_e scancode, cbool down, int ascii, int unicode, int shift); // Shift is bits
#endif
	





void Key_SetBinding (int keynum, const char *binding);
const char *Key_KeynumToString (int keynum, enum keyname_s nametype);
void Key_WriteBindings (FILE *f);
void Key_SetDest (keydest_e newdest);
void Keys_Flush_ServerBinds (void);

extern cbool chat_team;

extern cbool Key_Shift_Down (void);
extern cbool Key_Alt_Down (void);
extern cbool Key_Ctrl_Down (void);
shiftbits_e Key_ShiftBits (void);

const char *Key_ListExport (void); // Baker

// Meh ...
void Partial_Reset (void);
void Con_Undo_Clear (void);


#endif // QUAKE_GAME


#endif // ! __KEYS_H__


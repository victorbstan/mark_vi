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

// mnu_onscreenkeyboard.c

#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_OnScreenKeyboard
#define LOCAL_EVENT(NAME)		void Mnu_OnScreenKeyboard_ ## NAME
#define LOCAL_FN(NAME)			Mnu_OnScreenKeyboard_ ## NAME

#define local_menu				(&menux[LOCAL_MENU_STATE]) // static menux_t *menu = &menux[OUR_MENU_STATE]

//=============================================================================
/* NAME EDITOR MENU */    // From JoeQuake 0.15 Dev
//=============================================================================


typedef struct {
	int					idx;
	char				name[16]; // Name of key like "ESC" or "x"
	crectrb_t			rb;
	key_scancode_e		keyascii;
} m_scan_t;

static m_scan_t m_scan[100];
static int		m_scan_count = 0;

#define M_MAX_INPUT_24	24 // Which means 23 length
#define M_NUM_LINES_1	1

static const char		*osk_prompt;				// extern, keep it short.  Caveot emptor.
static char				osk_buffer[M_MAX_INPUT_24]; // extern, can't be too big
static int				osk_buffer_size_ok = 0;		// extern
static char				*osk_buffer_to_fill;
static cvar_t			*osk_cvar_to_set;
static menu_state_e		osk_return_dest;

//
// Draw
//

#define HS_OUTLINE_1		3
#define HS_SHADOW			4
#define CHAR_IS_THE_KEY_0	0
#define PADDING_12			12
#define M_LEFT_MARGIN		(320-267)/2 // last measure was 267 width (26 ish)
//#define M_TOP_MARGIN		((200-71)/2 + M_CHAR_HEIGHT_8 * 2) - 32 // last measure was 267 width (65ish)
#define M_TOP_MARGIN		((200-(71+32))/2 + M_CHAR_HEIGHT_8 * 2) - 32 - 16
#define PROMPT_X			M_LEFT_MARGIN
#define PROMPT_Y			M_TOP_MARGIN

LOCAL_EVENT (Draw) (void)
{
	{int n; for (n = 0; n < m_scan_count; n ++) {
		m_scan_t *e = &m_scan[n];
		Hotspots_Add (	/*left*/	e->rb.left   - HS_OUTLINE_1, 
						/*top*/		e->rb.top    - (HS_OUTLINE_1 + 8), 
						/*width*/	e->rb.width  + (HS_OUTLINE_1 * 2), 
						/*height*/	e->rb.height + ((HS_OUTLINE_1 +8) * 2), 
						/*count*/	1, 
						hotspottype_button // hotspottype_button_line
		);

		if (local_menu->cursor == n) {
			Draw_Fill (PRECT_SEND_INSET(&e->rb, -3), QUAKE_RED_251, 1);
		}

		Draw_Fill (PRECT_SEND_INSET(&e->rb, -2),	QUAKE_BLACK_0, 1);
		

		if (1 /*||  strlen(e->name) > 1*/) {
			Draw_Fill (RECT_SEND(e->rb), QUAKE_BLACK_0, 1);
		}

		//Draw_Fill (e->rb.left   - HS_OUTLINE_1,   0, 320, 200, QUAKE_BLACK_0, 0.25);
		M_Print (e->rb.left, e->rb.top, e->name);
	}}

	//Draw_Fill (PROMPT_X, PROMPT_Y, M_CHAR_WIDTH_8 * 40, M_CHAR_HEIGHT_8, QUAKE_BLACK_0, 1);
	{
		int box_x	= PROMPT_X + (strlen (osk_prompt) + 1) * M_CHAR_WIDTH_8;
		int blink_x = box_x + strlen (osk_buffer) * M_CHAR_WIDTH_8;

		M_Print (PROMPT_X, PROMPT_Y, osk_prompt);
		M_DrawTextBox	(box_x - M_CHAR_WIDTH_8, PROMPT_Y - M_CHAR_HEIGHT_8, /*# chars*/ osk_buffer_size_ok, M_NUM_LINES_1, NO_HOTSPOTS_0);
		M_PrintWhite	(box_x, PROMPT_Y, osk_buffer);
		M_DrawCharacter (blink_x, PROMPT_Y, 10 + ((int)(realtime * 4) & 1));
	}
}


//
// Key
//

// Since key can be upper or lower case it isn't quite a scancode
LOCAL_EVENT (KeyPress) (key_scancode_e key, int hotspot)
{
//	VID_Set_Window_Title ("key code: %d hotspot: %d", (int)key, (int)hotspot); // alert causes a continuous msg loop
	// We get a hotspot number.  Ignore the key(?).  This is an on-screen keyboard.  Right?

//	char	osk_buffer[M_MAX_PROMPT_24]; // extern, can't be too big
	if (key == K_ESCAPE) {
		if (osk_return_dest == menu_state_None_0)	Key_SetDest (key_console);
		else										Menu_SetDest (osk_return_dest);

		return; // GET OUT
	}

	else if (hotspot == NO_HOTSPOT_HIT_NEG1) {
		// The hard way.

		switch (key) {
		default:				// Nothing
		case_break K_UPARROW:	cursor_wrap_increment_0_count (local_menu->cursor, -1, m_scan_count);
		case_break K_DOWNARROW:	cursor_wrap_increment_0_count (local_menu->cursor, +1, m_scan_count); // 2 to svs max players
			
		case_break K_ENTER:		hotspot = local_menu->cursor; goto hotspot_pretend;
		} // end switch (key)

		return; // GET OUT
	}

hotspot_pretend:
	if (hotspot != NO_HOTSPOT_HIT_NEG1) {

		int slen = strlen (osk_buffer);
		m_scan_t *e = &m_scan[hotspot];
		switch (e->keyascii) {
		default:				if (in_range (K_SPACE, e->keyascii, 126) /*printables*/) {
									if (slen < (osk_buffer_size_ok - 1))
										osk_buffer[slen] = e->keyascii; // If room then we append.
								}

		case_break K_ESCAPE:	if (osk_return_dest == menu_state_None_0)	Key_SetDest (key_console);
								else										Menu_SetDest (osk_return_dest);
		case_break K_BACKSPACE:	if (slen > 0) 
									osk_buffer[slen - 1] = 0; // Back it off.
		case_break K_ENTER:		// Save it first
								if (osk_cvar_to_set) Cvar_SetQuick (osk_cvar_to_set, osk_buffer);
								else				 strlcpy (osk_buffer_to_fill, osk_buffer, osk_buffer_size_ok);
								
								//menux[osk_return_dest].Enter_Function (NULL); // Don't save.
								if (osk_return_dest == menu_state_None_0)	{
									Key_Console_Exo_Submit (); // Dumb
									Key_SetDest (key_console);
								}
								else										Menu_SetDest (osk_return_dest);
		
		}


	}
}

//
// Enter
//

LOCAL_EVENT (Enter_f) (lparse_t *unused)
{
	Key_SetDest (key_menu); Menu_SetDest (LOCAL_MENU_STATE);
	
}

//
// InitOnce
//


static void Add_Key (const char *s, key_scancode_e _keyascii, modify int *cursor_x, const int *cursor_y)
{
	key_scancode_e keyascii = _keyascii ? _keyascii : s[0];

	m_scan_t *e = &m_scan[m_scan_count];
	e->idx = m_scan_count;

	c_strlcpy (e->name, s);

	RECT_SET (e->rb, (*cursor_x), (*cursor_y), strlen(e->name) * M_CHAR_WIDTH_8, M_CHAR_HEIGHT_8);
	e->rb.right = RECT_RIGHT (e->rb);
	e->rb.bottom = RECT_BOTTOM (e->rb);

	//alert ("Bounds " QUOTED_S " %d %d .. %d %d", e->name, e->rb.left, e->rb.top, e->rb.width, e->rb.height);

	e->keyascii		= keyascii;

	(*cursor_x) += e->rb.width + PADDING_12;

	m_scan_count ++;
}

LOCAL_EVENT (InitOnce) (menux_t *self)
{
	int x = 0, y = M_TOP_MARGIN + 24; char sbuf[16];
	self->cursor_solid_count = COUNT_FLUID_NEG1; // Name maker has 2-dimensional cursoring.  Don't even try -- ha!
	
	// ROW 1
	x = M_LEFT_MARGIN;
	Add_Key (" ESC ", K_ESCAPE, &x, &y);

	{const char *s = "1234567890 ._"; int n, slen = strlen(s); for (n = 0; n < slen; n ++) {
		if (s[n] == SPACE_CHAR_32)
			x += M_CHAR_WIDTH_8; // I guess
		else {
			sbuf[0] = s[n]; sbuf[1] = 0;
			Add_Key (sbuf, CHAR_IS_THE_KEY_0, &x, &y);
		}
	}} y += M_CHAR_HEIGHT_8 * 4;
	
	// ROW 2
	x = M_LEFT_MARGIN + 28;
	{const char *s = "qwertyuiop -"; int n, slen = strlen(s); for (n = 0; n < slen; n ++) {
		if (s[n] == SPACE_CHAR_32)
			x += M_CHAR_WIDTH_8; // I guess
		else {
			sbuf[0] = s[n]; sbuf[1] = 0;
			Add_Key (sbuf, CHAR_IS_THE_KEY_0, &x, &y);
		}
	}} y += M_CHAR_HEIGHT_8 * 4;

	// ROW 3
	x = M_LEFT_MARGIN + 36;
	{const char *s = "asdfghjkl ;\""; int n, slen = strlen(s); for (n = 0; n < slen; n ++) {
		if (s[n] == SPACE_CHAR_32)
			x += M_CHAR_WIDTH_8; // I guess
		else {
			sbuf[0] = s[n]; sbuf[1] = 0;
			Add_Key (sbuf, CHAR_IS_THE_KEY_0, &x, &y);
		}
	}} y += M_CHAR_HEIGHT_8 * 4;

	// ROW 4
	x = M_LEFT_MARGIN + 36 + 8;
	{const char *s = "zxcvbnm"; int n, slen = strlen(s); for (n = 0; n < slen; n ++) {
		if (s[n] == SPACE_CHAR_32)
			x += M_CHAR_WIDTH_8; // I guess
		else {
			sbuf[0] = s[n]; sbuf[1] = 0;
			Add_Key (sbuf, CHAR_IS_THE_KEY_0, &x, &y);
		}
	}} 
	Add_Key (" BACK ", K_BACKSPACE, &x, &y);
	y += M_CHAR_HEIGHT_8 * 4;

	// ROW 5
	x = M_LEFT_MARGIN + 28 + 24;
	Add_Key ("   SPACE   ", K_SPACE, &x, &y);
	Add_Key (" RETURN ", K_ENTER, &x, &y);

	{ crectrb_t r = { 99999, 99999, 0, 0, 0, 0 };
		{ int n; for (n = 0; n < m_scan_count; n ++) {
			m_scan_t *e = &m_scan[n];
			if (e->rb.left < r.left)		r.left = e->rb.left;
			if (e->rb.top < r.top)			r.top = e->rb.top;
			if (e->rb.right > r.right)		r.right = e->rb.right;
			if (e->rb.bottom > r.bottom)	r.bottom = e->rb.bottom;
		}}
		r.width = RECT_WIDTH (r);
		r.height = RECT_HEIGHT (r);
		r = r;
	}

}


LOCAL_EVENT (PromptText) (const char *prompt, cvar_t *cvar_to_set, char *buffer_to_fill, int buffer_sizeof, menu_state_e return_dest)
{
	osk_prompt			= prompt;
	osk_cvar_to_set		= cvar_to_set;
	osk_buffer_to_fill	= buffer_to_fill;
	osk_buffer_size_ok	= buffer_sizeof;
	osk_return_dest		= return_dest;
	
	memset (osk_buffer, 0, sizeof(osk_buffer));

	if (osk_cvar_to_set)	{ c_strlcpy (osk_buffer, cvar_to_set->string);  } 
	else					{ c_strlcpy (osk_buffer, buffer_to_fill);		} // What we change is our source.
	
	LOCAL_FN (Enter_f) (NULL); // Enter us!
}


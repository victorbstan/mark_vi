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

// mnu_options_video.c

#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_Video
#define LOCAL_EVENT(NAME)		void Mnu_Video_ ## NAME

#define local_menu				(&menux[LOCAL_MENU_STATE]) // /*static menux_t *menu = &menux[OUR_MENU_STATE]

typedef enum { ENUM_FORCE_INT_GCC_ (opt)
	opt_mode_0,
	opt_fullscreen_1,

#ifdef WINQUAKE_RENDERER_SUPPORT
	OPT_STRETCH,
#endif // WINQUAKE_RENDERER_SUPPORT

	opt_test,
	opt_apply,

#ifdef GLQUAKE_RENDERER_SUPPORT
	opt_hwgamma,
	opt_texturefilter,
#endif // WINQUAKE_RENDERER_SUPPORT


	opt_touchscreen, // Arguably PLATFORM_IOS should not show this.


	opt_COUNT // Ender
} opt_e;

//=============================================================================
/* NEW VIDEO MENU -- johnfitz */

extern int glmode_idx; // Used by key and draw

#ifdef WINQUAKE_RENDERER_SUPPORT
	int	video_cursor_table[] = {48, 56, 72, 88, 96, 112};	// mode, fullscreen, stretch, test, apply, tablet mode)
#else
	int	video_cursor_table[] = {48, 56, 72, 80, 112, 120, 136};	// mode, fullscreen, test, apply, gamma, pixels, tablet mode
#endif // !WINQUAKE_RENDERER_SUPPORT





typedef struct {int width,height;} vid_menu_mode;

int vid_menu_rwidth;
int vid_menu_rheight;

//TODO: replace these fixed-length arrays with hunk_allocated buffers

vid_menu_mode vid_menu_modes[MAX_MODE_LIST];
int vid_menu_nummodes = 0;


void VID_Menu_Init (void)
{
	//start n at mode 1 because 0 is windowed mode
	{ int n; for (n = 1; n < vid.nummodes; n ++) {
		int w = vid.modelist[n].width;
		int h = vid.modelist[n].height;
		int j;

		for (j = 0; j < vid_menu_nummodes; j++) {
			if (vid_menu_modes[j].width == w && vid_menu_modes[j].height == h)
				break;
		}

		if (j == vid_menu_nummodes) {
			vid_menu_modes[j].width = w;
			vid_menu_modes[j].height = h;
			vid_menu_nummodes ++;
		}
	}} // end for
}

/*
================
VID_Menu_CalcAspectRatio

calculates aspect ratio for current vid_width/vid_height
================
*/
static void VID_Menu_CalcAspectRatio_ (void)
{
	
	int w = vid_width.value;
	int h = vid_height.value;

	int f = 2;
	while (f < w && f < h) {
		if ((w / f) * f == w && (h / f) * f == h) {
			w /= f;
			h /= f;
			f=2;
		}
		else
			f++;
	}
	vid_menu_rwidth = w;
	vid_menu_rheight = h;
}

/*
================
VID_Menu_ChooseNextMode

chooses next resolution in order, then updates vid_width and
vid_height cvars, then updates bpp and refreshrate lists
================
*/
static void VID_Menu_ChooseNextMode_ (int dir)
{
	int n;

	for (n = 0; n < vid_menu_nummodes; n++) {
		if (vid_menu_modes[n].width == vid_width.value && vid_menu_modes[n].height == vid_height.value)
			break;
	}

	if (n == vid_menu_nummodes) { //can't find it in list, so it must be a custom windowed res
		n = 0;
	}
	else {
		n += dir;
		if (n >= vid_menu_nummodes)
			n = 0;
		else if (n < 0)
			n = vid_menu_nummodes - 1;
	}

	Cvar_SetValueQuick (&vid_width, (float)vid_menu_modes[n].width);
	Cvar_SetValueQuick (&vid_height, (float)vid_menu_modes[n].height);

	VID_Menu_CalcAspectRatio_ ();
}


//
// Draw
//


LOCAL_EVENT (Draw) (void)
{
	int vid_idx = 0;
	qpic_t *p;
	char *title;

	M_DrawTransPic (16, 4, Draw_CachePic ("gfx/qplaque.lmp"), NO_HOTSPOTS_0, USE_IMAGE_SIZE_NEG1, USE_IMAGE_SIZE_NEG1);

	//p = Draw_CachePic ("gfx/vidmodes.lmp");
	p = Draw_CachePic ("gfx/p_option.lmp");
	M_DrawPic ( (320-p->width)/2, 4, p);

	// title
	title = "Video Options";
	M_PrintWhite ((320-8*strlen(title))/2, 32, title);

	// options
	Hotspots_Add (local_menu->column1, video_cursor_table[vid_idx], local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_toggle);
	M_Print (16, video_cursor_table[vid_idx], "        Video mode");
	M_Print (184, video_cursor_table[vid_idx], va("%dx%d (%d:%d)", (int)vid_width.value, (int)vid_height.value, vid_menu_rwidth, vid_menu_rheight));
	vid_idx++;

	Hotspots_Add (local_menu->column1, video_cursor_table[vid_idx], local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_toggle);
	M_Print (16, video_cursor_table[vid_idx], "        Fullscreen");
	Mnu_Part_DrawCheckbox (184, video_cursor_table[vid_idx], (int)vid_fullscreen.value);
	vid_idx++;

#ifdef WINQUAKE_RENDERER_SUPPORT
	Hotspots_Add (local_menu->column1, video_cursor_table[vid_idx], local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_toggle);
	M_Print (16, video_cursor_table[vid_idx], "           Stretch");
	#ifdef CORE_GL
		// WinQuake GL -- keep users out of trouble
		M_Print (184, video_cursor_table[vid_idx], (vid_sw_stretch.value >= 2) ? "320x240 nearest" : (vid_sw_stretch.value >= 1) ? "640x480 nearest" : "None (n/a WinQuake GL)");
	#else
		M_Print (184, video_cursor_table[vid_idx], (vid_sw_stretch.value >= 2) ? "320x240 nearest" : (vid_sw_stretch.value >= 1) ? "640x480 nearest" : "None");
	#endif
	vid_idx++;
#endif // !WINQUAKE_RENDERER_SUPPORT

	Hotspots_Add (local_menu->column1, video_cursor_table[vid_idx], local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_button);
	M_Print (16, video_cursor_table[vid_idx], "      Test changes");
	vid_idx++;

	Hotspots_Add (local_menu->column1, video_cursor_table[vid_idx], local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_button);
	M_Print (16, video_cursor_table[vid_idx], "     Apply changes");
	vid_idx++;

#ifdef GLQUAKE_RENDERER_SUPPORT
	Hotspots_Add (local_menu->column1, video_cursor_table[vid_idx], local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_toggle);
	M_Print (16, video_cursor_table[vid_idx], "        Brightness");
	M_Print (184, video_cursor_table[vid_idx], (vid_hardwaregamma.value) ? "Hardware gamma" : vid.direct3d == 9 ? "Shader gamma" : "Texture gamma");
	vid_idx++;

	Hotspots_Add (local_menu->column1, video_cursor_table[vid_idx], local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_toggle);
	M_Print (16, video_cursor_table[vid_idx], "        Pixelation");
	M_Print (184, video_cursor_table[vid_idx],	glmode_idx == TEXMODE_GL_LINEAR_MIPMAP_LINEAR_5 ? "Smooth (Default)" :
						glmode_idx == TEXMODE_GL_NEAREST_MIPMAP_LINEAR_2 ? "Pixelated" : "Pixelated/Rough" /* TEXMODE_GL_NEAREST_0*/				
		);
	vid_idx++;
	
#endif // !WINQUAKE_RENDERER_SUPPORT

	Hotspots_Add (local_menu->column1, video_cursor_table[vid_idx], local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_toggle);
	M_Print (16, video_cursor_table[vid_idx], "      Touch Screen");
	M_Print (184, video_cursor_table[vid_idx],	vid.touch_screen_active ? "ON" : "OFF");
	vid_idx++;


#ifdef GLQUAKE_RENDERER_SUPPORT
	{
		// Help uses hover
		int help_idx = local_menu->hover ? local_menu->hover->idx : local_menu->cursor;
		if (help_idx == opt_texturefilter) {
			M_PrintWhite (16, video_cursor_table[vid_idx - 1] + 24,  
				glmode_idx == TEXMODE_GL_LINEAR_MIPMAP_LINEAR_5 ?  "     Filter: GL_LINEAR_MIPMAP_LINEAR" : 
				glmode_idx == TEXMODE_GL_NEAREST_MIPMAP_LINEAR_2 ?  "     Filter: GL_NEAREST_MIPMAP_LINEAR" : 
				glmode_idx == TEXMODE_GL_NEAREST_0				  ? "     Filter: GL_NEAREST" : 
																	"     Filter: (other)"
			);
		}
	}
#endif // !WINQUAKE_RENDERER_SUPPORT



	// cursor
	M_DrawCharacter (168, video_cursor_table[local_menu->cursor], 12 + ((int)(realtime*4) & 1));

	// notes          "345678901234567890123456789012345678"
//	M_Print (16, 172, "Windowed modes always use the desk- ");
//	M_Print (16, 180, "top color depth, and can never be   ");
//	M_Print (16, 188, "larger than the desktop resolution. ");
}



//
// Key
//

// Since key can be upper or lower case it isn't quite a scancode
LOCAL_EVENT (KeyPress) (key_scancode_e key, int hotspot)
{
	// For everything except "test" and "apply" ... ENTER might as well be right arrow.  K_MOUSE1 results in "K_ENTER"
	if (key == K_ENTER && !isin2(local_menu->cursor, opt_test, opt_apply)) key = K_RIGHTARROW;

	switch (key) {
	default:												// Nothing?

	case_break K_ESCAPE:								VID_Cvars_Sync_To_Mode (&vid.modelist[vid.modenum_screen]); //sync cvars before leaving menu. FIXME: there are other ways to leave menu
														MENU_ROW_CHANGE_HARD_SOUND();
														Mnu_Options_Enter_f (NULL);

	case_break K_UPARROW:								MENU_ROW_CHANGE_HARD_SOUND (); local_menu->cursor --; if (local_menu->cursor < 0) local_menu->cursor = local_menu->cursor_solid_count - 1;
		

	case_break K_DOWNARROW:								MENU_ROW_CHANGE_HARD_SOUND (); local_menu->cursor ++; if (local_menu->cursor >= local_menu->cursor_solid_count) local_menu->cursor = 0;
	case_break K_LEFTARROW:
		//////////////////////////////////////////////////////////////////////////////////////
		MENU_TOGGLE_SOFT_SOUND();
		switch (local_menu->cursor) {
		default: 											// Nothing
		case_break opt_mode_0:							MENU_TOGGLE_SOFT_SOUND(); VID_Menu_ChooseNextMode_ (-1);
		case_break opt_fullscreen_1:					MENU_TOGGLE_SOFT_SOUND(); Cbuf_AddTextLine ("toggle vid_fullscreen");
		
#ifdef WINQUAKE_RENDERER_SUPPORT
		case_break OPT_STRETCH: {
			int newval = vid_sw_stretch.value <=0 ? 2 : CLAMP(0, (int)vid_sw_stretch.value - 1, 2);
			Cvar_SetValueQuick (&vid_sw_stretch, newval);
		}
#else
		case_break opt_hwgamma:							MENU_TOGGLE_SOFT_SOUND();  Cbuf_AddTextLine ("toggle vid_hardwaregamma");
			
		case_break opt_texturefilter: // 0 (default "5"), 1 - pixelated (default), 2 - 
			MENU_TOGGLE_SOFT_SOUND();
			switch (glmode_idx) { // Left 0, 5, 4
			default:										Cvar_SetQuick (&gl_texturemode, "GL_LINEAR_MIPMAP_LINEAR");
			case_break TEXMODE_GL_LINEAR_MIPMAP_LINEAR_5:	Cvar_SetQuick (&gl_texturemode, "GL_NEAREST");
			case_break TEXMODE_GL_NEAREST_MIPMAP_LINEAR_2:	Cvar_SetQuick (&gl_texturemode, "GL_LINEAR_MIPMAP_LINEAR");
			case_break TEXMODE_GL_NEAREST_0:				Cvar_SetQuick (&gl_texturemode, "GL_NEAREST_MIPMAP_LINEAR");
			}
#endif //WINQUAKE_RENDERER_SUPPORT

		case_break opt_touchscreen:						MENU_TOGGLE_SOFT_SOUND();  Cbuf_AddTextLine ("toggle vid_touchscreen");
		}
		//////////////////////////////////////////////////////////////////////////////////////

	case_break K_RIGHTARROW:
		//////////////////////////////////////////////////////////////////////////////////////
		MENU_TOGGLE_SOFT_SOUND();
		switch (local_menu->cursor) {
		default:											// Nothing
		case_break opt_mode_0:							MENU_TOGGLE_SOFT_SOUND(); VID_Menu_ChooseNextMode_ (1);			
		case_break opt_fullscreen_1:					MENU_TOGGLE_SOFT_SOUND(); Cbuf_AddTextLine ("toggle vid_fullscreen");
#ifdef WINQUAKE_RENDERER_SUPPORT
		case_break OPT_STRETCH: {
			int newval = vid_sw_stretch.value >=2 ? 0 : CLAMP(0, (int)vid_sw_stretch.value + 1, 2);
			Cvar_SetValueQuick (&vid_sw_stretch, newval);
			
		}
#else
		case_break opt_hwgamma:							if (vid.direct3d == 9) MENU_TOGGLE_SOFT_SOUND();  // No noise for others because it studders?
														Cbuf_AddTextLine ("toggle vid_hardwaregamma");
		case_break opt_texturefilter:
			/////////////////////////////////////
			MENU_TOGGLE_SOFT_SOUND();
			switch (glmode_idx) { // Left 0, 5, 1
			default:										Cvar_SetQuick (&gl_texturemode, "GL_LINEAR_MIPMAP_LINEAR");
			case_break TEXMODE_GL_LINEAR_MIPMAP_LINEAR_5:	Cvar_SetQuick (&gl_texturemode, "GL_NEAREST_MIPMAP_LINEAR");
			case_break TEXMODE_GL_NEAREST_MIPMAP_LINEAR_2:	Cvar_SetQuick (&gl_texturemode, "GL_NEAREST");
			case_break TEXMODE_GL_NEAREST_0:				Cvar_SetQuick (&gl_texturemode, "GL_LINEAR_MIPMAP_LINEAR");
			} // end switch (glmode_idx)
			/////////////////////////////////////
#endif //WINQUAKE_RENDERER_SUPPORT
		case_break opt_touchscreen:						MENU_TOGGLE_SOFT_SOUND();  Cbuf_AddTextLine ("toggle vid_touchscreen");
		} // End switch (local_menu->cursor) for 
		//////////////////////////////////////////////////////////////////////////////////////

	case_break K_ENTER:
		//////////////////////////////////////////////////////////////////////////////////////
		sMenu.entersound = true; // Keep?
		switch (local_menu->cursor) {
		default:							// Nothing?
		case_break opt_mode_0:			VID_Menu_ChooseNextMode_ (1);			
		case_break opt_fullscreen_1:	Cbuf_AddTextLine ("toggle vid_fullscreen");
			
#ifdef WINQUAKE_RENDERER_SUPPORT
		case_break OPT_STRETCH:			// Nothing?
#else // GL ...
		case_break opt_hwgamma:			// Nothing?
		case_break opt_texturefilter:	// Nothing?
#endif //WINQUAKE_RENDERER_SUPPORT
		case_break opt_test:			
#ifndef PLATFORM_ANDROID // Cheap but whatever
										Cbuf_AddTextLine ("vid_test");
#endif // !PLATFORM_ANDROID
		case_break opt_apply:			
#ifndef PLATFORM_ANDROID // Cheap but whatever
										Cbuf_AddTextLine ("vid_restart");
#endif // !PLATFORM_ANDROID
		case_break opt_touchscreen:		Cbuf_AddTextLine ("toggle vid_touchscreen");
		} // end switch (local_menu->cursor)
		//////////////////////////////////////////////////////////////////////////////////////
	} // End switch (key)
}


//
// Enter
//

LOCAL_EVENT (Enter_f) (lparse_t *unused)
{
	Key_SetDest (key_menu); Menu_SetDest (LOCAL_MENU_STATE);

	//set all the cvars to match the current mode when entering the menu
	VID_Cvars_Sync_To_Mode (&vid.modelist[vid.modenum_screen]);

	//aspect ratio
	VID_Menu_CalcAspectRatio_ ();
}


//
// InitOnce
//

LOCAL_EVENT (InitOnce) (menux_t *self)
{
	DEBUG_COMPILE_TIME_ASSERT (/*hint word*/ video_cursor_table, opt_COUNT == ARRAY_COUNT(video_cursor_table));
	self->cursor_solid_count = opt_COUNT; // Immutable
}


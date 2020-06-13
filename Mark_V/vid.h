/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2009 John Fitzgibbons and others
Copyright (C) 2007-2008 Kristian Duske
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

#ifndef __VID_H__
#define __VID_H__

// vid.h -- video driver defs

// moved here for global use -- kristian
typedef enum { ENUM_FORCE_INT_GCC_ (MODESTATE) MODESTATE_UNINIT = -1 /* <- unused */, MODESTATE_WINDOWED = 0, MODESTATE_FULLSCREEN = 1 } MODESTATE_e;

#ifdef WINQUAKE_RENDERER_SUPPORT

#define VID_CBITS	6
#define VID_GRADES	(1 << VID_CBITS)
typedef byte pixel_t; // a pixel can be one, two, or four bytes

#define WINQUAKE_MAX_WIDTH_3000  3000	// MAXWIDTH in r_shared.h is 3000.  Does not cascade into other definitions as much as MAXHEIGHT
#define WINQUAKE_MAX_HEIGHT_1080 1080	// Must also change MAXHEIGHT 1080 in r_shared.h, d_ifacea.h.  This affects asm.
#endif // WINQUAKE_RENDERER_SUPPORT

#define QWIDTH_MINIMUM_320 320

#ifdef GLQUAKE
#define QHEIGHT_MINIMUM_2XX 200		// 200, supported mostly for testing
#else
#define QHEIGHT_MINIMUM_2XX 240		// 240, mostly so double size is 640x480 // I guess.
#endif

typedef struct vrect_s
{
	int			x,y,width,height;
	struct vrect_s	*pnext;	// Baker: ASM expects this in struct
} vrect_t;

enum {TEARDOWN_NO_DELETE_GL_CONTEXT_0 = 0, TEARDOWN_FULL_1 = 1};

enum {USER_SETTING_FAVORITE_MODE = 0, ALT_ENTER_TEMPMODE = 1};

#define MAX_MODE_LIST				600
#define MAX_MODE_WIDTH_10000		10000
#define MAX_MODE_HEIGHT_10000		10000
#define MIN_MODE_WIDTH_640			640
#define MIN_MODE_HEIGHT_400			400

#define MIN_WINDOWED_MODE_WIDTH		320
#define MIN_WINDOWED_MODE_HEIGHT	200

typedef struct
{
	MODESTATE_e	type;
#ifdef PLATFORM_OSX
	void*		ptr;	// Baker: I use this for OS X
#endif // PLATFORM_OSX
	int			width;
	int			height;
	int			bpp;
#ifdef SUPPORTS_REFRESHRATE
	int			refreshrate;
#endif // SUPPORTS_REFRESHRATE
} vmode_t;


#define VID_MIN_CONTRAST_1_0 1.0
#define VID_MAX_CONTRAST_2_0 2.0

#define VID_MIN_POSSIBLE_GAMMA_0_5 0.5
#define VID_MAX_POSSIBLE_GAMMA_4_0 4.0

#define VID_MIN_MENU_GAMMA_0_5 0.5
#define VID_MAX_MENU_GAMMA_1_0 1.0


typedef struct mrect_s
{
	int				left, right, bottom, top;
	int				center_x, center_y;
	int				width, height;
} mrect_t;

// Baker: Similar to GL Initial setup after window is constructed, needs to know palette
#ifdef WINQUAKE_GL // !defined(GLQUAKE) && defined(CORE_GL) // WinQuake GL
typedef struct {
	unsigned		rgbapal[PALETTE_COLORS_256];
	unsigned int	texslot;
	int				w, h;
	int				numpels;
	int				width_pow2, height_pow2;
	float			s1, t1;
	unsigned		*rgbabuffer;
	pixel_t			*pixelbytes;	// Link vid.buffer to us.
} wingl_t;
#endif // WINQUAKE_GL // !GLQUAKE but CORE_GL - WinQuake GL

typedef struct
{
	vmode_t			modelist[MAX_MODE_LIST];
	int				nummodes; // The number of them filled in

	vmode_t			screen;

#ifdef WINQUAKE_GL // !defined(GLQUAKE) && defined(CORE_GL) // WinQuake GL
	wingl_t			wingl;
#endif // WINQUAKE_GL -- !GLQUAKE but CORE_GL - WinQuake GL

#ifdef SUPPORTS_RESIZABLE_WINDOW // Windows resize on the fly
	int				border_width;
	int				border_height;
	mrect_t			client_window;
	int				resized; // Baker: resize on fly, if resized this flag gets cleared after resize actions occur.  Set 2 for setmode.
#endif // SUPPORTS_RESIZABLE_WINDOW
	int				modenum_screen;		// mode # on-screen now
	int				modenum_user_selected;	// mode # user intentionally selected (i.e. not an ALT-ENTER toggle)

	int				conwidth;		// Largely used by software renderer, rarely used by GL?
	int				conheight;		// Largely used by software renderer, rarely used by GL?

#ifdef GLQUAKE_RENDERER_SUPPORT // The irony
	int				maxwarpwidth;
	int				maxwarpheight;
#endif // GLQUAKE_RENDERER_SUPPORT

#ifdef WINQUAKE_RENDERER_SUPPORT
// These need to be set when screen changes
	unsigned		rowbytes;		// may be > width if displayed in a window
	float			stretch_x;		// If we scaled for large resolution  WINDOWS StretchBlt usage
	float			stretch_y;		// If we scaled for large resolution  WINDOWS StretchBlt usage
	float			aspect;			// width / height -- < 0 is taller than wide

	pixel_t			*buffer;		// invisible buffer, the main vid.buffer!
	byte			*basepal;		// host_basepal
	pixel_t			*colormap;		// 256 * VID_GRADES size
	byte			altblack;
	byte			alpha50map[PALETTE_COLORS_256 * PALETTE_COLORS_256]; // Best in front and best behind

	byte			*surfcache;
	int				surfcachesize;
//	int				highhunkmark;  extincted by malloc instead

	int				stretch_old_cvar_val;	// The cvar value at time of set.
#endif // WINQUAKE_RENDERER_SUPPORT
#if defined(WINQUAKE_RENDERER_SUPPORT) && defined(PLATFORM_OSX)
	short			*pzbuffer;

	unsigned int	texture;
	unsigned int	texture_actual_width, texture_actual_height; //POW2 upsized
	float			texture_s1, texture_t1;

	unsigned int    rgbapal[PALETTE_COLORS_256];
	unsigned int	*bitmap;

	cbool			texture_initialized;
#endif // WINQUAKE_RENDERER_SUPPORT + PLATFORM_OSX

// Color
#ifdef GLQUAKE_RENDERER_SUPPORT
	unsigned int	d_8to24table[PALETTE_COLORS_256];  // Palette representation in RGB color
	cbool			ever_set_gamma;
#endif // GLQUAKE_RENDERER_SUPPORT

	byte			gammatable[GAMMA_UNITS_256];		// Palette gamma ramp (gamma, contrast)
	
#ifdef WINQUAKE_RENDERER_SUPPORT
	byte			curpal[PALETTE_SIZE_768];	// Palette RGB with gamma ramp
#endif // WINQUAKE_RENDERER_SUPPORT

	int				numpages;
	int				recalc_refdef;		// if true, recalc vid-based stuff

	vmode_t			desktop;
	cbool			canalttab;
	cbool			wassuspended;
	cbool			ActiveApp;
	cbool			Hidden;
	cbool			Minimized;
	cbool			sound_suspended;
	cbool			initialized;
	cbool			system_enhanced_keys; // A bit dumb, but we need this somewhere global and it relates to video directly because initial video setup triggers this.
	cbool			nomouse;

#ifdef GLQUAKE_RENDERER_SUPPORT // Windows resize on the fly

	cbool			warp_stale;
	cbool			consize_stale;

	cbool			scale_dirty; // Happens if vid.conwidth changes, scr_scaleauto changes, scr_menuscale changes, scr_sbarscalechanges, vid_conscale
	float			menu_scale;
	float			sbar_scale;	

#endif  // GLQUAKE_RENDERER_SUPPORT

	int				direct3d;
	int				multisamples;
	
	
// Brave New World
	int				is_mobile;							// Probably prevent from vid_restart?  2 = iPad for UI size
	cbool			is_mobile_ios_keyboard;
	cbool			is_screen_portrait;
	cbool			mobile_keyup;						// IOS uses
	cbool			mobile_bluetooth_keyboard_entry;

	int				touch_screen_game_controls_on;		// Recalc with every Input_Think just like "have mouse"
	cbool			touch_screen_active;
} viddef_t;



extern	viddef_t	vid;				// global video state

extern	int clx, cly, clwidth, clheight;

typedef enum { ENUM_FORCE_INT_GCC_ (tap_type)
	tap_type_none_0,		// Nothing is down.
	tap_type_looking_1,		// First down is always a look.
	tap_type_firing_2,		// A second down that is fast and within small tolerance is a fire.
} tap_type_e;



typedef enum { ENUM_FORCE_INT_GCC_(touch_button)
	touch_button_canvas_0 = 0,		// Missing everything is canvas
	touch_button_forward_left_1,
	touch_button_forward,
	touch_button_forward_right,
	touch_button_left,
	touch_button_back,
	touch_button_right,
	
	touch_button_attack,
	touch_button_jump,
	touch_button_next_weapon,

	touch_button_turnleft,
	touch_button_turnright,

	touch_button_showscores,

	touch_button_COUNT					// More of a maxnum, but whatever ...
} touch_button_e;


#define QKEY_TABLET_FORWARD_LEFT_500	494 + touch_button_forward_left_1 // 495
#define QKEY_TABLET_FORWARD				494 + touch_button_forward // 496 ...
#define QKEY_TABLET_FORWARD_RIGHT		494 + touch_button_forward_right
#define QKEY_TABLET_LEFT				494 + touch_button_left // 498
#define QKEY_TABLET_BACK				494 + touch_button_back
#define QKEY_TABLET_RIGHT				494 + touch_button_right // 500
#define QKEY_TABLET_ATTACK				494 + touch_button_attack
#define QKEY_TABLET_NEXT_WEAPON			494 + touch_button_next_weapon // 502
#define QKEY_TABLET_JUMP				494 + touch_button_jump // 503
#define QKEY_TABLET_TURNLEFT			494 + touch_button_turnleft // 504
#define QKEY_TABLET_TURNRIGHT			494 + touch_button_turnright // 515
#define QKEY_TABLET_SHOWSCORES			494 + touch_button_showscores // 515



typedef struct {
	crect_t r;
	void	*touch_stamp;
} touch_button_t;

typedef struct {
	void			*touch_stamp;
	int				idx;
	cbool			is_dead;
	touch_button_e  touch_button_idx;
} touch_stamp_t;

#define MAX_TOUCH_STAMPS_16 16
typedef struct {
	void			*something_here;				// Translation from game portal to 
	glmatrix		game_projection;				// Projection for the view.
	glmatrix		game_modelview;					// refdef version
//	glmatrix		game_playerview;				// Player view.
// To do chase active we would probably need to a collision point because the angles would be different
// Then we'd need to reverse out the required angles.

	int				game_viewport[4];				// I think this is going to correspond to the physical rect.
	float			r_fovx, r_fovy;					// Do we use?
	float			znear, zfar;
	vec3_t			game_org, game_angles;			// We eventually want double click
	vec3_t			player_org, player_angles;		// We eventually want double click

	int				menu_viewport[4];
	crect_t			touch_sbar;
	glmatrix		menu_projection;
	glmatrix		menu_modelview;

	//
	//void			*focused;
	//int				focus_num;
	
	cbool			*phave_mouse;					// Silly reference to the global.

// Tablet hotspots
	crect_t			escape_box;								// Super hotspot.

	cbool			in_left;								// Do we show "left" button for sliders.
	crect_t			menu_up;								// Super hotspot.
	crect_t			menu_left;								// Super hotspot.
	crect_t			menu_enter;								// Super hotspot.
	crect_t			menu_down;								// Super hotspot.

	crect_t			menu_right;								// Super hotspot.
	crect_t			menu_backsp;							// Super hotspot.

	struct {
		void		*touch_stamp;
		tap_type_e	tap_is_down;
		int			down_x, down_y;
		cbool		is_attack_firing;
		ticktime_t	last_down_time;					// For comparing.  We cannot assume game is 0 for multitouch.
		float		angle_accum;					// Only from mouse movement.
		float		last_delta;
		vec3_t		attack_angles;					// Regardless of what is going on, send these.  One message.
		//int			deltaframe;
	} canvas;
	
	touch_button_t	touch_buttons[touch_button_COUNT];
	int				touch_buttons_num_down;

	touch_stamp_t	touch_stamps[MAX_TOUCH_STAMPS_16];
	int				touch_stamps_num_down;
				// Does this include the canvas?  >>> YES? <<<<
													// What if ESC pressed as third touch?  DENIED.
													// If K_MOUSE1 is down, we ignore all touches.
													// But for ESC we only check the UP!
} focusor_t;
// Need to cease the touches when:  don't have key game.  Or exit key_Game.  Enter intermission.  Start a demo.
// We could do that in M_Draw.


extern focusor_t focus0;

//cmd void VID_Test (void);
//cmd void VID_Restart_f (void);
void VID_Alt_Enter_f (void);


// During run ...
void VID_AppActivate(cbool fActive, cbool minimize, cbool hidden);
void VID_Local_Suspend (cbool bSuspend);
void VID_BeginRendering (int *x, int *y, int *width, int *height);
void VID_EndRendering (void);
void VID_SwapBuffers (void);
void VID_Local_SwapBuffers (void);

// Platform localized video setup ...
vmode_t VID_Local_GetDesktopProperties (void);
void VID_Local_Window_PreSetup (void);

// Main
void VID_Init (void);
void VID_Local_Init (void);
int VID_SetMode (int modenum);
cbool VID_Local_SetMode (int modenum);
void VID_Shutdown (void);
void VID_Local_Shutdown (void);
void VID_Local_Window_Renderer_Teardown (int destroy, cbool reset_video_mode); // Many versions don't care about the resize, but WinQuake does.
//void _VID_Local_Set_Window_Title (const char *text);
void VID_Set_Window_Title (const char *fmt, ...) __core_attribute__((__format__(__printf__,1,2))) ;

// Video modes
cbool VID_Mode_Exists (vmode_t *test, int *outmodenum);
void VID_Local_AddFullscreenModes (void);


// Cvars and modes
vmode_t VID_Cvars_To_Mode (void);
void VID_Cvars_Sync_To_Mode (vmode_t *mymode);
void VID_Cvars_Set_Autoselect_Temp_Fullscreen_Mode (int favoritemode);
void VID_Cvars_Set_Autoselect_Temp_Windowed_Mode (int favoritemode);

#ifdef CORE_GL // Applies to either GLQUAKE or WinQuake GL
void VID_Renderer_Setup (void);
#endif // CORE_GL

#ifdef GLQUAKE_RENDERER_SUPPORT
void VID_Local_Startup_Dialog (void);
void VID_Local_Multisample_f (cvar_t *var);
void VID_BrightenScreen (void); // Non-hardware gamma

// Gamma Table
void VID_Gamma_Init (void);
void VID_Gamma_Think (void);
void VID_Gamma_Shutdown (void);
cbool VID_Local_IsGammaAvailable (unsigned short* ramps);
void VID_Local_Gamma_Set (unsigned short* ramps);
cbool VID_Local_Gamma_Reset (void);
void VID_Gamma_Clock_Set (void); // Initiates a "timer" to ensure gamma is good in fullscreen

void Vid_Gamma_TextureGamma_f (lparse_t *line);
#endif // GLQUAKE_RENDERER_SUPPORT


#ifdef SUPPORTS_RESIZABLE_WINDOW
// Baker: resize on the fly
void VID_Resize_Check (int resize_level); // System messages calls this
void VID_BeginRendering_Resize_Think_Resize_Act (void); // Exclusively called by vid.c but we'll declare it here.
// static void VID_BeginRendering_Resize_Think (void); // Internal to vid.c
// Baker: end resize on the fly
#endif // SUPPORTS_RESIZABLE_WINDOW

#ifdef WINQUAKE_RENDERER_SUPPORT
void VID_Local_SetPalette (byte *palette);
// called after any gamma correction

void VID_ShiftPalette (byte *palette);
// called for bonus and pain flashes, and for underwater color changes

void VID_Update (vrect_t *rects); // Equivalent of swap buffers for WinQuake


void VID_Local_Modify_Palette (byte *palette); // Only MH Windows WinQuake needs this.  On Mac won't do anything.  On WinQuake GL, won't do anything.
cbool VID_CheckGamma (void);  // Equivalent of VID_Gamma_Think
void VID_Palette_NewGame (void); // New game needs to reload palette (a few rare mods use custom palette / colormap)

#endif // WINQUAKE_RENDERER_SUPPORT


// Vsync on Windows doesn't work for software renderer
// But could probably be made to work
void VID_Local_Vsync (void);
void VID_Local_Vsync_f (cvar_t *var);

// Baker: Doesn't apply on a Mac
cbool VID_Local_Vsync_Init (const char *gl_extensions_str);

unsigned *VID_GetBuffer_RGBA_Malloc (int *width, int *height, cbool bgra);



#endif	// ! __VID_H__


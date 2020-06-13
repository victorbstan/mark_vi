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

// mnu_options.c

#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_Options
#define LOCAL_EVENT(NAME)		void Mnu_ ## Options ## _ ## NAME

#define local_menu				(&menux[LOCAL_MENU_STATE]) // /*static menux_t *menu = &menux[OUR_MENU_STATE]

typedef enum { ENUM_FORCE_INT_GCC_ (opt)
	opt_customize_00 = 0,
	opt_goconsole_01,
	opt_reset_02,
	opt_screensize_03,
	opt_contrast_04,
	opt_gamma_05,
	opt_mousespeed_06,
	opt_cdvolume_07,
	opt_soundvolume_08,
	opt_extmusic_09,
	opt_alwaysrun_10,
	opt_usemouse_11,
	opt_invertmouse_12,
	opt_lookspring_13,
	opt_preferences_14,
	opt_videomode_15,
	opt_COUNT
} opt_e;

//=============================================================================
/* OPTIONS MENU */


// Draw type (none, slider, checkbox
// Range  (hi/low)
// Action
typedef void (*qboolfunc_t)(cbool);
typedef enum { ENUM_FORCE_INT_GCC_ (mdraw) mdraw_none_0, mdraw_check, mdraw_slide, mdraw_vmode, mdraw_textbutton} mdraw_t;
typedef enum { ENUM_FORCE_INT_GCC_ (moperation) op_nonzero, op_greaterthan, op_lessthan } moperation_e; // True is determined by what

#if defined(GLQUAKE_RENDERER_SUPPORT) && !defined (DIRECT3D9_WRAPPER) // May 7 2018
double gamma_timer = 0; // Set to realtime + 0.35 for every minor gamma change.
#endif // GLQUAKE + !DIRECT3D9_WRAPPER

typedef struct
{
	opt_e 		optnum;
	const char	*label;
	const char	*description;
	mdraw_t		control_type;
	cvar_t		*cvar_eval;
	float		a, b, c;
	qboolfunc_t	btoggleFunc; // Any mdraw_check that isn't op_nonzero must have this
	cbool		disabled;
	int			row;
} moptions_t;

static void Mnu_Options_AlwaysRun_Toggle_ (cbool is_setting_run)
{
	switch (is_setting_run) {
	default: /*true*/	Cvar_SetValueQuick (&cl_forwardspeed, 400); Cvar_SetValueQuick (&cl_backspeed, 400);
	case_break false:	Cvar_SetValueQuick (&cl_forwardspeed, 200); Cvar_SetValueQuick (&cl_backspeed, 200);
	}
}


static void Mnu_Options_Pitch_Toggle_ (cbool action)
{
	Cvar_SetValueQuick (&m_pitch, -m_pitch.value); // Just reverse the value (0 --> 1 or 1 --> 0)
}

static const char s_externmusic[]	= "cd or mp3 music"; // Baker: Ironically have to have this as char array for below struct data?
static const char s_lookspring[]	= "levels view for keyboarders";

moptions_t menu_options_draw [opt_COUNT] =
{
	// Option #			Label					Description		Control Type		Cvar eval
	{opt_customize_00,	 "Customize controls", "",				mdraw_textbutton					},
	{opt_goconsole_01,	 "     Go to console", "",				mdraw_textbutton					},
	{opt_reset_02,		 " Reset to defaults", "",				mdraw_textbutton					},

	// Option #			Label					Description		Control Type		Cvar eval			ticks   min slide				max slide
	{opt_screensize_03,	 "       Screen size", "",				mdraw_slide,		&scr_viewsize, 		10,		30,						120									},
	{opt_contrast_04,	 "          Contrast", "",				mdraw_slide,		&vid_contrast,		21,		VID_MIN_CONTRAST_1_0,	VID_MAX_CONTRAST_2_0				},
	{opt_gamma_05,		 "             Gamma", "",				mdraw_slide,		&vid_gamma,			21,		VID_MAX_MENU_GAMMA_1_0, VID_MIN_MENU_GAMMA_0_5				},
#ifndef PLATFORM_ANDROID // NOT ANDROID
	{opt_mousespeed_06,	 "       Mouse Speed", "",				mdraw_slide,		&sensitivity, 		21,		1,						21									},
#else
	{opt_mousespeed_06,	 "       Mouse Speed", "",				mdraw_slide,		&sensitivity, 		11,		1,						11									},
#endif
	{opt_cdvolume_07,	 "   CD Music Volume", "",				mdraw_slide,		&bgmvolume, 		11,		0,						3									},
	{opt_soundvolume_08, "      Sound Volume", "",				mdraw_slide,		&sfxvolume, 		21,		0,						1									},

	{opt_extmusic_09,	 "    External Music", s_externmusic,	mdraw_check,		&external_music,	op_nonzero},

	// Option #			Label					Description		Control Type		Cvar eval			eval type		versus  unused	toggle func
	{opt_alwaysrun_10,	 "        Always Run", "",				mdraw_check,		&cl_forwardspeed, 	op_greaterthan, 200,	0,		Mnu_Options_AlwaysRun_Toggle_		},
	{opt_usemouse_11,    "        Mouse Look", "",				mdraw_check,		&in_freelook, 		op_nonzero															},
	{opt_invertmouse_12, "      Invert Mouse", "",				mdraw_check,		&m_pitch, 			op_lessthan,	0,		0,		Mnu_Options_Pitch_Toggle_			},
	{opt_lookspring_13,  "        Lookspring", s_lookspring,	mdraw_check,		&lookspring, 		op_nonzero															},
	{opt_preferences_14,"       Preferences", "",				mdraw_textbutton																							},
	{opt_videomode_15, 	 "     Video Options", "",				mdraw_vmode																									},
};
//int num_menu_options_draw = sizeof(menu_options_draw) / sizeof(menu_options_draw[0]);
//int	options_available = OPTIONS_ITEMS;

static float Mnu_Options_Evaluate_ (const moptions_t *myopt)
{
	if (myopt->control_type == mdraw_check) { // Check box
		int evaltype = (int)myopt->a;
		float eval_number = myopt->b;
		switch (evaltype) {
		default:				System_Error ("Unknown eval type");
		case op_nonzero:		return myopt->cvar_eval->value != 0;
		case op_greaterthan:	return myopt->cvar_eval->value > eval_number;
		case op_lessthan:		return myopt->cvar_eval->value < eval_number;
		}

	}

	if (myopt->control_type == mdraw_slide) {
		float lowbar		= myopt->b; // Slide min
		float highbar		= myopt->c; // Slide max
		float tick_range	= highbar - lowbar;
		float into_range	= (myopt->cvar_eval->value - lowbar) / tick_range; // Probably what pct we (0 to 100)?

		return into_range;
	}

	return 0;
}


static void Mnu_Options_AdjustSliders_ (int this_cursor, int dir, cbool is_silent)
{
	const moptions_t *myopt = &menu_options_draw[this_cursor];
	
	switch (myopt->control_type) {
	default: // Nothing
	case_break mdraw_slide: {
			cbool			slider_positive	= myopt->c > myopt->b;
			float			highval			= slider_positive ? myopt->c : myopt->b;
			float			lowval			= slider_positive ? myopt->b : myopt->c;
			float			unitamount		= (myopt->c - myopt->b) / (myopt->a - 1);
			float			newcvarval		= myopt->cvar_eval->value +  unitamount * dir;

			if (!is_silent) MENU_TOGGLE_SOFT_SOUND();
			newcvarval = CLAMP (lowval, newcvarval, highval);
			Cvar_SetValueQuick (myopt->cvar_eval, newcvarval);
			
#if defined(GLQUAKE_RENDERER_SUPPORT) && !defined (DIRECT3D9_WRAPPER) // May 7 2018
			if (myopt->optnum == opt_gamma_05 && vid_hardwaregamma.value == 0) {
				gamma_timer = realtime + 0.35;
			}
#endif // GLQUAKE + !DIRECT3D9_WRAPPER

			if (local_menu->cursor != this_cursor)		// Baker: Mar 6 2018 - final attempt?
				local_menu->cursor = this_cursor;

	}
	case_break mdraw_check: {
			moperation_e	evaltype		= (int)myopt->a;
			float			eval_number		= myopt->b;
			cbool			was_true;

			if (!is_silent) MENU_TOGGLE_SOFT_SOUND();
			switch (evaltype) {
			default: System_Error ("Unknown eval type");
			case_break op_nonzero:		Cvar_SetValueQuick (myopt->cvar_eval, !myopt->cvar_eval->value);
			case_break op_greaterthan:	was_true = myopt->cvar_eval->value > eval_number;
										myopt->btoggleFunc(!was_true);
					
			case_break op_lessthan:		was_true = myopt->cvar_eval->value < eval_number;
										myopt->btoggleFunc(!was_true);
			}
	}} // End mdraw_check, end switch
}

//void Mnu_Levels_Key (key_scancode_e key, int hotspot);
// Page count is 1 less than number visible, because 1 item must always be seen.


//
// Draw
//

LOCAL_EVENT (Draw) (void)
{
	hotspot_menu_group_t *f = &hotspot_menu_group;
	int row; // col_16 = 16, col_220 = 220, row;
	qpic_t	*p;

// May 7 2018 - We never hide this anymore
#if 0//defined(GLQUAKE_RENDERER_SUPPORT) && !defined (DIRECT3D9_WRAPPER) && !defined(PLATFORM_ANDROID)
	menu_options_draw[opt_gamma_05].disabled = (vid_hardwaregamma.value == 0);
	if (menu_options_draw[local_menu->cursor].disabled) {
		local_menu->cursor++; if (local_menu->cursor >= local_menu->cursor_solid_count) local_menu->cursor = 0;
	}

#endif // GLQUAKE_RENDERER_SUPPORT

	if (f->focus) {
		switch (f->focus_event) {
		default: // impossible

		case_break focus_event_thumb_position:
			// THUMB?
			if (f->focus_event == focus_event_thumb_position) {
				// Units should be 0-20 if there are 21 ticks.
				float pct = f->focus_event_thumb_top / (f->focus_down_track_usable - 1.0);
				int units = c_rint (pct * (f->focus_down_possibles - 1.0)); // Problem getting to 100?
				moptions_t *myopt = &menu_options_draw[f->focus_idx];
				// Need to convert the thump top to a value.  OOF!  Don't put it on a pedalstood.
				float tickamount =  (myopt->c - myopt->b) / (myopt->a - 1);
				float newval = units * tickamount + myopt->b;
				//if ((int)(newval * 100) == (int)(1.05 * 100)) {
				//	newval = newval;
				//}

				//VID_Set_Window_Title ("Value for %s is %g", myopt->cvar_eval->name, newval);

				if (myopt->cvar_eval->value != newval) {
					Cvar_SetValueQuick (myopt->cvar_eval, newval);
					if (!f->focus_event_msgtime_ext || f->focus_event_msgtime_ext < realtime) {
						MENU_TOGGLE_SOFT_SOUND(); // Play a sound otherwise it is weird
						f->focus_event_msgtime_ext = realtime + 0.25; // Not too often.
					}
				}

				if (local_menu->cursor != f->focus_idx)		// Baker: Mar 6 2018 - final attempt?
					local_menu->cursor = f->focus_idx;

				// Clear the message.
				f->focus_event = focus_event_none_0;
			}

		case_break focus_event_none_0:
			if (f->focus_part && f->focus_event_msgtime_ext >= 0 && f->focus_event_msgtime_ext < realtime) {
				float repeat_time = 0;
				cbool is_silent = false; //(f->focus_idx == opt_soundvolume_07) ? false : true; // Sound volume needs to make noise when adjusted
				switch (f->focus_part) {
				default:	goto no_action;// Thumb or something.  Do nothing.
				case_break focus_part_button1: Mnu_Options_AdjustSliders_ (f->focus_idx, -1, is_silent); repeat_time = (1/4.0); // 4 per sec?
				case_break focus_part_button2: Mnu_Options_AdjustSliders_ (f->focus_idx,  1, is_silent); repeat_time = (1/4.0); // 4 per sec?
				case_break focus_part_track1:  Mnu_Options_AdjustSliders_ (f->focus_idx, -1, is_silent); repeat_time = 0;
				case_break focus_part_track2:  Mnu_Options_AdjustSliders_ (f->focus_idx,  1, is_silent); repeat_time = 0;
				}
				f->focus_event_msgtime_ext = repeat_time ? realtime + repeat_time : -1; // -1 = never.

				
no_action:		(0); // Null statement.  A label must have a statement :(
			}
		} // End switch
	} // End if


	M_DrawTransPic (16, 4, Draw_CachePic ("gfx/qplaque.lmp"), NO_HOTSPOTS_0, USE_IMAGE_SIZE_NEG1, USE_IMAGE_SIZE_NEG1);
	p = Draw_CachePic ("gfx/p_option.lmp");
		M_DrawPicCentered (4, p);


	{ int n; for (n = 0, row = 32; n < local_menu->cursor_solid_count; n ++) {
		moptions_t *e = &menu_options_draw[n];
		
		if (e->disabled) {
			// We need to emit insert hotspot.  Ignore crazy possibility that a server could somehow trigger it to disappear
			// While it had mouse focus ;-)
			Hotspots_Add (0, 0, 0, 0, 1, hotspottype_inert);
			e->row = 0;
			continue;
		}

//#define COL_16 16
		M_Print (local_menu->column1, row, e->label);
		switch (e->control_type) {
		default: // Nothing
		case_break mdraw_none_0:		// Hotspots_Add (local_menu->column1, row, local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_button);
		case_break mdraw_textbutton:	Hotspots_Add (local_menu->column1, row, local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_button);
		case_break mdraw_check:			Hotspots_Add (local_menu->column1, row, local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_toggle);
										Mnu_Part_DrawCheckbox (local_menu->column2, row, Mnu_Options_Evaluate_(&menu_options_draw[n]) );

		case_break mdraw_slide:			Mnu_Part_DrawSlider (local_menu->column2, row, Mnu_Options_Evaluate_(&menu_options_draw[n]), e->a, /*is_gamma*/ n == opt_gamma_05);
		case_break mdraw_vmode:			Hotspots_Add (local_menu->column1, row, local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_button);

#ifdef WINQUAKE_RENDERER_SUPPORT
			if (vid.stretch_x > 1) M_Print (local_menu->column2, row, va("%dx%d x%d", vid.screen.width, vid.screen.height, (int)vid.stretch_x) );
			
			else
#endif
									M_Print (local_menu->column2, row, va("%dx%d", vid.screen.width, vid.screen.height) );
		} // End switch
		e->row = row;
		row += M_CHAR_HEIGHT_8;
	}} // End for

	{
		// If we have hover, describe that instead.  Otherwise describe the cursor.
		int help_idx = local_menu->hover ? local_menu->hover->idx : local_menu->cursor;
		M_PrintWhite (M_CANVAS_HALF_WIDTH_160 - strlen(menu_options_draw[help_idx].description)*(M_CHAR_WIDTH_8 / 2), row + 8, menu_options_draw[help_idx].description);
	}

// cursor
	M_DrawCharacter (200, menu_options_draw[local_menu->cursor].row, 12+((int)(realtime*4)&1));

#if defined(GLQUAKE_RENDERER_SUPPORT) && !defined (DIRECT3D9_WRAPPER) // May 7 2018
	if (gamma_timer && gamma_timer < realtime) {
		// Push through a gamma update
		void Vid_Gamma_TextureGamma_f (lparse_t *line);
		lparse_t line = {0};
		line.args[1] = va ("%2.2f", vid_gamma.value);
		line.count = 2;
		Vid_Gamma_TextureGamma_f (&line);
		gamma_timer = 0;
	}
#endif // GLQUAKE + !DIRECT3D9_WRAPPER

	focus0.in_left = in_range (opt_screensize_03, local_menu->cursor, opt_soundvolume_08);
}

static void Mnu_Options_Do_ResetConfig_ (void)
{
	Cbuf_AddTextLine ("resetall"); //johnfitz
	Cbuf_AddTextLine ("exec default.cfg");
}

//
// Key
//

// Since key can be upper or lower case it isn't quite a scancode
LOCAL_EVENT (KeyPress) (key_scancode_e key, int hotspot)
{
	int key_action_cursor = local_menu->cursor;

//	if (menu_options_draw[opt_gamma_05].disabled && key_action_cursor >= opt_gamma_05)
//		key_action_cursor ++;

	if (isin2 (key, K_MOUSEWHEELDOWN, K_MOUSEWHEELUP)) {
		int desc_idx = menux[menu_state_PlayerSetup].hover ? menux[menu_state_PlayerSetup].hover->idx : local_menu->cursor;
	}

	switch (key) {
	default:					// Nothing?
	case_break K_ESCAPE:		Host_WriteConfiguration (); Mnu_Main_Enter_f (NULL); // Baker: Save config here to guarantee time and effort setting up is not lost.
	case_break K_UPARROW:		MENU_ROW_CHANGE_HARD_SOUND(); local_menu->cursor--; if (local_menu->cursor < 0) local_menu->cursor = local_menu->cursor_solid_count - 1;
								// Advance past a disabled item
								if (menu_options_draw[local_menu->cursor].disabled) {
									local_menu->cursor--; if (local_menu->cursor < 0) local_menu->cursor = local_menu->cursor_solid_count - 1;
								}
								
	case_break K_DOWNARROW:		MENU_ROW_CHANGE_HARD_SOUND(); local_menu->cursor++; if (local_menu->cursor >= local_menu->cursor_solid_count) local_menu->cursor = 0;
								// Advance past a disabled item
								if (menu_options_draw[local_menu->cursor].disabled) {
									local_menu->cursor++; if (local_menu->cursor >= local_menu->cursor_solid_count) local_menu->cursor = 0;
								}
								
	case_break K_LEFTARROW:		Mnu_Options_AdjustSliders_ (key_action_cursor, -1, false /* not silent*/);
	case_break K_RIGHTARROW:	Mnu_Options_AdjustSliders_ (key_action_cursor, 1, false /* not silent*/);
	
	case_break K_ENTER:
		switch (key_action_cursor) {
		default:						Mnu_Options_AdjustSliders_ (key_action_cursor, 1, false /* not silent*/);
		case_break opt_customize_00:	Mnu_CustomizeControls_Enter_f (NULL);
		case_break opt_goconsole_01:	Con_ToggleConsole_f (NULL);
		case_break opt_reset_02:		Mnu_Dialog_Modal_YesNo (/* yes function*/ Mnu_Options_Do_ResetConfig_, NULL, "Are you sure you want to reset" NEWLINE "all keys and settings?");
		case_break opt_preferences_14:	Mnu_Preferences_Enter_f (NULL);			
		case_break opt_videomode_15:	Mnu_Video_Enter_f (NULL);
		} // End switch (key_action_cursor)
	} // End switch (key)

}


//
// Enter
//

LOCAL_EVENT (Enter_f) (lparse_t *unused)
{
#if defined(GLQUAKE_RENDERER_SUPPORT) && !defined (DIRECT3D9_WRAPPER) // May 7 2018
	gamma_timer = 0;
#endif // GLQUAKE + !DIRECT3D9_WRAPPER
	Key_SetDest (key_menu); Menu_SetDest (LOCAL_MENU_STATE);
}


//
// InitOnce
//

LOCAL_EVENT (InitOnce) (menux_t *self)
{
	DEBUG_COMPILE_TIME_ASSERT (/*hint word*/ opt_COUNT, opt_COUNT == 16);   // it is 15 because of gamma slider!
	self->cursor_solid_count = opt_COUNT; // Immutable - let's do this and check cursor against disabled attribute
}


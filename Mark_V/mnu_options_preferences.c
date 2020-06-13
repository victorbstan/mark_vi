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

// mnu_options_preferences.c

#include "menu_local.h" // Special access

#define LOCAL_MENU_STATE		menu_state_Preferences
#define LOCAL_EVENT(NAME)		void Mnu_ ## Preferences ## _ ## NAME

#define local_menu				(&menux[LOCAL_MENU_STATE]) // /*static menux_t *menu = &menux[OUR_MENU_STATE]

typedef enum { ENUM_FORCE_INT_GCC_ (opt)
	opt_crosshair_0 = 0,
	opt_weapon_draw_1,
	opt_invisibility_2,
	opt_view_blends_3,
	opt_bobbing_4,
	opt_flashblend_5,
	opt_stain_maps_6,
	opt_start_demos_7,
	opt_server_aim_8,
	opt_draw_clock_9,
	opt_autoscale_10,
	opt_status_bar_11,
	opt_effects_12,
	opt_set_fitz_13,
	opt_set_mark_v_14,
	opt_COUNT
} opt_e;

//=============================================================================
// * PREFERENCES MENU */


typedef enum { ENUM_FORCE_INT_GCC_ (q_control)
	q_control_none_0,
	q_control_labeltoggle,
	q_control_labelbutton,
	q_control_COUNT,
} q_control_e;


typedef struct _mnu_ent_t_s { 
	opt_e			opt_idx;
	q_control_e		q_control; 
	const char		*caption;
	const char		*help_text; 
	cbool			is_gap_after;
	int				eval_idx; // 0 means nothing to eval.  Otherwise subtract 1
	int				fitz_num;
	int				markv_num; 
	const char		*comma_text;
	const char		*winquake_comma_text;

	// --------------------------------------------------------
	int				row_px;
	int				col1_px;
	int				col2_px;
	int				cursor_col;

	int				basket_value;
	int				basket_count;
	char 			basket_text[MAX_QPATH_64]; // I guess
} mnu_ent_t;


#define QCTRL(X) q_control_ ## X
static mnu_ent_t mnu_ent_table[] = { //                                                                           fitz mv                                    
	{ opt_crosshair_0,		QCTRL(labeltoggle), "   Crosshair", "Standard Quake crosshair is a '+'", /*gp*/ 0,  1,  0,  2, "None,Quake Default,Small Dot"										},
	{ opt_weapon_draw_1,	QCTRL(labeltoggle), " Weapon Draw", "FitzQuake position is lower",              0,  2,  1,  1, "Quake Default,FitzQuake (Low),High,Gangsta"							},
	{ opt_invisibility_2,	QCTRL(labeltoggle), "Invisibility", "Default: invisible = no weapon draw",      0,  3,  0,  1, "Quake Default,Draw Weapon"											},
	{ opt_view_blends_3,	QCTRL(labeltoggle), " View Blends", "Screen blend underwater, powerup, ..",     0,  4,  0,  1, "Quake Default,Lite,None"											},
	{ opt_bobbing_4,		QCTRL(labeltoggle), "     Bobbing", "Bobbing: cl_bob, cl_rollangle, ..",        0,  5,  0,  0, "Quake Default,DarkPlaces,None"										},
	{ opt_flashblend_5,		QCTRL(labeltoggle), "  Flashblend", "Moving dynamic light drawing",             0,  6,  1,  0, "Quake Default,GLQuake style", "(GL Only)"							},
	{ opt_stain_maps_6,		QCTRL(labeltoggle), "  Stain Maps", "Darkened stains that gradually fade",      1,  7,  0,  1, "Off,Subtle"															},

	{ opt_start_demos_7,	QCTRL(labeltoggle), "  Startdemos", "Play startup demos on Quake startup",      0,  8,  1,  1, "Do Not Start,Quake Default"							    			},
	{ opt_server_aim_8,		QCTRL(labeltoggle), "  Server Aim", "Lite aim help mostly for keyboarders",     0,  9,  0,  0, "Quake Default,Aim Help Off" 										},
	{ opt_draw_clock_9,		QCTRL(labeltoggle), "  Draw Clock", "Show amount of time into level",           1, 10,  1,  0, "Deathmatch Only,Never,Always" 										},
	
	{ opt_autoscale_10,		QCTRL(labeltoggle), "   Autoscale", "Adjust status bar for resolution",         0, 11,  2,  2, "Forced Off,User Cvar Control,Auto Small,Auto Medium,Auto Large", "(GL Only)"   	},
	{ opt_status_bar_11,	QCTRL(labeltoggle), "  Status Bar", "Only default is uncentered in deathmatch", 0, 12,  0,  3, "Quake Default,Minimal (QW-ish),Translucent (GL),Centered" 			},
	{ opt_effects_12,		QCTRL(labeltoggle), "     Effects", "Original had jerky stairs/monsters",       1, 13,  0,  0, "Normal,JoeQuake (QMB),Normal + Jerky", "Normal,Normal + Jerky"		},

	{ opt_set_fitz_13,		QCTRL(labelbutton), "Set To FitzQuake", "Set FitzQuake 0.85 default settings",	0,  0, 112, -1, ""																	},
	{ opt_set_mark_v_14,	QCTRL(labelbutton), "Set To Mark V", "Set Mark V revised settings",				0,  0, 124, -1, ""																	},
}; const int mnu_ent_table_count = ARRAY_COUNT(mnu_ent_table);


static int Mnu_Preferences_Get_Basket_Value_ (const opt_e option_idx)
{
// We return 0 (catch all default) if no match
    switch (option_idx) {
    default:						System_Error ("unknown idx %d", option_idx);
    case_break opt_crosshair_0:		return CLAMP(0, (int) scr_crosshair.value, 2);                                                                          // Crosshair
    case_break opt_weapon_draw_1:   return Choose321 (r_viewmodel_offset.value, r_viewmodel_size.value >= FUHQUAKE_POSITION_1_2X, !r_viewmodel_quake.value); // Weapon Draw
    case_break opt_invisibility_2:  return CONVBOOL r_viewmodel_ring.value;                                                                                // Invisibility
    case_break opt_view_blends_3:   return Choose12 (v_polyblend_lite.value /*lite*/, !v_polyblend.value /*off*/);                                          // View Blends
    case_break opt_bobbing_4:		return Choose12 (cl_sidebobbing.value, !cl_bob.value);                                                                  // Bobbing
    
    case_break opt_stain_maps_6:	return CONVBOOL r_stains.value;                                                                                         // Stains
    case_break opt_start_demos_7:   return CONVBOOL host_startdemos.value;																					// Start Demos
    case_break opt_server_aim_8:	return iif (sv_aim.value >= 1,     1, 0);                                                                               // Server Aim
    case_break opt_draw_clock_9:	return Choose12(!scr_clock.value /*never*/, scr_clock.value >= 1 /*always*/);                                           // Draw Clock

#ifdef WINQUAKE_RENDERER_SUPPORT
	case_break opt_flashblend_5:	return 0;                                                                       // Flash Blends
    case_break opt_autoscale_10:	return 0;                                                                       // Autoscale
	case_break opt_effects_12:		return !r_lerpmodels.value;
#else
	case_break opt_flashblend_5:	return CONVBOOL gl_flashblend.value;                                                                                    // Flash Blends
	case_break opt_autoscale_10:	return CLAMP(0, (int)scr_scaleauto.value + 1, 5);                                                                       // Autoscale
	case_break opt_effects_12:		return Choose12 (qmb_active.value, !r_lerpmodels.value);
#endif

    case_break opt_status_bar_11:	return Choose123(scr_viewsize.value == 110, scr_sbaralpha.value < 1, scr_sbarcentered.value);                           // Status bar
	
    }
	//Quake Default,Minimal (QW-ish),Translucent (GL),Centered
    
	return -1; // Unreachable due to switch statement default doing a System_Error
}




static cbool Mnu_Preferences_Set_Basket_Value_ (const opt_e option_idx, const int newval)
{
    switch (option_idx) {
	default:						System_Error ("Out of bounds option_idx");
    case_break opt_crosshair_0:		Cvar_SetValueQuick (&scr_crosshair, newval);													// Crosshair
    case_break opt_weapon_draw_1:   Cvar_SetValueQuick (&r_viewmodel_quake,  iif (newval == 1 /*fitz*/, 0, 1));						// Weapon
									Cvar_SetValueQuick (&r_viewmodel_size,   iif (newval >= 2 /*fuh*/, FUHQUAKE_POSITION_1_2X, 0)); 
									Cvar_SetValueQuick (&r_viewmodel_offset, iif (newval == 3 /*side*/, 5.0, 0));
    case_break opt_invisibility_2:  Cvar_SetValueQuick (&r_viewmodel_ring,   iif (newval, 1 /* trans*/, 0) );						// Invisibility
    case_break opt_view_blends_3:   Cvar_SetValueQuick (&v_polyblend,		Choose3 (newval, 1, 1, 0) );															// View Blends (None is no warp either)
									Cvar_SetValueQuick (&v_polyblend_lite,	Choose3 (newval, 0, 1, 0) );
									Cvar_SetValueQuick (&r_waterwarp,		Choose3 (newval, 1, 1, 0) );    
    
	case_break opt_bobbing_4: {	cvar_t *cvar_list[] = { &v_gunkick, &v_kickpitch, &v_kickroll, &v_kicktime, &cl_bob, &cl_bobcycle, &cl_bobup, &cl_rollangle, NULL };
					switch (newval) {
					default:
					case_break 0:	Cvar_ListResetQuick (cvar_list);		Cvar_ResetQuick (&cl_sidebobbing);
					case_break 1:	Cvar_ListResetQuick (cvar_list);		Cvar_SetValueQuick (&cl_sidebobbing, 1);
					case_break 2:	Cvar_ListSetValueQuick (cvar_list, 0);	Cvar_ResetQuick (&cl_sidebobbing);
					}
	}
    case_break opt_flashblend_5:	Cvar_SetValueQuick (&gl_flashblend, newval);													// Flash Blend
    case_break opt_stain_maps_6:	Cvar_SetValueQuick (&r_stains, newval);															// Stain maps
    case_break opt_start_demos_7:	Cvar_SetValueQuick (&host_startdemos, newval);													// Startdemos
	case_break opt_server_aim_8:	Cvar_SetValueQuick (&sv_aim, iif (newval,  2 /*off*/, 0.93 /*quake default */));				// Server Aim
    case_break opt_draw_clock_9:	Cvar_SetValueQuick (&scr_clock, newval - 1);													// Clock
    case_break opt_autoscale_10:	Cvar_SetValueQuick (&scr_scaleauto, newval - 1);												// Autoscale
	case_break opt_status_bar_11:	Cvar_SetValueQuick (&scr_sbaralpha,    Choose4(newval, 1, 0, 0.5, 1)	);						// Status Bar
									Cvar_SetValueQuick (&scr_sbarcentered, Choose4(newval, 0, 1, 0, 1)		);
									Cvar_SetValueQuick (&scr_viewsize,	   iif (newval == 1, 110, 100)		);

									//Quake Default,Minimal (QW-ish),Translucent (GL),Centered
#ifdef WINQUAKE_RENDERER_SUPPORT
    case_break opt_effects_12:		Cvar_SetValueQuick (&v_smoothstairs, iif (newval == 1, 0, 1) /* jerky */);						// Effects
									Cvar_SetValueQuick (&r_lerpmodels,   iif (newval == 1, 0, 1) );
									Cvar_SetValueQuick (&r_lerpmove,     iif (newval == 1, 0, 1) );

#else
    case_break opt_effects_12:		Cvar_SetValueQuick (&v_smoothstairs, iif (newval == 2, 0, 1) /* jerky */);						// Effects
									Cvar_SetValueQuick (&r_lerpmodels,   iif (newval == 2, 0, 1) );
									Cvar_SetValueQuick (&r_lerpmove,     iif (newval == 2, 0, 1) );
									Cvar_SetValueQuick (&qmb_active,     iif (newval == 1, 1, 0) );
#endif 
    } // end switch
	return true;
}



//
// Draw
//

LOCAL_EVENT (Draw) (void)
{
	qpic_t	*p;

	M_DrawTransPic (16, 4, Draw_CachePic ("gfx/qplaque.lmp"), NO_HOTSPOTS_0, USE_IMAGE_SIZE_NEG1, USE_IMAGE_SIZE_NEG1);
	p = Draw_CachePic ("gfx/p_option.lmp");
		M_DrawPicCentered (4, p);

	{ int n; for (n = 0; n < local_menu->cursor_solid_count; n++) {
		mnu_ent_t *e = &mnu_ent_table[n];

		M_Print     (e->col1_px, e->row_px, e->caption);

		switch (e->q_control) {
		default:							// Shouldn't happen
		case_break q_control_labeltoggle:{	const char *s = NULL;
											int s_len; // Would be better as size_t
											const char *our_commadic_text = e->comma_text;
		
#ifdef WINQUAKE_RENDERER_SUPPORT // Override text if winquake only string exists
											if (e->winquake_comma_text)  our_commadic_text = e->winquake_comma_text;
#endif // WINQUAKE_RENDERER_SUPPORT

											e->basket_value = Mnu_Preferences_Get_Basket_Value_(e->opt_idx);
											// if (e->basket_value == IDX_NOT_FOUND_NEG1) alert ("Invalid value"); debug

											s = String_Instance (our_commadic_text, ',', e->basket_value + 1, &s_len);
											strlcpy (e->basket_text, s, s_len + 1);

											e->basket_text[s_len] = 0;

											M_Print		(e->col2_px, e->row_px, e->basket_text);
											
		}
		case_break q_control_labelbutton:	break; // Nothing to do
		}
			
		
		Hotspots_Add (6 * M_CHAR_WIDTH_8, e->row_px, local_menu->colwidth, M_HOTHEIGHT_8, 1, hotspottype_toggle /*hotspottype_button*/);
	}} // end for

	{ // Cursor ignores hotspot hover
		mnu_ent_t *e = &mnu_ent_table[local_menu->cursor]; // Cursor
		M_DrawCharacter (e->cursor_col, e->row_px, 12+((int)(realtime*4)&1));
	}

	{
		// Help text does not
		int help_idx = local_menu->hover ? local_menu->hover->idx : local_menu->cursor;
		mnu_ent_t *e = &mnu_ent_table[help_idx];
		M_PrintWhite (M_CANVAS_HALF_WIDTH_160 - strlen(e->help_text) * (M_CHAR_WIDTH_8 / 2), 23 * M_CHAR_HEIGHT_8, e->help_text);
	}
}


//
// Key
//

// Since key can be upper or lower case it isn't quite a scancode
LOCAL_EVENT (KeyPress) (key_scancode_e key, int hotspot)
{
	int help_idx = local_menu->hover ? local_menu->hover->idx : local_menu->cursor;
	mnu_ent_t *e = &mnu_ent_table[local_menu->cursor];
	
	switch (key) {
	default:				// Nothing
	case_break K_ESCAPE:	Mnu_Options_Enter_f (NULL);
	case_break K_DOWNARROW:	MENU_ROW_CHANGE_HARD_SOUND (); if (++local_menu->cursor >= local_menu->cursor_solid_count) local_menu->cursor = 0;
	case_break K_UPARROW:	MENU_ROW_CHANGE_HARD_SOUND (); if (--local_menu->cursor < 0) local_menu->cursor = local_menu->cursor_solid_count - 1;
	case_break K_LEFTARROW: if (e->q_control == q_control_labeltoggle) {
								
								int newval = e->basket_value - 1;
								if (newval >= e->basket_count) newval = 0; else if (newval < 0) newval = e->basket_count - 1;
								Mnu_Preferences_Set_Basket_Value_ (e->opt_idx, newval);
								MENU_TOGGLE_SOFT_SOUND();
							}
							

	case_break K_RIGHTARROW: goto righty;
		
	case_break K_ENTER:		
		righty:
							if (e->q_control == q_control_labeltoggle) {
									int newval = e->basket_value + 1;
									if (newval >= e->basket_count) 
										newval = 0; 
									else if (newval < 0)
										newval = e->basket_count - 1;
									Mnu_Preferences_Set_Basket_Value_ (e->opt_idx, newval);
									MENU_TOGGLE_SOFT_SOUND();
							} else if (e->q_control == q_control_labelbutton) {
								{ int row_px = 32; int n; for (n = 0; n < local_menu->cursor_solid_count; n ++) {
									mnu_ent_t *e = &mnu_ent_table[n];
									if (e->q_control == q_control_labeltoggle) {
										int newval = local_menu->cursor == opt_set_fitz_13 ? e->fitz_num : e->markv_num;
										Mnu_Preferences_Set_Basket_Value_ (e->opt_idx, newval);
										MENU_TOGGLE_SOFT_SOUND();
									}
								}} // end for
							}
							
	} // end switch (key)

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

LOCAL_EVENT (InitOnce) (menux_t *self)
{
	DEBUG_COMPILE_TIME_ASSERT (/*hint word*/ opt_COUNT, opt_COUNT == 15);   // it is 15 because of gamma slider!
	self->cursor_solid_count = opt_COUNT; // Immutable - let's do this and check cursor against disabled attribute

	{ int row_px = 32; int n; for (n = 0; n < self->cursor_solid_count; n ++) {
		mnu_ent_t *e = &mnu_ent_table[n];
		const char *our_commadic_text = e->comma_text;
		
#ifdef WINQUAKE_RENDERER_SUPPORT // Override text if winquake only string exists
		if (e->winquake_comma_text)  our_commadic_text = e->winquake_comma_text;
#endif // WINQUAKE_RENDERER_SUPPORT
		e->basket_count = String_Count_Char (our_commadic_text, ',') + 1; // Num of commas in the string + 1
		
		e->row_px	= row_px;
		e->col1_px	= self->column1;
		e->col2_px	= self->column2;
		e->cursor_col = e->col2_px - 2 * M_CHAR_HEIGHT_8;

		if (e->fitz_num > 100) {
			e->col1_px = e->fitz_num; // Really? Yes it is true.
			e->cursor_col = e->col1_px - 2 * M_CHAR_HEIGHT_8; // Really?
		}

		

		row_px += M_CHAR_HEIGHT_8;  if (e->is_gap_after) row_px += M_CHAR_HEIGHT_8;
	}} // End for
}


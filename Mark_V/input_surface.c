/*
Copyright (C) 2013 Baker

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// input_surface.c -- input


#include "quakedef.h"
#include "input.h"		// Courtesy
#include "menu_local.h"	// Privledged access

//
// Touch surfaces
//

focusor_t focus0;



// This by definition must be with mousedown?
void IN_AttackUp (lparse_t *line);
void Key_Game_Canvas_PressUp (void)
{
	switch (focus0.canvas.tap_is_down) {
	default:								// We somehow weren't down?
	case_break tap_type_looking_1:			View_StopPitchDrift (); // Stop drift?  Did this work?
											// Con_PrintLinef ("Accumulation: %4.3f time %2.4f", focus0.angle_accum, realtime - focus0.last_down_time);
	case_break tap_type_firing_2:			IN_AttackUp (NULL);
											focus0.canvas.is_attack_firing = false;
	}
	focus0.canvas.tap_is_down = tap_type_none_0;
}

// Up off hotspot is not a click
// Then again if the hotspot is continuous, we want the down.
void IN_AttackDown (lparse_t* line);
void Key_Game_Canvas_PressDown (int x, int y)
{
	float delta_time = focus0.canvas.last_down_time ? realtime - focus0.canvas.last_down_time : 99999;
	float newpitch, newyaw; QGetPitchYaw (&newpitch, &newyaw, x, y, focus0.game_viewport, &focus0.game_projection, &focus0.game_modelview, focus0.game_angles, clwidth, 0 /* winy */, focus0.znear, focus0.zfar); // pitch 0, yaw 2 for 77

	// Move
	if (focus0.canvas.tap_is_down) {
		log_fatal ("Already press down");
		return; // If already down, ignore it I guess?
	}
	
	if (delta_time < 0.25) {
		// TAP FIRE.  Short = assume tap fire.  FIRE IMMEDIATELY.
		if (newpitch > 180)
			newpitch = newpitch - 360; // 358?  ---> -2   320? ---> -40
		
		VectorSet (focus0.canvas.attack_angles, newpitch, newyaw, 0);	CL_BoundViewPitch (focus0.canvas.attack_angles); // No roll

		IN_AttackDown (NULL);
		focus0.canvas.is_attack_firing  = true;						// Begin firing?
		focus0.canvas.tap_is_down		= tap_type_firing_2; // For the moment.
	}
	else {
		// DRAG LOOK
		focus0.canvas.down_x = x, focus0.canvas.down_y = y;
		focus0.canvas.angle_accum			= 0.0;
		focus0.canvas.tap_is_down			= tap_type_looking_1; // What kind of tap?
	}

	focus0.canvas.last_down_time		= realtime;

}


void Key_Game_Button_Action (touch_button_e touch_button, cbool is_down)
{
	switch (touch_button) {
	default: // Nada
	case_break touch_button_forward_left_1:	Key_Event_Ex (NO_WINDOW_NULL, QKEY_TABLET_FORWARD, is_down, ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
											Key_Event_Ex (NO_WINDOW_NULL, QKEY_TABLET_LEFT, is_down, ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
	case_break touch_button_forward:		Key_Event_Ex (NO_WINDOW_NULL, QKEY_TABLET_FORWARD, is_down, ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
	case_break touch_button_forward_right:	Key_Event_Ex (NO_WINDOW_NULL, QKEY_TABLET_FORWARD, is_down, ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
											Key_Event_Ex (NO_WINDOW_NULL, QKEY_TABLET_RIGHT, is_down, ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
	case_break touch_button_left:			Key_Event_Ex (NO_WINDOW_NULL, QKEY_TABLET_LEFT, is_down, ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
	case_break touch_button_back:			Key_Event_Ex (NO_WINDOW_NULL, QKEY_TABLET_BACK, is_down, ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
	case_break touch_button_right:			Key_Event_Ex (NO_WINDOW_NULL, QKEY_TABLET_RIGHT, is_down, ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
	case_break touch_button_attack:			Key_Event_Ex (NO_WINDOW_NULL, QKEY_TABLET_ATTACK, is_down, ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
	case_break touch_button_jump:			//Key_Event_Ex (NO_WINDOW_NULL, QKEY_TABLET_FORWARD, is_down, ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
											Key_Event_Ex (NO_WINDOW_NULL, QKEY_TABLET_JUMP, is_down, ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
	case_break touch_button_next_weapon:	Key_Event_Ex (NO_WINDOW_NULL, QKEY_TABLET_NEXT_WEAPON, is_down, ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);

	case_break touch_button_turnleft:		Key_Event_Ex (NO_WINDOW_NULL, QKEY_TABLET_TURNLEFT, is_down, ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
	case_break touch_button_turnright:		Key_Event_Ex (NO_WINDOW_NULL, QKEY_TABLET_TURNRIGHT, is_down, ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
	case_break touch_button_showscores:		Key_Event_Ex (NO_WINDOW_NULL, QKEY_TABLET_SHOWSCORES, is_down, ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);

	}
}

void Key_Game_Button_UnPair (touch_button_t *btn)
{ DEBUG_ASSERT (btn->touch_stamp != NULL); {
	int stamp_idx					= Touch_Stamp_Find_Idx (btn->touch_stamp);
	touch_stamp_t	*touche			= &focus0.touch_stamps[stamp_idx];
	
	btn->touch_stamp = NULL; focus0.touch_buttons_num_down --; touche->touch_button_idx = IDX_NOT_FOUND_NEG1; // Unpair
}} // Ender


void Key_Game_Canvas_PressMove (int x, int y)
{
	float newpitch, newyaw; QGetPitchYaw (&newpitch, &newyaw, x, y, focus0.game_viewport, &focus0.game_projection, &focus0.game_modelview, focus0.game_angles, clwidth, 0 /* winy */, focus0.znear, focus0.zfar); // pitch 0, yaw 2 for 77

	//if (Flag_Check_Bool (mousebuttons, mousebuttonbits_mouse1_bit_1) && focus0.is_down) {
	if (focus0.canvas.tap_is_down == tap_type_looking_1) {
		float oldpitch, oldyaw; QGetPitchYaw (&oldpitch, &oldyaw, focus0.canvas.down_x, focus0.canvas.down_y, focus0.game_viewport, &focus0.game_projection, &focus0.game_modelview, focus0.game_angles, clwidth, 0 /* winy */, focus0.znear, focus0.zfar); // pitch 0, yaw 2 for 77
		
		if (focus0.canvas.down_x != x || focus0.canvas.down_y != y)  {
#if 0 // It is "correct" but feels reversed and too slow.
//			float delta_pitch	= angledelta_maybe_wrap (newpitch - oldpitch, NULL);	// I think this takes the shortest path?
// try		float delta_yaw	    = angledelta_maybe_wrap (newyaw - oldyaw, NULL);
#else
			float delta_pitch	= angledelta_maybe_wrap (-newpitch + oldpitch, NULL);	// I think this takes the shortest path?
			float delta_yaw	    = angledelta_maybe_wrap (-newyaw + oldyaw, NULL);
			float sens			= CLAMP(0.5, sensitivity.value, 61); // I guess.  61 isn't special.
			delta_pitch *= sens / 2.0;
			delta_yaw *= sens / 2.0;
#endif
			
			focus0.canvas.angle_accum	+= fabs(delta_pitch) + fabs(delta_yaw);

			// STUPIDITY: Quake pitch is 90 (full down) to -90 (full up)
			cl.viewangles[PITCH] = angle_maybe_wrap (cl.viewangles[PITCH] - delta_pitch, NULL);
			cl.viewangles[YAW]   = angle_maybe_wrap (cl.viewangles[YAW] - delta_yaw, NULL);

			// Baker: OOF!
			if (cl.viewangles[PITCH] > 180)
				cl.viewangles[PITCH] = cl.viewangles[PITCH] - 360; // 358?  ---> -2   320? ---> -40

			CL_BoundViewPitch (cl.viewangles);

			View_StopPitchDrift (); // Stop drift?  Did this work?			
			focus0.canvas.down_x = x, focus0.canvas.down_y = y;
		}
	}

	else if (focus0.canvas.tap_is_down == tap_type_firing_2) {
		// Update it.
		if (newpitch > 180)
			newpitch = newpitch - 360; // 358?  ---> -2   320? ---> -40

		VectorSet (focus0.canvas.attack_angles, newpitch, newyaw, 0);	CL_BoundViewPitch (focus0.canvas.attack_angles); // No roll
	}

}


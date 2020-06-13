#ifndef CORE_SDL
#include "environment.h"
#ifdef PLATFORM_WINDOWS // Has to be here, set by a header


/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2012 John Fitzgibbons and others
Copyright (C) 2009-2014 Baker and others

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
// in_win.c -- windows 95 mouse and joystick code
// 02/21/97 JCB Added extended DirectInput code to support external controllers.


#include "quakedef.h"
#include "winquake.h"

//
//#define JOY_ABSOLUTE_AXIS	0x00000000		// control like a joystick
//#define JOY_RELATIVE_AXIS	0x00000010		// control like a mouse, spinner, trackball
//#define	JOY_MAX_AXES		6			// X, Y, Z, R, U, V
//#define JOY_AXIS_X			0
//#define JOY_AXIS_Y			1
//#define JOY_AXIS_Z			2
//#define JOY_AXIS_R			3
//#define JOY_AXIS_U			4
//#define JOY_AXIS_V			5

///////////////////////////////////////////////////////////////////////////////
//  PLATFORM: DISPATCH.  AT LEAST THE DEFAULT ONE.
///////////////////////////////////////////////////////////////////////////////

#define JOYID_NOT_FOUND_NEG1			-1
#define STICK_THRESHOLD_0_9				0.9
#define TRIGGER_THRESHOLD_NEG_0_01		-0.01

static cbool				joy_disabled_commandline	= false;
static int					joy_active_instance_idx		= JOYID_NOT_FOUND_NEG1;
static cbool				first_read = false;

typedef enum
{
    SDL_CONTROLLER_AXIS_INVALID = -1,
    SDL_CONTROLLER_AXIS_LEFTX,			// 0
    SDL_CONTROLLER_AXIS_LEFTY,			// 1
    SDL_CONTROLLER_AXIS_TRIGGERLEFT,			// 2
    SDL_CONTROLLER_AXIS_RIGHTY,			// 3
    SDL_CONTROLLER_AXIS_RIGHTX,
//	SDL_CONTROLLER_AXIS_TRIGGERLEFT,	// 4
    SDL_CONTROLLER_AXIS_TRIGGERRIGHT,	// 5
    SDL_CONTROLLER_AXIS_MAX				// count = 6
} SDL_GameControllerAxis;

typedef enum
{
    SDL_CONTROLLER_BUTTON_INVALID = -1,
    SDL_CONTROLLER_BUTTON_A,
    SDL_CONTROLLER_BUTTON_B,
    SDL_CONTROLLER_BUTTON_X,
    SDL_CONTROLLER_BUTTON_Y,
    SDL_CONTROLLER_BUTTON_BACK,
    SDL_CONTROLLER_BUTTON_GUIDE,
    SDL_CONTROLLER_BUTTON_START,
    SDL_CONTROLLER_BUTTON_LEFTSTICK,
    SDL_CONTROLLER_BUTTON_RIGHTSTICK,
    SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
    SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
    SDL_CONTROLLER_BUTTON_DPAD_UP,
    SDL_CONTROLLER_BUTTON_DPAD_DOWN,
    SDL_CONTROLLER_BUTTON_DPAD_LEFT,
    SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
	SDL_CONTROLLER_BUTTON_LEFTTRIGGER,
	SDL_CONTROLLER_BUTTON_RIGHTTRIGGER,
    SDL_CONTROLLER_BUTTON_MAX
} SDL_GameControllerButton;

typedef struct joyaxis_s
{
	float x;
	float y;
} joyaxis_t;

typedef struct joy_buttonstate_s
{
	cbool buttondown[SDL_CONTROLLER_BUTTON_MAX];
} joybuttonstate_t;

typedef struct axisstate_s
{
	float axisvalue[SDL_CONTROLLER_AXIS_MAX]; // normalized to +-1
} joyaxisstate_t;

static joybuttonstate_t joy_buttonstate;
static joyaxisstate_t joy_axisstate;

static double joy_buttontimer[SDL_CONTROLLER_BUTTON_MAX];
static double joy_emulatedkeytimer[10];

//
//enum _ControlList
//{
//	AxisNada = 0, AxisForward, AxisLook, AxisSide, AxisTurn, AxisFly
//};
static	JOYINFOEX	ji;

static int		joy_haspov;
static int		joy_numbuttons;

//static int		joy_oldbuttonstate, joy_oldpovstate;


static int		joy_flags = JOY_RETURNCENTERED | JOY_RETURNBUTTONS | JOY_RETURNPOV | JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | JOY_RETURNR | JOY_RETURNU | JOY_RETURNV;


PDWORD	mControllerAxis[SDL_CONTROLLER_AXIS_MAX] = { &ji.dwXpos, &ji.dwYpos, &ji.dwZpos, &ji.dwRpos, &ji.dwUpos, &ji.dwVpos };

/*
================
IN_AxisMagnitude

Returns the vector length of the given joystick axis
================
*/
static vec_t IN_ApplyEasing_AxisMagnitude (joyaxis_t axis)
{
	vec_t magnitude = sqrt ((axis.x * axis.x) + (axis.y * axis.y));
	return magnitude;
}

/*
================
IN_ApplyEasing

assumes axis values are in [-1, 1] and the vector magnitude has been clamped at 1.
Raises the axis values to the given exponent, keeping signs.
================
*/
static joyaxis_t IN_ApplyEasing(joyaxis_t axis, float exponent)
{
	joyaxis_t result = {0};
	vec_t eased_magnitude;
	vec_t magnitude = IN_ApplyEasing_AxisMagnitude (axis);
	
	if (magnitude == 0)
		return result;
	
	eased_magnitude = pow(magnitude, exponent);
	
	result.x = axis.x * (eased_magnitude / magnitude);
	result.y = axis.y * (eased_magnitude / magnitude);
	return result;
}

/*
================
IN_ApplyMoveEasing

same as IN_ApplyEasing, but scales the output by sqrt(2).
this gives diagonal stick inputs coordinates of (+/-1,+/-1).

forward/back/left/right will return +/- 1.41; this shouldn't be a problem because
you can pull back on the stick to go slower (and the final speed is clamped
by sv_maxspeed).
================
*/
static joyaxis_t IN_ApplyMoveEasing(joyaxis_t axis, float exponent)
{
	joyaxis_t result = IN_ApplyEasing(axis, exponent);
	const float v = sqrt (2.0);
	
	result.x *= v;
	result.y *= v;

	return result;
}

/*
================
IN_ApplyDeadzone

in: raw joystick axis values converted to floats in +-1
out: applies a circular deadzone and clamps the magnitude at 1
     (my 360 controller is slightly non-circular and the stick travels further on the diagonals)

deadzone is expected to satisfy 0 < deadzone < 1

from https://github.com/jeremiah-sypult/Quakespasm-Rift
and adapted from http://www.third-helix.com/2013/04/12/doing-thumbstick-dead-zones-right.html
================
*/
static joyaxis_t IN_ApplyDeadzone(joyaxis_t axis, float deadzone)
{
	joyaxis_t result = {0};
	vec_t magnitude = IN_ApplyEasing_AxisMagnitude (axis);
	
	if ( magnitude > deadzone ) {
		const vec_t new_magnitude = c_min(1.0, (magnitude - deadzone) / (1.0 - deadzone));
		const vec_t scale = new_magnitude / magnitude;
		result.x = axis.x * scale;
		result.y = axis.y * scale;
	}
	
	return result;
}


/*
================
IN_JoyKeyEvent

Sends a Key_Event if a unpressed -> pressed or pressed -> unpressed transition occurred,
and generates key repeats if the button is held down.

Adapted from DarkPlaces by lordhavoc
================
*/
static void IN_JoyKeyEvent(cbool wasdown, cbool isdown, key_scancode_e key, double *timer)
{
	// we can't use `realtime` for key repeats because it is not monotomic
	const double currenttime = System_DoubleTime();
	
	//__android_log_print( ANDROID_LOG_INFO, CORE_ANDROID_LOG_TAG, "IN_JoyKeyEvent");

	if (wasdown)
	{
		if (isdown)
		{
			if (currenttime >= *timer)
			{
				*timer = currenttime + 0.1;
				Key_Event_Ex (NO_WINDOW_NULL, key, /*down */ true, ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
			}
		}
		else
		{
			*timer = 0;
			
			Key_Event_Ex (NO_WINDOW_NULL, key, /*down */ false, ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
		}
	}
	else
	{
		if (isdown)
		{
			*timer = currenttime + 0.5;
			Key_Event_Ex (NO_WINDOW_NULL, key, /*down */ true, ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
		}
	}
}

/*
================
IN_KeyForControllerButton
================
*/
static int IN_KeyForControllerButton(SDL_GameControllerButton /* HERONED*/ button)
{
	switch (button)
	{
		case SDL_CONTROLLER_BUTTON_A: return K_ABUTTON;
		case SDL_CONTROLLER_BUTTON_B: return K_BBUTTON;
		case SDL_CONTROLLER_BUTTON_X: return K_XBUTTON;
		case SDL_CONTROLLER_BUTTON_Y: return K_YBUTTON;
case 4 /*SDL_CONTROLLER_BUTTON_LEFTTRIGGER*/: return K_LSHOULDER;
	case 5 /*SDL_CONTROLLER_BUTTON_LEFTTRIGGER*/: return K_RSHOULDER;

		case 6 /*SDL_CONTROLLER_BUTTON_BACK*/: return K_TAB;
		case 7 /*SDL_CONTROLLER_BUTTON_START*/: return K_ESCAPE;

//		case SDL_CONTROLLER_BUTTON_LEFTSTICK: return K_LTHUMB;
//		case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return K_RTHUMB;
		case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return K_LTRIGGER;
		case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return K_RTRIGGER;
		case SDL_CONTROLLER_BUTTON_DPAD_UP: return K_UPARROW;
		case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return K_DOWNARROW;
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return K_LEFTARROW;
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return K_RIGHTARROW;
//#if 1 // Mobile only
//		case SDL_CONTROLLER_BUTTON_LEFTTRIGGER: return K_LTRIGGER;
//		case SDL_CONTROLLER_BUTTON_RIGHTTRIGGER: return K_RTRIGGER;
//#endif

		default: return 0;
	}
}

/*
================
IN_Commands

Emit key events for game controller buttons, including emulated buttons for analog sticks/triggers
================
*/

cbool SDL_GameControllerGetButton (int button_num)
{
	if (in_range (SDL_CONTROLLER_BUTTON_DPAD_UP, button_num, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) {
		//Con_SafePrintLinef ("pov %d", ji.dwPOV);
		if(ji.dwPOV == JOY_POVCENTERED) return false;

		switch (button_num) { // Degrees
		default: // Impossible
		case SDL_CONTROLLER_BUTTON_DPAD_UP:		return in_range (    0, ji.dwPOV,  4500);
		case SDL_CONTROLLER_BUTTON_DPAD_DOWN:	return in_range (13500, ji.dwPOV, 22500);
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT:	return in_range (22500, ji.dwPOV, 31500);
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:	return in_range ( 4500, ji.dwPOV, 13500);
		}
		return false;
	}

	if (button_num == 0)
		button_num = button_num + ji.dwButtons * 0;
	{
		unsigned buttonstate	= ji.dwButtons;
		unsigned button_bit		= 1 << button_num;
		return CONVBOOL (buttonstate & button_bit);
	}
}


void Input_Local_Joystick_Commands (void)
{
	joyaxisstate_t newaxisstate;

	const float stickthreshold = STICK_THRESHOLD_0_9;
	const float triggerthreshold = joy_deadzone_trigger.value;
	if (joy_disabled_commandline || joy_active_instance_idx == JOYID_NOT_FOUND_NEG1 || !joy_enable.value /*|| !joy_active_joystick*/)
		return;

	if (!first_read)
		return; // Haven't read it yet

	//Con_SafePrintLinef ("joybutt %x", (int)ji.dwButtons);
	// emit key events for controller buttons
	{ int n; for (n = 0; n < SDL_CONTROLLER_BUTTON_MAX; n++) {
		cbool newstate = SDL_GameControllerGetButton (n);
		cbool oldstate = joy_buttonstate.buttondown[n];
		
		joy_buttonstate.buttondown[n] = newstate;
		
		// NOTE: This can cause a reentrant call of IN_Commands, via SCR_ModalMessage when confirming a new game.
		IN_JoyKeyEvent(oldstate, newstate, IN_KeyForControllerButton((SDL_GameControllerButton)n), &joy_buttontimer[n]);
	}}

	{ int n; for (n = 0; n < SDL_CONTROLLER_AXIS_MAX; n++) {
		newaxisstate.axisvalue[n] = ((*mControllerAxis[n]) - 32768.0) / 32768.0f;
	}}


	// emit emulated arrow keys so the analog sticks can be used in the menu
	if (key_dest != key_game)
	{
		IN_JoyKeyEvent(joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_LEFTX] < -stickthreshold, newaxisstate.axisvalue[SDL_CONTROLLER_AXIS_LEFTX] < -stickthreshold, K_LEFTARROW, &joy_emulatedkeytimer[0]);
		IN_JoyKeyEvent(joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_LEFTX] > stickthreshold,  newaxisstate.axisvalue[SDL_CONTROLLER_AXIS_LEFTX] > stickthreshold, K_RIGHTARROW, &joy_emulatedkeytimer[1]);
		IN_JoyKeyEvent(joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_LEFTY] < -stickthreshold, newaxisstate.axisvalue[SDL_CONTROLLER_AXIS_LEFTY] < -stickthreshold, K_UPARROW, &joy_emulatedkeytimer[2]);
		IN_JoyKeyEvent(joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_LEFTY] > stickthreshold,  newaxisstate.axisvalue[SDL_CONTROLLER_AXIS_LEFTY] > stickthreshold, K_DOWNARROW, &joy_emulatedkeytimer[3]);
		IN_JoyKeyEvent(joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_RIGHTX] < -stickthreshold,newaxisstate.axisvalue[SDL_CONTROLLER_AXIS_RIGHTX] < -stickthreshold, K_LEFTARROW, &joy_emulatedkeytimer[4]);
		IN_JoyKeyEvent(joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_RIGHTX] > stickthreshold, newaxisstate.axisvalue[SDL_CONTROLLER_AXIS_RIGHTX] > stickthreshold, K_RIGHTARROW, &joy_emulatedkeytimer[5]);
		IN_JoyKeyEvent(joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_RIGHTY] < -stickthreshold,newaxisstate.axisvalue[SDL_CONTROLLER_AXIS_RIGHTY] < -stickthreshold, K_UPARROW, &joy_emulatedkeytimer[6]);
		IN_JoyKeyEvent(joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_RIGHTY] > stickthreshold, newaxisstate.axisvalue[SDL_CONTROLLER_AXIS_RIGHTY] > stickthreshold, K_DOWNARROW, &joy_emulatedkeytimer[7]);
	}
#if 1 // On Android these don't seem to be analog.
	// emit emulated keys for the analog triggers
	// For true Android devices, this fires both.  For an XBox One S Controller, it works properly.
//	if (!joy_active_joystick_trigger) {
		// AXIS REVERSED FROM DESKTOP!
#ifdef PLATFORM_ANDROID
		IN_JoyKeyEvent(joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_TRIGGERLEFT] > triggerthreshold,  newaxisstate.axisvalue[SDL_CONTROLLER_AXIS_TRIGGERLEFT] > triggerthreshold, K_RTRIGGER, &joy_emulatedkeytimer[8]);
		IN_JoyKeyEvent(joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_TRIGGERRIGHT] > triggerthreshold, newaxisstate.axisvalue[SDL_CONTROLLER_AXIS_TRIGGERRIGHT] > triggerthreshold, K_LTRIGGER, &joy_emulatedkeytimer[9]);
#else
		IN_JoyKeyEvent(joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_TRIGGERLEFT] < TRIGGER_THRESHOLD_NEG_0_01,  newaxisstate.axisvalue[SDL_CONTROLLER_AXIS_TRIGGERLEFT] < TRIGGER_THRESHOLD_NEG_0_01, K_RTRIGGER, &joy_emulatedkeytimer[8]);
		IN_JoyKeyEvent(joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_TRIGGERLEFT] > triggerthreshold, newaxisstate.axisvalue[SDL_CONTROLLER_AXIS_TRIGGERLEFT] > triggerthreshold, K_LTRIGGER, &joy_emulatedkeytimer[9]);
#endif
//	}
#endif
	joy_axisstate = newaxisstate;
}

cbool Input_Local_Joystick_Read (void)
{
	memset (&ji, 0, sizeof(ji));

	ji.dwSize = sizeof(ji);
	ji.dwFlags = joy_flags;

	if (joyGetPosEx(joy_active_instance_idx, &ji) == JOYERR_NOERROR) {
		//Con_SafePrintLinef ("0: %2.4f 1: %2.4f 2: %2.4f 3: %2.4f 4: %2.4f 5: %2.4f",
		//	(float)(*mControllerAxis[0]), 
		//	(float)(*mControllerAxis[1]),
		//	(float)(*mControllerAxis[2]),
		//	(float)(*mControllerAxis[3]),
		//	(float)(*mControllerAxis[4]),
		//	(float)(*mControllerAxis[5])
		//
		//);
		first_read = true;
		return true;
	}
	
	return false;
}




void Input_Local_Joystick_Shutdown (void)
{
	// We don't have to do anything here

}



void Input_Local_Joystick_Move (usercmd_t *cmd)
{
	float	speed;
	joyaxis_t moveRaw, moveDeadzone, moveEased;
	joyaxis_t lookRaw, lookDeadzone, lookEased;


	// verify joystick is available and that the user wants to use it
	if (joy_disabled_commandline || joy_active_instance_idx == JOYID_NOT_FOUND_NEG1 || !joy_enable.value /*|| !joy_active_joystick*/)
		return;

	// collect the joystick data, if possible
	if (!Input_Local_Joystick_Read ())
		return;
	
	moveRaw.x = joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_LEFTX];
	moveRaw.y = joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_LEFTY];
	lookRaw.x = joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_RIGHTX];
	lookRaw.y = joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_RIGHTY];
	                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
	//Con_SafePrintLinef ( "Input_Local_Joystick_move: %g %g %g %g", moveRaw.x, moveRaw.y, lookRaw.x, lookRaw.y);

	if (joy_swapmovelook.value)
	{
		joyaxis_t temp = moveRaw;
		moveRaw = lookRaw;
		lookRaw = temp;
	}
	
	moveDeadzone = IN_ApplyDeadzone (moveRaw, joy_deadzone.value);
	lookDeadzone = IN_ApplyDeadzone (lookRaw, joy_deadzone.value);

	moveEased = IN_ApplyMoveEasing (moveDeadzone, joy_exponent_move.value);
	lookEased = IN_ApplyEasing (lookDeadzone, joy_exponent.value);
	
	//if ((in_speed.state & 1) ^ (cl_alwaysrun.value != 0.0))
	if (in_speed.state & 1)
		speed = cl_movespeedkey.value;
	else
		speed = 1;

	cmd->sidemove += (cl_sidespeed.value * speed * moveEased.x);
	cmd->forwardmove -= (cl_forwardspeed.value * speed * moveEased.y);

	cl.viewangles[YAW] -= lookEased.x * joy_sensitivity_yaw.value * host_frametime_;
	cl.viewangles[PITCH] += lookEased.y * joy_sensitivity_pitch.value * (joy_invert.value ? -1.0 : 1.0) * host_frametime_;

	if (lookEased.x != 0 || lookEased.y != 0)
		View_StopPitchDrift ();


	// bounds check pitch
	CL_BoundViewPitch (cl.viewangles); // joystick lock
}



cbool Input_Local_Joystick_Startup (void)
{
	//alert ("Input_Local_Joystick_Startup");
	if (!COM_CheckParm("-joystick")) {
		joy_disabled_commandline = true;
		return false;
	}

	{
		int numdevs = joyGetNumDevs ();

		// verify joystick driver is present
		if (numdevs == 0) {
			Con_SafePrintLinef (NEWLINE "joystick not found -- driver not present" NEWLINE);
			return false;
		}

		{
			MMRESULT	mmr = -1;
			int joy_provisional_idx = JOYID_NOT_FOUND_NEG1;

			// cycle through the joystick ids for the first valid one
			{ int n; for (n = 0; n < numdevs ; n ++) {
				memset (&ji, 0, sizeof(ji));
				ji.dwSize = sizeof(ji);
				ji.dwFlags = JOY_RETURNCENTERED;

				if ((mmr = joyGetPosEx(n, &ji)) == JOYERR_NOERROR) {
					// There is a joystick here.
					//static char *GetJoystickName_Alloc (int index, const char *szRegKey); char *name =  GetJoystickName_Alloc (n, "");					

					// ACCEPTED
					joy_provisional_idx = n;
					break;					
				} // End if joystick found
			}} // End for loop

			// abort startup if we didn't find a valid joystick
			//if (mmr != JOYERR_NOERROR /* which is zero*/)
			if (joy_provisional_idx == JOYID_NOT_FOUND_NEG1) {
				Con_SafePrintLinef ("joystick not found -- no valid joysticks (%x)", mmr);
				return false;
			}

			{ // get the capabilities of the selected joystick abort startup if command fails
				JOYCAPS		jc = {0};
				
				if ((mmr = joyGetDevCaps(joy_active_instance_idx, &jc, sizeof(jc))) != JOYERR_NOERROR) {
					Con_PrintLinef ("joystick not found -- invalid joystick capabilities (%x)", mmr);
					return false;
				}

				// save the joystick's number of buttons and POV status buttons 10/pov 16 for Xbox One Controller
				joy_numbuttons	= jc.wNumButtons;
				joy_haspov		= jc.wCaps & JOYCAPS_HASPOV;

				// old button and POV states default to no buttons pressed
				//joy_oldbuttonstate = joy_oldpovstate = 0;

				// mark the joystick as available and advanced initialization not completed
				// this is needed as cvars are not available during initialization

				// get the capabilities of the selected joystick abort startup if command fails
				Cmd_AddCommands ((voidfunc_t)Input_Local_Joystick_Startup); // Warning because Input_Local_Joystick_Startup is cbool return not void

				Con_PrintLinef (NEWLINE "joystick detected" NEWLINE);
				joy_active_instance_idx = joy_provisional_idx;
				return true;
			}
		}
	}
}

cbool Input_Local_Is_Joystick (void)
{
	return joy_active_instance_idx != JOYID_NOT_FOUND_NEG1; // For now?
}



//#include <mmsystem.h>
//#include <regstr.h>
//
//#ifdef REGSTR_VAL_JOYOEMNAME 
//#undef REGSTR_VAL_JOYOEMNAME 
//#endif
//#define REGSTR_VAL_JOYOEMNAME "OEMName"
//
//static char *GetJoystickName_Alloc (int index, const char *szRegKey) // May return NULL
//{
//    /* added 7/24/2004 by Eckhard Stolberg */
//    /*
//       see if there is a joystick for the current
//       index (1-16) listed in the registry
//     */
//    char *name = NULL;
//    HKEY hTopKey;
//    HKEY hKey;
//    DWORD regsize;
//    LONG regresult;
//    char regkey[256];
//    char regvalue[256];
//    char regname[256];
//
//	c_snprintf3 (regkey, "%s\\%s", REGSTR_PATH_JOYCONFIG, szRegKey, REGSTR_KEY_JOYCURR); //SDL_snprintf(regkey, SDL_arraysize(regkey),
//               
//    hTopKey = HKEY_LOCAL_MACHINE;
//    regresult = RegOpenKeyExA(hTopKey, regkey, 0, KEY_READ, &hKey);
//    if (regresult != ERROR_SUCCESS) {
//        hTopKey = HKEY_CURRENT_USER;
//        regresult = RegOpenKeyExA(hTopKey, regkey, 0, KEY_READ, &hKey);
//    }
//    if (regresult != ERROR_SUCCESS) {
//        return NULL;
//    }
//
//    /* find the registry key name for the joystick's properties */
//    regsize = sizeof(regname);
//    //SDL_snprintf(regvalue, SDL_arraysize(regvalue), "Joystick%d%s", index + 1, REGSTR_VAL_JOYOEMNAME);
//	c_snprintf2 (regvalue, "Joystick%d%s", index + 1, REGSTR_VAL_JOYOEMNAME);
//    regresult =
//        RegQueryValueExA(hKey, regvalue, 0, 0, (LPBYTE) regname, &regsize);
//    RegCloseKey(hKey);
//
//    if (regresult != ERROR_SUCCESS) {
//        return NULL;
//    }
//
//    /* open that registry key */    
//	//SDL_snprintf(regkey, SDL_arraysize(regkey),
//	c_snprintf2 (regkey, "%s\\%s", REGSTR_PATH_JOYOEM, regname);
//                 
//    regresult = RegOpenKeyExA(hTopKey, regkey, 0, KEY_READ, &hKey);
//    if (regresult != ERROR_SUCCESS) {
//        return NULL;
//    }
//
//    /* find the size for the OEM name text */
//    regsize = sizeof(regvalue);
//    regresult =
//        RegQueryValueExA(hKey, REGSTR_VAL_JOYOEMNAME, 0, 0, NULL, &regsize);
//    if (regresult == ERROR_SUCCESS) {
//        /* allocate enough memory for the OEM name text ... */
//        name = (char *) malloc(regsize);
//        if (name) {
//            /* ... and read it from the registry */
//            regresult = RegQueryValueExA(hKey,
//                                         REGSTR_VAL_JOYOEMNAME, 0, 0,
//                                         (LPBYTE) name, &regsize);
//        }
//    }
//    RegCloseKey(hKey);
//
//    return (name);
//}


#endif // PLATFORM_WINDOWS

#endif // !CORE_SDL
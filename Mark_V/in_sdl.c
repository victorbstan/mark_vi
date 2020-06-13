#ifdef CORE_SDL
#include "environment.h"
#ifndef PLATFORM_ANDROID // NOT ANDROID



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
// in_sdl.c


#include "quakedef.h"
#include "sdlquake.h"



///////////////////////////////////////////////////////////////////////////////
//  PLATFORM: DISPATCH.  AT LEAST THE DEFAULT ONE.
///////////////////////////////////////////////////////////////////////////////

#define JOYID_NOT_FOUND_NEG1 -1
static cbool				joy_disabled_commandline	= false;
static SDL_JoystickID		joy_active_instance_idx		= JOYID_NOT_FOUND_NEG1;
static SDL_GameController *		joy_active_joystick			= NULL;
//static cbool				joy_active_joystick_trigger	= false;
//static SDL_GameController *	joy_active_game_controller	= NULL;
static char					joy_active_name[256];

cbool Input_Local_Is_Joystick (void)
{
	return CONVBOOL joy_active_joystick;
}

static cbool IN_Joystick_Active_Close (SDL_GameController *joy_consider)
{ DEBUG_ASSERT (joy_active_joystick == joy_consider); {
	
	joy_active_joystick			= NULL;
	joy_active_instance_idx		= JOYID_NOT_FOUND_NEG1;
	joy_active_name[0]			= 0;
//	joy_active_joystick_trigger	= false;
//	joy_active_game_controller	= NULL;
	return false;
}}

//static cbool IN_Joystick_Consider_Opened (SDL_GameController *joy_consider)
//{ DEBUG_ASSERT (!joy_active_joystick); {
//	int buttons = SDL_JoystickNumButtons(joy_consider);
//	const char *joyname = SDL_JoystickName (joy_consider);	
//
//	//alert ("IN_Joystick_Consider_Opened");
//	if (buttons >= 4) {
//		// Set the 3 variables
//		//alert ("IN_Joystick_Consider_Opened %d", SDL_JoystickInstanceID(joy_consider));
//		joy_active_joystick	= joy_consider;
//		joy_active_instance_idx	= SDL_JoystickInstanceID(joy_consider);
//		c_strlcpy (joy_active_name, joyname);
//
//		//alert ("Controller detected: " QUOTED_S " with instance id %d", joy_active_name, joy_active_instance_idx);
//		//while (1) {
//		//	if ( SDL_IsGameController(joy_active_instance_idx) ) {
//		//		const char *controllername = SDL_GameControllerNameForIndex(joy_active_instance_idx);
//		//		SDL_GameController *gamecontroller = SDL_GameControllerOpen(joy_active_instance_idx);
//		//		if (gamecontroller) {
//		//			Con_SafePrintLinef ("Controller detected: %s", controllername != NULL ? controllername : "NULL");
//		//			joy_active_game_controller = gamecontroller;
//		//			break;
//		//		}
//		//	}
//
//			Con_SafePrintLinef ("Joystick detected: " QUOTED_S " with instance id %d", joy_active_name, joy_active_instance_idx);
//			//alert ("Controller detected: " QUOTED_S " with instance id %d", joy_active_name, joy_active_instance_idx);
//		//	break;
//		//}
//		return true;
//	}
//	
//	//alert ("Ignoring");
//	Con_WarningLinef ("Ignoring controller: %s", joyname != NULL ? joyname : "NULL");
//
//	if (SDL_JoystickGetAttached(joy_consider)) {
//		//alert ("About to close");
//		SDL_JoystickClose(joy_consider);
//		//alert ("After close");
//	} else {
//		Con_SafePrintLinef ("Joystick not attached"); // Does this happen
//	}
//	return false;
//}}



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
		case SDL_CONTROLLER_BUTTON_BACK: return K_TAB;
		case SDL_CONTROLLER_BUTTON_START: return K_ESCAPE;	
		case SDL_CONTROLLER_BUTTON_LEFTSTICK: return K_LTHUMB;
		case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return K_RTHUMB;
		case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return K_LSHOULDER;
		case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return K_RSHOULDER;
		case SDL_CONTROLLER_BUTTON_DPAD_UP: return K_UPARROW;
		case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return K_DOWNARROW;
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return K_LEFTARROW;
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return K_RIGHTARROW;
#if 0 // Mobile only
		case SDL_CONTROLLER_BUTTON_LEFTTRIGGER: joy_active_joystick_trigger = true; return K_LTRIGGER;
		case SDL_CONTROLLER_BUTTON_RIGHTTRIGGER: joy_active_joystick_trigger = true; return K_RTRIGGER;
#endif

		default: return 0;
	}
}

int Input_Local_Capture_Mouse (cbool bDoCapture)
{
	static cbool captured = false;

	if (bDoCapture && !captured)
	{
//	    SDL_WM_GrabInput(SDL_GRAB_ON);
//	    SDL_SetRelativeMouseMode(SDL_TRUE);
//		ShowCursor (FALSE); // Hides mouse cursor
//		SetCapture (sysplat.mainwindow);	// Captures mouse events
#ifdef INPUT_RELATIVE
		SDL_SetRelativeMouseMode (SDL_TRUE);
#else // ^^^ INPUT_RELATIVE
		SDL_ShowCursor (SDL_DISABLE /*0 FALSE*/); // Hides mouse cursor
		SDL_CaptureMouse (SDL_ENABLE /* 1 TRUE*/); //  Use this function to capture the mouse and to track input outside an SDL window.
#endif // !INPUT_RELATIVE
		// Con_DPrintLinef ("Mouse Captured");
		captured = true;
	}

	if (!bDoCapture && captured)
	{
//	    SDL_WM_GrabInput(SDL_GRAB_OFF);
//	    SDL_SetRelativeMouseMode(SDL_FALSE);
		//ShowCursor (TRUE); // Hides mouse cursor
		//ReleaseCapture ();
		//ClipCursor (NULL); // Can't hurt
#ifdef INPUT_RELATIVE
		SDL_SetRelativeMouseMode (SDL_FALSE);
#else // ^^^ INPUT_RELATIVE
		SDL_CaptureMouse (SDL_DISABLE /* 0 FALSE*/); //  Use this function to capture the mouse and to track input outside an SDL window.
		SDL_ShowCursor (SDL_ENABLE /* 1 TRUE*/); // Hides mouse cursor
#endif // !INPUT_RELATIVE
		// Con_DPrintLinef ("Mouse Released");
		captured = false;
	}

	return 1; // Accepted
}


cbool Input_Local_Update_Mouse_Clip_Region_Think (mrect_t* mouseregion)
{
// What's the plan here?
	mrect_t oldregion = *mouseregion;
	// This should be the client area ...  does it return negative too I hope?
	SDL_GetWindowPosition (sysplat.mainwindow, &mouseregion->left, &mouseregion->top);
	SDL_GetWindowSize (sysplat.mainwindow, &mouseregion->width, &mouseregion->height);

	// What about +1 possibility?

	if (memcmp (mouseregion, &oldregion, sizeof(mrect_t) ) != 0)
	{  // Changed!
		mouseregion->bottom = mouseregion->top + mouseregion->height;
		mouseregion->right = mouseregion->left + mouseregion->width;
		mouseregion->center_x = (mouseregion->left + mouseregion->right) / 2;
		mouseregion->center_y = (mouseregion->top + mouseregion->bottom) / 2;
		//ClipCursor (&windowinfo.rcClient); // SDL can't.  Also if cursor is invisible should Windows even try?  Yeah probably.
		return true;
	}
	return false;
}

void Input_Local_Mouse_Cursor_SetPos (int x, int y)
{
//	SetCursorPos (x, y);
#ifndef PLATFORM_ANDROID // Android + SDL 2.0.3 does not have SDL_WarpMouseGlobal is 2.0.4 feature
	if (SDL_WarpMouseGlobal (x,y) != 0) // Returns 0 on success.
		log_debug ("Unable to set mouse position");
#endif // PLATFORM_ANDROID
}

void Input_Local_Mouse_Cursor_GetPos (required int *px, required int *py, cbool towindow)
{
#ifndef PLATFORM_ANDROID // Android + SDL 2.0.3 does not have SDL_GetGlobalMouseState is 2.0.4 feature
	int x, y;
	if (SDL_GetGlobalMouseState (&x, &y) != 0) // Returns 0 on success
		log_debug ("Unable to get mouse position");

	REQUIRED_ASSIGN (px, x);
	REQUIRED_ASSIGN (py, y);
#endif // !PLATFORM_ANDROID
}

#ifdef PLATFORM_ANDROID
cbool Touch_Out_SDL (SDL_Event *e)
{
	mouseaction_e	mouseaction =	e->type == SDL_FINGERMOTION ? mouseaction_move : 
									e->type == SDL_FINGERUP		? mouseaction_up : 
																  mouseaction_down_0; // Cancel?  Guess no cancel for SDL;
#if PLATFORM_SCREEN_PORTRAIT !=0 // And we are.
	//float x_pct	= (1.0 - e->tfinger.y);	// Because portrait 
	float x_pct	= e->tfinger.y;	// Because portrait 
	float y_pct	= (1.0 - e->tfinger.x);			// Because portrait HEREON
//	float y_pct	= e->tfinger.x;			// Because portrait
	float x		= x_pct * vid.screen.width; // Right?
	float y		= y_pct * vid.screen.height; // Right?
	// This will cease to be right soon?
// March 26 2018	int q_x		= x * (320.0 / vid.screen.width);	// WINQUAKE_RENDERER_SUPPORT
// March 26 2018	int q_y		= y * (240.0 / vid.screen.height);
#else
	float x_pct	= e->tfinger.x;	// Because portrait
	float y_pct	= e->tfinger.y;			// Because portrait
	float x		= x_pct * vid.screen.width; // Right?
	float y		= y_pct * vid.screen.height; // Right?
#endif // ! PORTRAIT

	if (e->tfinger.x < 0)
		Con_WarningLinef ("Out of bounds x coordinate %g", e->tfinger.x);

	//__android_log_print(ANDROID_LOG_INFO, CORE_ANDROID_LOG_TAG,  "SDL_FINGER id %daction %d xy: %d %d", (int)e->tfinger.fingerId, mouseaction, q_x, q_y);
//	if (mouseaction == mouseaction_up) {
//		__android_log_print(ANDROID_LOG_INFO, CORE_ANDROID_LOG_TAG,"SDL_FINGER %d raw (%1.2f %1.2f) final (%d %d) screen (%d %d) desktop (%d %d)", mouseaction, x_pct, y_pct, (int)x, (int)y, vid.screen.width, vid.screen.height, vid.desktop.width, vid.desktop.height );
//	} 

	// Adding 10 to avoid 0 because tfinger is starting at index 0, but we consider an id of 0 to be NULL (unassigned).
	Touch_Action ((void *)(e->tfinger.fingerId + 10), mouseaction, x, y);
	return true;
}
#endif // PLATFORM_ANDROID


cbool SDLQ_IN_ReadInputMessages (void *_sdl_event)
{
	SDL_Event  *e    = _sdl_event;
	cbool down = false;

	int button_bits = 0;

	switch (e->type) {
#if 0
	case SDL_JOYHATMOTION: {           /**< Joystick hat position change */
		Key_Event_Ex (NO_WINDOW_NULL, K_UPARROW,  /*down*/ CONVBOOL (e->jhat.value & SDL_HAT_UP), K_UPARROW, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1); // Future, convert this to utf-32
		Key_Event_Ex (NO_WINDOW_NULL, K_LEFTARROW,  /*down*/ CONVBOOL (e->jhat.value & SDL_HAT_LEFT), K_LEFTARROW, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1); // Future, convert this to utf-32
		Key_Event_Ex (NO_WINDOW_NULL, K_DOWNARROW,  /*down*/ CONVBOOL (e->jhat.value & SDL_HAT_DOWN), K_DOWNARROW, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1); // Future, convert this to utf-32
		Key_Event_Ex (NO_WINDOW_NULL, K_RIGHTARROW,  /*down*/ CONVBOOL (e->jhat.value & SDL_HAT_RIGHT), K_RIGHTARROW, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1); // Future, convert this to utf-32
		return true;
	}
	case SDL_JOYBUTTONDOWN: {
//		alert ("Button %d", (int)e->jbutton.button);
		int keycode = IN_KeyForControllerButton (e->jbutton.button);
		
		if (keycode) {
//			alert ("Keycode %d", keycode);
			Key_Event_Ex (NO_WINDOW_NULL, keycode,  /*down*/ true, keycode, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1); // Future, convert this to utf-32
		}
		//else alert ("Button %d not handled", (int)e->jbutton.button);
		return true; // handled
	}		

	case SDL_JOYBUTTONUP: {
		int keycode = IN_KeyForControllerButton (e->jbutton.button);
		if (keycode) {
			Key_Event_Ex (NO_WINDOW_NULL, keycode,  /*down*/ false, keycode, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1); // Future, convert this to utf-32
		}
		return true; // handled
	}
#endif

//
// Input events
//

// Keyboard character emission

// SDL 2.0.3 ANDROID IS NOT GIVING US SDL_TEXTINPUT!!!!
// March 22 2018 - Workaround is below in SDL_KEYDOWN/SDL_KEYUP
	case SDL_TEXTINPUT:
		if (1) {
#pragma message ("TODO: emit UTF-16 or UCS2 or something")
			char *ch; for (ch = e->text.text; *ch; ch++) {
				int unicode = *ch;
				int ascii 	= in_range (32, unicode, 126) ? unicode : 0;
				//alert ("SDL_TEXTINPUT: %d", (int)ascii);  Doesn't happen.
				// We do not do control characters here.
				Key_Event_Ex (NO_WINDOW_NULL, SCANCODE_0,  /*down*/ true, ascii, unicode, shiftbits (e)); // Future, convert this to utf-32
			}
		}
		return true;

//
// Keyboard scancode emission
//

	case SDL_KEYDOWN:
		down = true;  // Fall through ...

	case SDL_KEYUP:

		if (1) {
			int theirs				= e->key.keysym.scancode;
			key_scancode_e scancode	= keymap[theirs][2];
			// Keypad HOME, END, etc are screwed for now and won't emit a "keypress" (but will emit a keydown)
			// Reason?  We don't have a way to know from the scan code whether it should emit a number or a cursor move
			// And SDL numlock reports wrong.
			// I'm not going to write a per platform numlock checker to work around this SDL limitation.
			// But on windows is a simple GetKeyState (VK_NUMLOCK) != 0;
			cbool should_emit 		= down && (!in_range (32, scancode, 126) && !in_range (K_NUMPAD_0, scancode, K_NUMPAD_DIVIDE));
			int ascii				= should_emit ? scancode : 0;
			if (scancode)
				//Key_Event_Ex (NO_WINDOW_NULL, scancode, down, ascii, /*unicode*/ 0, shiftbits (e) ); // Future, convert this to utf-32
#ifdef PLATFORM_ANDROID
				// ANDROID IS NOT GIVING US SDL_TEXTINPUT
				// So we push through both scancode + ascii
				// And have Key_Event_Ex wired slightly differently for
				// for Android
				//alert ("Scancode %d ascii %d", scancode, ascii);
				Key_Event_Ex (NO_WINDOW_NULL, scancode, down, /*should_emit*/  ascii, /*unicode*/ UNICODE_0, shiftbits (e)); // Future, convert this to utf-32
#else
				Key_Event_Ex (NO_WINDOW_NULL, scancode, down, /*should_emit*/  ASCII_0, /*unicode*/ UNICODE_0, shiftbits (e)); // Future, convert this to utf-32
#endif

		}

		return true; // handled

#ifdef PLATFORM_ANDROID // For Android we want only the touch events
	case SDL_FINGERMOTION:	return Touch_Out_SDL (e);
	case SDL_FINGERDOWN:	return Touch_Out_SDL (e);
	case SDL_FINGERUP:		return Touch_Out_SDL (e);		
#else // Not Android
	case SDL_MOUSEWHEEL:
		if (1) {
			cbool direction = !(e->wheel.y > 0);
			key_scancode_e scancode = direction ? K_MOUSEWHEELDOWN : K_MOUSEWHEELUP;
			Key_Event_Ex (NO_WINDOW_NULL, scancode, /*down */ true, ASCII_0 , UNICODE_0, shiftbits(e));
			Key_Event_Ex (NO_WINDOW_NULL, scancode, /*up   */ false, ASCII_0 , UNICODE_0, shiftbits(e));
		}
		return true; // handled

	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
//	case SDL_MOUSEMOTION:	// Yes mouse move is in here  (no longer!)

		if (1) {
			int buttons, shift, x, y;

			getmousebits (e, &buttons, &shift, &x, &y);
			Input_Mouse_Button_Event (buttons, /*Is mouse move = Mar 5 2018*/ false, x, y);
		}
		return true; // handled

#ifdef INPUT_RELATIVE
	case SDL_MOUSEMOTION:
		input_accum_x += e->motion.xrel;
		input_accum_y += e->motion.yrel;
		{
			// https://wiki.libsdl.org/SDL_GetMouseState
			// March 5 2018
			int x, y, buttons, shift;
			getmousebits (e, &buttons, &shift, &x, &y); // getmousebits #defined as Platform_SDL_Input_GetMouseBits
			Input_Mouse_Button_Event (buttons, /*Is mouse move = Mar 5 2018*/ true, x, y);
		}
		return true;
#endif // INPUT_RELATIVE
#endif // !PLATFORM_ANDROID - which uses SDL touch events instead.

	case SDL_CONTROLLERDEVICEADDED:
		if (joy_active_instance_idx == -1)
		{
			joy_active_joystick = SDL_GameControllerOpen(e->cdevice.which);
			if (joy_active_joystick == NULL)
				Con_DPrintLinef ("Couldn't open game controller");
			else
			{
				SDL_Joystick *joy = SDL_GameControllerGetJoystick(joy_active_joystick);
				joy_active_instance_idx = SDL_JoystickInstanceID(joy);
			}
		}
		else
			Con_DPrintf("Ignoring SDL_CONTROLLERDEVICEADDED");
		return true;

	case SDL_CONTROLLERDEVICEREMOVED:
		#pragma message ("This would be better firing joystick key releases")
		if (joy_active_instance_idx != -1 && e->cdevice.which == joy_active_instance_idx)
		{
			SDL_GameControllerClose(joy_active_joystick);
			joy_active_joystick = NULL;
			joy_active_instance_idx = -1;
		}
		else
			Con_DPrintLinef ("Ignoring SDL_CONTROLLERDEVICEREMOVED");
		return true;

	case SDL_CONTROLLERDEVICEREMAPPED:
		Con_DPrintLinef ("Ignoring SDL_CONTROLLERDEVICEREMAPPED");
		return true;

//	case WM_MOUSELEAVE:	 // Mouse cancel
	//	return true; // handled

	default:
		return false; // not handled
	}
}



void Input_Local_Joy_AdvancedUpdate_f (lparse_t *unused)
{
}

cbool Input_Local_Joystick_Startup (void)
{	
		
	//alert ("Input_Local_Joystick_Startup");
	if (COM_CheckParm("-nojoy")) {
		joy_disabled_commandline = true;
		return false;
	}
	
	if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) == -1) {
		Con_WarningLinef ("could not initialize SDL Game Controller");
		joy_disabled_commandline = true; // Can this even happen?
		return false;
	}

	{
		int nummappings = 0;
		char controllerdb[MAX_OSPATH];
		c_snprintf1 (controllerdb, "%s/gamecontrollerdb.txt", com_basedir);
		//alert ("Controllerdb %s", controllerdb); 
		nummappings = SDL_GameControllerAddMappingsFromFile(controllerdb);
		if (nummappings > 0)
			Con_SafePrintLinef ("%d mappings loaded from gamecontrollerdb.txt", nummappings);
	}


	{  int n; for (n = 0; n < SDL_NumJoysticks(); n++) {
		const char *joyname = SDL_JoystickNameForIndex(n);

		if (!SDL_IsGameController(n)) {
			// REJECTED - NO MAPPINGS - Not recognized as a game controller
			Con_WarningLinef ("joystick missing controller mappings: %s", joyname != NULL ? joyname : "NULL" );
			continue;
		}

		{
			const char *controllername = SDL_GameControllerNameForIndex(n);
			SDL_GameController *gamecontroller = SDL_GameControllerOpen(n);

			if (!gamecontroller) {
				// REJECTED - COULDN'T OPEN
				Con_WarningLinef ("failed to open controller: %s", controllername != NULL ? controllername : "NULL");
				continue;
			}

			Con_SafePrintLinef ("detected controller: %s", controllername != NULL ? controllername : "NULL");
			
			joy_active_instance_idx = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(gamecontroller));
			joy_active_joystick = gamecontroller;

			// ACCEPTED
			return true;;
		}
	}}

	//alert ("Done looping");
	return false; // NOTHING FOUND!
}


cbool Input_Local_Joystick_Read (void)
{
    return false; // I guess?
}


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
IN_Commands

Emit key events for game controller buttons, including emulated buttons for analog sticks/triggers
================
*/
void Input_Local_Joystick_Commands (void)
{
	joyaxisstate_t newaxisstate;

	const float stickthreshold = 0.9;
	const float triggerthreshold = joy_deadzone_trigger.value;
	
	//__android_log_print( ANDROID_LOG_INFO, CORE_ANDROID_LOG_TAG, "Input_Local_Joystick_Commands");
	if (joy_disabled_commandline || !joy_enable.value || !joy_active_joystick)
		return;

#if 1 // This doesn't seem to be doing anything on Android
	// emit key events for controller buttons
	{ int n; for (n = 0; n < SDL_CONTROLLER_BUTTON_MAX; n++) {
		cbool newstate = SDL_GameControllerGetButton (joy_active_joystick, (SDL_GameControllerButton) n);// HERONED
		cbool oldstate = joy_buttonstate.buttondown[n];
		
		joy_buttonstate.buttondown[n] = newstate;
		
		// NOTE: This can cause a reentrant call of IN_Commands, via SCR_ModalMessage when confirming a new game.
		IN_JoyKeyEvent(oldstate, newstate, IN_KeyForControllerButton((SDL_GameControllerButton)n), &joy_buttontimer[n]);
	}}
#endif
	
	{ int n; for (n = 0; n < SDL_CONTROLLER_AXIS_MAX; n++) {

		newaxisstate.axisvalue[n] = SDL_GameControllerGetAxis(joy_active_joystick, (SDL_GameControllerAxis)n) / 32768.0f;
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
//		IN_JoyKeyEvent(joy_axisstate.axisvalue[2] > triggerthreshold,  newaxisstate.axisvalue[SDL_CONTROLLER_AXIS_TRIGGERRIGHT] > triggerthreshold, K_RTRIGGER, &joy_emulatedkeytimer[8]);
//		IN_JoyKeyEvent(joy_axisstate.axisvalue[2] > triggerthreshold, newaxisstate.axisvalue[SDL_CONTROLLER_AXIS_TRIGGERLEFT] > triggerthreshold, K_LTRIGGER, &joy_emulatedkeytimer[9]);

		IN_JoyKeyEvent(joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_TRIGGERLEFT] > triggerthreshold,  newaxisstate.axisvalue[SDL_CONTROLLER_AXIS_TRIGGERLEFT] > triggerthreshold, K_LTRIGGER, &joy_emulatedkeytimer[8]);
		IN_JoyKeyEvent(joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_TRIGGERRIGHT] > triggerthreshold, newaxisstate.axisvalue[SDL_CONTROLLER_AXIS_TRIGGERRIGHT] > triggerthreshold, K_RTRIGGER, &joy_emulatedkeytimer[9]);

#endif
//	}
#endif

	joy_axisstate = newaxisstate;

}

/*
================
IN_JoyMove
================
*/
void Input_Local_Joystick_Move (usercmd_t *cmd)
{
	float	speed;
	joyaxis_t moveRaw, moveDeadzone, moveEased;
	joyaxis_t lookRaw, lookDeadzone, lookEased;

	if (joy_disabled_commandline || !joy_enable.value || !joy_active_joystick)
		return;
	
	moveRaw.x = joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_LEFTX];
	moveRaw.y = joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_LEFTY];
	lookRaw.x = joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_RIGHTX];
	lookRaw.y = joy_axisstate.axisvalue[SDL_CONTROLLER_AXIS_RIGHTY];
	
	//	__android_log_print( ANDROID_LOG_INFO, CORE_ANDROID_LOG_TAG, "Input_Local_Joystick_move: %g %g %g %g", moveRaw.x, moveRaw.y, lookRaw.x, lookRaw.y);

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

	CL_BoundViewPitch (cl.viewangles);
}


void Input_Local_Joystick_Shutdown (void)
{
	SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
}


void Input_Local_Init (void)
{
//    Not needed?
//    if (SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL) == -1)
//        Con_WarningLinef ("Warning: SDL_EnableKeyRepeat() failed.");
}

void Input_Local_Shutdown (void)
{
	Input_Local_Joystick_Shutdown ();
}


// Baker: On Windows these might not only be key events.
void Input_Local_SendKeyEvents (void)
{
	int			done = 0;
	SDL_Event	my_event;

	while (SDL_PollEvent (&my_event)) {
		int should_quit = Session_Dispatch (&my_event);
		if (should_quit) {
			System_Quit();
		}
	}
    
	SDL_Delay(1);
}

// Baker: Stops drag flag on Mac (when activation is received by a mouseclick on title bar and user drags it.
//  On Windows do this too.
void Input_Local_Deactivate (void)
{


}

#endif // !PLATFORM_ANDROID
#endif // CORE_SDL

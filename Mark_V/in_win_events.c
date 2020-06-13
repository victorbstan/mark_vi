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

///////////////////////////////////////////////////////////////////////////////
//  PLATFORM: DISPATCH.  AT LEAST THE DEFAULT ONE.
///////////////////////////////////////////////////////////////////////////////




static holy_key = 0;
cbool WIN_IN_ReadInputMessages (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	cbool down = false;

	int button_bits = 0;

	switch (msg)
	{
//
// Input events
//

// Keyboard character emission
	case WM_CHAR:
//		if (holy_key) {
//			Con_DPrintLinef ("Rejected a scan code %c %d", in_range(32, wparam, 126) ? wparam : 0, wparam);
//		}
//		else 
		{
#pragma message ("TODO: wparam is UTF-16 or UCS2 or something")
			int unicode = wparam;
			int ascii 	= in_range (32, unicode, 126) ? unicode : 0;
			// We do not do control characters here.
			Key_Event_Ex (NO_WINDOW_NULL, SCANCODE_0, true, ascii, unicode, shiftbits());  // ascii, unicode, shift
		}
		return true;

//
// Keyboard scancode emission
//

	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		down = true;  // Fall through ...

	case WM_SYSKEYUP:
	case WM_KEYUP:
		if (in_keymap.value /* off = no*/) { // 1005
			// Looking for 96.
			int key = ((int) lparam >> 16) & 255;
//			Con_PrintLinef ("lparam %d key %d", lparam, key);
			// Top position, right?
			#define TOP_LEFT_HARDWARE_KEY_41 41 // The top left key on the keyboard.  US = tilde, varies with some keyboards.
			if (key == TOP_LEFT_HARDWARE_KEY_41) {
				Key_Event_Ex (NO_WINDOW_NULL, KEYMAP_HARDWARE_TILDE_511, down, /*should_emit*/  ASCII_0, /*unicode*/ UNICODE_0, shiftbits());
				Shell_Input_ResetDeadKeys ();
				holy_key = true;
				return true;
			}
			holy_key = false;
			// Otherwise keep going ...
		}
			

		if (1) {
			int theirs				= wparam;
			key_scancode_e scancode	= keymap[theirs][2];
			char instruction		= keymap[theirs][3]; // 0, E, R, N
			int ascii				= scancode;
			cbool extended			= !!((lparam >> 24) & 1);
			int is_right;
			cbool should_emit 		= down && !in_range (32, scancode, 126); // Control keys only?
			// Key combos aren't working?
			switch (instruction) {
			default:			Host_Error ("Unknown instruction");
			case_break  0 :		/* No special instruction*/
//								if (in_range(32, scancode, 126)) // Commented out testing stuff.
//									ascii = ascii; 
			case_break 'L':		if (!extended) {
									scancode = keymap[theirs][4]; // Use numpad scancode.
								}
			case_break 'E':		// If extended, emit the same but "ours" translation scancode is specified value
								if (extended) {
									scancode = keymap[theirs][4]; // Use numpad scancode.
								}

			case_break 'N':		// No emission even though scancode is in 128-255 range.
								ascii = 0;
			case_break 'R':		// Determine left vs. right
								switch (theirs) {
								default:				Host_Error ("Unknown platform scan code");
								case_break VK_CONTROL:	is_right = (lparam >> 24) & 1;			scancode = ascii = is_right ? K_RCTRL :  K_LCTRL;
								case_break VK_MENU:		is_right = (lparam >> 24) & 1;			scancode = ascii = is_right ? K_RALT :   K_LALT;
								case_break VK_SHIFT:	is_right = (lparam >> 16 & 255) == 54;	scancode = ascii = is_right ? K_RSHIFT : K_LSHIFT;
								}
			}

			// Scan code event:  Send the scancode.
			//                   ascii -- only if down AND it is a control character.
			//                      Because there is no ascii emission on up.
			//                            And ascii emission on down is ONLY for control characters
			//                              because the keymap event in WM_CHAR does the emit for ascii.
			//                   Even if we aren't using keymapping, the function being called would need to use the scancode only.
			//                   Because this is a scancode event here, that does ascii for control characters.
			if (scancode)
				Key_Event_Ex (NO_WINDOW_NULL, scancode, down, /*should_emit*/  ASCII_0, /*unicode*/ UNICODE_0, shiftbits());
		}

		return true; // handled

	case WM_MOUSEWHEEL:
		if (1) {
			cbool direction = (short)HIWORD(wparam) > 0; // short.  Not int.
			key_scancode_e scancode = direction ? K_MOUSEWHEELUP : K_MOUSEWHEELDOWN;
			// Can't remember if we are supposed to send the ascii or not for something with no ascii
			// I think NO, but not 100%
			// Figure out the right way make the function fatal error if receives something out of line
			// So we can catch stupidity.
			Key_Event_Ex (NO_WINDOW_NULL, scancode, true, ASCII_0 , UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
			Key_Event_Ex (NO_WINDOW_NULL, scancode, false,   ASCII_0 , UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
		}
		return true; // handled

//
// This point forward we want to discard the messages if directinput is active.  Returning true.
// inps.have_mouse should do the trick.
//

	case WM_MOUSEMOVE:
		if (focus0.phave_mouse && *(focus0.phave_mouse))
			return true; // If we have the mouse captured we are not interested in mouse moves.  HANDLED, BUT IGNORED.
	
		//Con_SafePrintLinef ("in_win x, y mousemove = %d, %d", LOWORD(lparam) /*x*/, HIWORD(lparam) /*y*/);
		// We don't have the mouse ... fall through.

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_LBUTTONUP:

	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:

		// Baker: We sometimes get WM_MOUSEFIRST or WM_MOUSEMOVE messages that may be the result
		// Of cursor capture.
		if (1) {
			int buttons, shift, x, y;
			
			getmousebits (wparam, lparam, &buttons, &shift, &x, &y);
			
			Input_Mouse_Button_Event (buttons, (msg == WM_MOUSEMOVE), LOWORD(lparam) /*x*/, HIWORD(lparam) /*y*/);
		}
		return true; // handled

//
//
//

	default:
		return false; // not handled
	}
}



// Baker: On Windows these might not only be key events.
void Input_Local_SendKeyEvents (void)
{
    MSG        msg;

	while (PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE))
	{
	// we always update if there are any event, even if we're paused
		scr_skipupdate = 0;

		if (!GetMessage (&msg, NULL, 0, 0))
			System_Quit ();

      	TranslateMessage (&msg);
      	DispatchMessage (&msg);
	}
}

// Baker: Stops drag flag on Mac (when activation is received by a mouseclick on title bar and user drags it.
//  On Windows do this too.
void Input_Local_Deactivate (void)
{


}







void Input_Local_Shutdown (void)
{

}

void Input_Local_Init (void)
{

}


#endif // PLATFORM_WINDOWS

#endif // !CORE_SDL
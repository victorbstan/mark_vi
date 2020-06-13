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
// input.c -- input


#include "quakedef.h"
#include "input.h"		// Courtesy
#include "menu_local.h"	// Privledged access

#ifdef INPUT_RELATIVE
	int input_accum_x, input_accum_y;
	/// #define Input_Local_Mouse_Cursor_SetPos // Baker: wicked evil ... but not just yet
#endif // INPUT_RELATIVE

// How do we convert this to Quake?

keyvalue_t key_scancodes_table [KEYMAP_KEYNAMES_118] = {  // April 2018 - By all accounts this appears to be unused!!!
	{ "BACKSPACE",      K_BACKSPACE         },
	{ "TAB",            K_TAB               },
	{ "ENTER",          K_ENTER             },
	{ "ESCAPE",         K_ESCAPE            },
	{ "SPACE",          K_SPACE             },
	{ "SEMICOLON",      K_SEMICOLON         },
	{ "TILDE",          K_GRAVE             },
	{ "LCTRL",          K_LCTRL             },
	{ "RCTRL",          K_RCTRL             },
	{ "LALT",           K_LALT              },
	{ "RALT",           K_RALT              },
	{ "LSHIFT",         K_LSHIFT            },
	{ "RSHIFT",         K_RSHIFT            },
	{ "LWIN",           K_LWIN              },
	{ "RWIN",           K_RWIN              },
	{ "MENU",           K_MENU              },
	{ "CAPSLOCK",       K_CAPSLOCK          },
	{ "NUMLOCK",        K_NUMLOCK           }, // Is same as keypad numlock or is different key?
	{ "SCROLLLOCK",     K_SCROLLLOCK        },
	{ "PAUSE",          K_PAUSE             },
	{ "PRINTSCREEN",    K_PRINTSCREEN       },
	{ "INS",            K_INSERT            },
	{ "DEL",            K_DELETE            },
	{ "LEFTARROW",      K_LEFTARROW         },
	{ "RIGHTARROW",     K_RIGHTARROW        },
	{ "UPARROW",        K_UPARROW           },
	{ "DOWNARROW",      K_DOWNARROW         },
	{ "PGUP",           K_PAGEUP            },
	{ "PGDN",           K_PAGEDOWN          },
	{ "HOME",           K_HOME              },
	{ "END",            K_END               },
	{ "F1",             K_F1                },
	{ "F2",             K_F2                },
	{ "F3",             K_F3                },
	{ "F4",             K_F4                },
	{ "F5",             K_F5                },
	{ "F6",             K_F6                },
	{ "F7",             K_F7                },
	{ "F8",             K_F8                },
	{ "F9",             K_F9                },
	{ "F10",            K_F10               },
	{ "F11",            K_F11               },
	{ "F12",            K_F12               },

	{ "KP_0",           K_NUMPAD_0          },	// Emission
	{ "KP_1",           K_NUMPAD_1          },
	{ "KP_2",           K_NUMPAD_2          },
	{ "KP_3",           K_NUMPAD_3          },
	{ "KP_4",           K_NUMPAD_4          },
	{ "KP_5",           K_NUMPAD_5          },
	{ "KP_6",           K_NUMPAD_6          },
	{ "KP_7",           K_NUMPAD_7          },
	{ "KP_8",           K_NUMPAD_8          },
	{ "KP_9",           K_NUMPAD_9          },
	{ "KP_MULTIPLY",    K_NUMPAD_MULTIPLY   },
	{ "KP_PLUS",        K_NUMPAD_PLUS       },
	{ "KP_SEPARATOR",   K_NUMPAD_SEPARATOR  },
	{ "KP_MINUS",       K_NUMPAD_MINUS      },
	{ "KP_PERIOD",      K_NUMPAD_PERIOD     },
	{ "KP_DIVIDE",      K_NUMPAD_DIVIDE     },
#if 0 // Disallow, I think - Build 1010
	{ "KP_ENTER",		K_NUMPAD_ENTER		},	// Emission!!!!
#endif
	{ "MOUSE1",         K_MOUSE1            },
	{ "MOUSE2",         K_MOUSE2            },
	{ "MOUSE3",         K_MOUSE3            },
	{ "MOUSE4",         K_MOUSE4            },
	{ "MOUSE5",         K_MOUSE5            },

	{ "MWHEELUP",       K_MOUSEWHEELUP      },
	{ "MWHEELDOWN",     K_MOUSEWHEELDOWN    },
	{ "JOY1",           K_JOY1              },
	{ "JOY2",           K_JOY2              },
	{ "JOY3",           K_JOY3              },
	{ "JOY4",           K_JOY4              },
	{ "AUX1",           K_AUX1              },
	{ "AUX2",           K_AUX2              },
	{ "AUX3",           K_AUX3              },
	{ "AUX4",           K_AUX4              },
	{ "AUX5",           K_AUX5              },
	{ "AUX6",           K_AUX6              },
	{ "AUX7",           K_AUX7              },
	{ "AUX8",           K_AUX8              },
	{ "AUX9",           K_AUX9              },
	{ "AUX10",          K_AUX10             },
	{ "AUX11",          K_AUX11             },
	{ "AUX12",          K_AUX12             },
	{ "AUX13",          K_AUX13             },
	{ "AUX14",          K_AUX14             },
	{ "AUX15",          K_AUX15             },
	{ "AUX16",          K_AUX16             },
	{ "AUX17",          K_AUX17             },
	{ "AUX18",          K_AUX18             },
	{ "AUX19",          K_AUX19             },
	{ "AUX20",          K_AUX20             },
	{ "AUX21",          K_AUX21             },
	{ "AUX22",          K_AUX22             },
	{ "AUX23",          K_AUX23             },
	{ "AUX24",          K_AUX24             },
	{ "AUX25",          K_AUX25             },
	{ "AUX26",          K_AUX26             },
	{ "AUX27",          K_AUX27             },
	{ "AUX28",          K_AUX28             },
	{ "AUX29",          K_AUX29             },
	{ "AUX30",          K_AUX30             },
	{ "AUX31",          K_AUX31             },
	{ "AUX32",          K_AUX32             },

	{ "LTHUMB",			K_LTHUMB            },
	{ "RTHUMB",			K_RTHUMB            },
	{ "LSHOULDER",		K_LSHOULDER         },
	{ "RSHOULDER",		K_RSHOULDER         },
	{ "ABUTTON",		K_ABUTTON           },
	{ "BBUTTON",		K_BBUTTON           },
	{ "XBUTTON",		K_XBUTTON           },
	{ "YBUTTON",		K_YBUTTON           },
	{ "LTRIGGER",		K_LTRIGGER          },
	{ "RTRIGGER",		K_RTRIGGER          },
NULL, 0}; // Null term


void Input_Force_CenterView_f (lparse_t *unnused) { cl.viewangles[PITCH] = 0; }


typedef enum { ENUM_FORCE_INT_GCC_ (input_state)
	input_none,
	input_have_keyboard,
	input_have_mouse_keyboard,
	input_have_windowskey,
} input_state_t;

typedef struct
{
	input_state_t	current_state;
	cbool			initialized, have_mouse, have_keyboard;
	cbool			disabled_windows_key;

// Internals
	mrect_t			mouse_clip_screen_rect;
	int				mouse_accum_x, mouse_accum_y;
	int				mouse_old_button_state;
} inp_info_t;


#define MRECT_PRINT(_x) _x.left, _x.top, _x.right, _x.bottom, _x.center_x, _x.center_y
enum { GET_IT_1 = 1, LOSE_IT_2 = 2 };


keyvalue_t input_state_text [] =
{
	KEYVALUE (input_none),
	KEYVALUE (input_have_keyboard),
	KEYVALUE (input_have_mouse_keyboard),
NULL, 0 };  // NULL termination

static inp_info_t inps;


void Input_Info_f (void)
{
	Con_PrintLinef ("IN Info ...");
	Con_PrintLinef ("%-25s :  %s", "current_state", KeyValue_GetKeyString (input_state_text, inps.current_state) );
	Con_PrintLinef ("%-25s :  %d", "initialized", inps.initialized);
	Con_PrintLinef ("%-25s :  %d", "have_mouse", inps.have_mouse);
	Con_PrintLinef ("%-25s :  %d", "have_keyboard", inps.have_keyboard);
	Con_PrintLinef ("%-25s :  %d", "disabled_windows_key", inps.disabled_windows_key);
	Con_PrintLinef ("%-25s :  (%d, %d)-(%d, %d) center: %d, %d", "mouse_clip_screen_rect:", MRECT_PRINT(inps.mouse_clip_screen_rect) );
	Con_PrintLinef ("%-25s :  %d", "mouse_accum_x", inps.mouse_accum_x);
	Con_PrintLinef ("%-25s :  %d", "mouse_accum_y", inps.mouse_accum_y);
	Con_PrintLinef ("%-25s :  %d", "mouse_old_button_state", inps.mouse_old_button_state);
}

#pragma message ("OS X mouse input has to be purely event oriented, we can't just nab the screen at any given time")
#ifdef PLATFORM_OSX
void Input_Think (void) { }
#else
void Input_Think (void)
{
	cbool			is_touch_screen_active = vid.touch_screen_active /*<-- assignment*/ = (vid.is_mobile || vid_touchscreen.value);

	input_state_t	newstate = (inps.initialized && vid.ActiveApp && !vid.Minimized && !vid.Hidden) ? input_have_keyboard : input_none;

	// We want windowed mousegrab in windowed mode if we are in the menu with keygrab
	//    OR
	//  We are not paused.  We don't have the console up and we are in-game or message mode2.

	// For full-screen.
	cbool			windowed_mouse_grab = 
						(key_dest == key_menu && sMenu.keys_bind_grab) 
						|| 
						(!is_touch_screen_active && !cl.paused && !console1.forcedup && isin2 (key_dest, key_game, key_message));

	// Any time we want the mouse in windowed mode, we want in fullscreen mode.
	// Plus
	// How does "No mouse" figure into this? FULL IGNORE SOMEHOW, HAHA
	cbool			mouse_grab =  windowed_mouse_grab; // ||
//			(vid.screen.type == MODESTATE_FULLSCREEN && key_dest != key_menu && !vid_tablet_mode.value;

	cbool			disable_windows_key = input_have_keyboard && vid.screen.type == MODESTATE_FULLSCREEN;
//	cbool			can_mouse_track = inps.initialized && !vid.Minimized && !vid.Hidden) && dont have mouse

	if (disable_windows_key != inps.disabled_windows_key)
	{
		switch (disable_windows_key)
		{
		case true:
			if (vid.system_enhanced_keys) Shell_Input_KeyBoard_Capture (true /*capture*/, false /*act on stickey*/, vid.screen.type == MODESTATE_FULLSCREEN /*act on windows key*/);
			break;
		case false:
			if (vid.system_enhanced_keys) Shell_Input_KeyBoard_Capture (false /*capture*/, false /*act on stickey*/, vid.screen.type == MODESTATE_FULLSCREEN /*act on windows key*/);
			break;
		}

		inps.disabled_windows_key = disable_windows_key;
	}

	// newstate upgrades from should have "keyboard" to should have "mouse"
	// If the key_dest is game or we are binding keys in the menu
	if (newstate == input_have_keyboard && mouse_grab && in_nomouse.value == 0 && vid.nomouse == 0)
		newstate = input_have_mouse_keyboard; // Upgrade from Keyboard to Keyboard + mouse.

#if 0
	Con_PrintLinef ("current_state: %s (init %d active %d mini %d)", Keypair_String (input_state_text, inps.current_state),
		inps.initialized, vid.ActiveApp, vid.Minimized);
#endif


	{
		int		old_touch_screen_game_controls  = vid.touch_screen_game_controls_on;
		//cbool	new_touch_screen_game_controls	= vid.touch_screen_active && key_dest == key_game && !cl.intermission && !cls.demoplayback;
		cbool	bnew_touch_screen_game_controls	= vid.touch_screen_active && vid.mobile_bluetooth_keyboard_entry == false && key_dest == key_game && !cl.intermission && (!cls.demoplayback || vid_touchscreen.value > 1) && !console1.forcedup;
		int		new_touch_screen_game_controls  = bnew_touch_screen_game_controls ? (vid_touchscreen.value >=2 ? 2 : 1) : 0;

		cbool	did_change	= new_touch_screen_game_controls != old_touch_screen_game_controls;

		if (did_change) {
			// Cancel any touch_stamps, release any buttons
			touch_button_t *canvas_surface = &focus0.touch_buttons[touch_button_canvas_0];

			if (canvas_surface->touch_stamp) {
				Key_Game_Button_UnPair (canvas_surface);
				Key_Game_Canvas_PressUp ();
			}

			// We check even showscores
			{ int n; for (n = touch_button_forward_left_1; n < touch_button_COUNT; n++) {
				touch_button_t *btn = &focus0.touch_buttons[n];
				if (btn->touch_stamp) {
					Key_Game_Button_Action (n, /*is down*/ false); // Perform release action.
					Key_Game_Button_UnPair (btn);
				}
			}}

		}

		vid.touch_screen_game_controls_on = new_touch_screen_game_controls;
	
	}

	if (newstate != inps.current_state)
	{ // New state.
		char	mouse_action	= ( newstate == input_have_mouse_keyboard && inps.have_mouse == false) ? GET_IT_1 :  (( newstate != input_have_mouse_keyboard && inps.have_mouse == true) ? LOSE_IT_2 : 0);
		char	keyboard_action = ( newstate != input_none && inps.have_keyboard == false) ? GET_IT_1 :  (( newstate == input_none && inps.have_keyboard == true) ? LOSE_IT_2 : 0);

#if 0
		Con_PrintLinef ("State change");
#endif

		switch (keyboard_action)
		{
		case GET_IT_1:
			// Sticky keys
			if (vid.system_enhanced_keys) Shell_Input_KeyBoard_Capture (true /*capture*/, true /*act on stickey*/, vid.screen.type == MODESTATE_FULLSCREEN /*act on windows key*/);

			inps.have_keyboard = true;
			break;

		case LOSE_IT_2:
			// Note we still need our key ups when entering the console
			// Sticky keys, Window key reenabled

			if (vid.system_enhanced_keys) Shell_Input_KeyBoard_Capture (false, true /*act on stickey*/, vid.screen.type == MODESTATE_FULLSCREEN);
			// Key ups

			inps.have_keyboard = false;
			break;
		}

		switch (mouse_action)
		{
		case GET_IT_1:

			// Load window screen coords to mouse_clip_screen_rect
			// And clip the mouse cursor to that area
			Input_Local_Update_Mouse_Clip_Region_Think (&inps.mouse_clip_screen_rect);

			// Hide the mouse cursor and attach it
			Input_Local_Capture_Mouse (true);

			// Center the mouse on-screen
			Input_Local_Mouse_Cursor_SetPos (inps.mouse_clip_screen_rect.center_x, inps.mouse_clip_screen_rect.center_y);

			// Clear movement accumulation
			inps.mouse_accum_x = inps.mouse_accum_y = 0;

			inps.have_mouse = true;
			break;

		case LOSE_IT_2:
			// Baker: We have to release the mouse buttons because we can no longer receive
			// mouse up events.
			Key_Release_Mouse_Buttons ();

			// Release it somewhere out of the way
			Input_Local_Mouse_Cursor_SetPos (inps.mouse_clip_screen_rect.right - 80, inps.mouse_clip_screen_rect.top + 80);

			// Release the mouse and show the cursor.  Also unclips mouse.
			Input_Local_Capture_Mouse (false);

			// Clear movement accumulation and buttons
			inps.mouse_accum_x = inps.mouse_accum_y = inps.mouse_old_button_state = 0;

			inps.have_mouse = false;
			break;
		}
		inps.current_state = newstate;
	}

	if (inps.have_mouse && Input_Local_Update_Mouse_Clip_Region_Think (&inps.mouse_clip_screen_rect) == true)
	{
		// Re-center the mouse cursor and clear mouse accumulation
		Input_Local_Mouse_Cursor_SetPos (inps.mouse_clip_screen_rect.center_x, inps.mouse_clip_screen_rect.center_y);
		inps.mouse_accum_x = inps.mouse_accum_y = 0;
	}

	// End of function
}



focus_part_e Focus_Part (hotspot_menu_item_t *hs, int left, int top)
{
	switch (hs->hotspottype) {
	default: System_Error ("Focus_Part Unknown hotspot type");
	case_break hotspottype_vscroll:
		if (RECT_HIT_Y(hs->r_thumb, top))		return focus_part_thumb;
		if (RECT_HIT_Y(hs->r_button1, top))		return focus_part_button1;
		if (RECT_HIT_Y(hs->r_button2, top))		return focus_part_button2;
		if (RECT_HIT_Y(hs->r_track1, top))		return focus_part_track1;
		if (RECT_HIT_Y(hs->r_track2, top))		return focus_part_track2;
		Con_PrintLinef ("Bad focus on vscroll, unknown part");	
	case_break hotspottype_hscroll:
		if (RECT_HIT_X(hs->r_thumb, left))		return focus_part_thumb;
		if (RECT_HIT_X(hs->r_button1, left))	return focus_part_button1;
		if (RECT_HIT_X(hs->r_button2, left))	return focus_part_button2;
		if (RECT_HIT_X(hs->r_track1, left))		return focus_part_track1;
		if (RECT_HIT_X(hs->r_track2, left))		return focus_part_track2;
		Con_PrintLinef ("Bad focus on vscroll, unknown part");	
	}
	return 0;
}






// Two things:  What if something like a Surface Pro and touch but touchscreen mode off?
//     Answer: Should collide with the menu items but not the game buttons
//

void Touch_Action (void *touch_stamp, mouseaction_e mouseaction, int rawx, int rawy)
{
//	fprintf (stderr, "%d %d", rawx, rawy);
	int stamp_idx		  = (mouseaction == mouseaction_down_0) ? Touch_Stamp_New_Idx (touch_stamp) : Touch_Stamp_Find_Idx (touch_stamp);
	touch_stamp_t *touche = &focus0.touch_stamps[stamp_idx];

#if defined(WINQUAKE_RENDERER_SUPPORT) && !defined(PLATFORM_IOS) // Yes android needs to do this.
	rawx /= vid.stretch_x;
	rawy /= vid.stretch_y;
#endif // WINQUAKE_RENDERER_SUPPORT but not IOS


//	Con_PrintLinef("Touch %d state %d", (int)touch_stamp, (int)mouseaction);

	Key_Surface_Action (touche, mouseaction, rawx, rawy);
	
	if (isin2(mouseaction, mouseaction_up, mouseaction_cancelled )) {
		Touch_Stamp_Kill_Idx (touche->idx);
	}
}

// All mouse presses come through here.
// Warning: For mouse move we receive button bits!
static void Key_Mouse_Button (key_scancode_e scancode, mouseaction_e mouseaction, cbool havemouse, int rawx, int rawy)
{
	switch (havemouse) {
	default: /*true*/	// Captured mouse game mode.
		if (mouseaction == mouseaction_down_0)	Key_Event_Ex (NO_WINDOW_NULL, scancode, true,  ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
		if (mouseaction == mouseaction_up)		Key_Event_Ex (NO_WINDOW_NULL, scancode, false, ASCII_0, UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
	
	case_break false:
		
		// We don't have mouse
		#pragma message ("Make sure key binds still work ok")
		
		if (scancode == K_MOUSE1 || (mouseaction == mouseaction_move && (scancode & mousebuttonbits_mouse1_bit_1))/**/ ) {
			#define MOUSE1_AS_TOUCH_STAMP ((void *) K_MOUSE1) // Faking it just needs an id
			int stamp_idx		  = (mouseaction == mouseaction_down_0) ? Touch_Stamp_New_Idx (MOUSE1_AS_TOUCH_STAMP) : Touch_Stamp_Find_Idx (MOUSE1_AS_TOUCH_STAMP);
			touch_stamp_t *touche = &focus0.touch_stamps[stamp_idx];
			
			if (mouseaction == mouseaction_move)
				mouseaction = mouseaction_move;

			Key_Surface_Action (touche, mouseaction, /*havemouse,*/ rawx, rawy); // Green light!

			if (isin2(mouseaction, mouseaction_up, mouseaction_cancelled )) {
				Touch_Stamp_Kill_Idx (touche->idx);
			}

			break;
		}


		if (mouseaction == mouseaction_move) {
			// K_MOUSE2, K_MOUSE3 or even no mouse button down ... these can perform as mousemoves.
			Key_Surface_Action (/*touch stamp*/ NULL, mouseaction, /*havemouse,*/ rawx, rawy); // Green light!
			break;
		}

		if (scancode == K_MOUSE2 && key_dest == key_menu && mouseaction == mouseaction_up) {
			if (realtime > sMenu.keys_bind_grab_off_time + 0.25) {
				M_KeyPress (K_ESCAPE, NO_HOTSPOT_HIT_NEG1);
			}
			break;
		}

// In Mark V 1036:
//    If we were in full screen, mouse is captured in menu ALWAYS during or not key grab.
//    If we were in windowed, mouse is never captured in menu EXCEPT during key grab.  Key Grab captures the mouse.
//		Make sure mouse buttons can be captured.
//		Make sure moues3 can be captured.
// goal: avoid mouse2 exit.  mgrabmousetime > 0.25 secs?
// what about mouse1?
// 

// problem #2 table full?  WHEN?
// .... IS IT JUST CUSTOMIZE CONTROLS?

// problem #3 - ensure touches cannot become key binds, right?

	}					
}


// A touch event may turning into a mouse1 event.
// DOWN: can turn into a K_MOUSE1 event IF all the following is true.
//       1) Doesn't hit a touch button or canvas.  Another way of saying not in "active game".
//       2) Number of touches down is 0.
// MOVE: can turn into a K_MOUSE1 even IF all the following is true.
//       1) Number of touches down is 1.  (We should be that touch.)
//       2) No touch button is down (And there better not be).
// UP:   1) Number of touches down is 1.  (We should be that touch)
//       2) No touch button is down
// Therefore, by extension ..
//  Mouse Check - the above conditions
//  Touch Check - key_game with not demo/intermission.  We never check for ESCAPE in touch system.
// Touch system is in-game system.





// Main entry point for all mouse presses from system specific input.  Final canvas conversion of X/Y occurs here.
void Input_Mouse_Button_Event (mousebuttonbits_e mouse_button_bits, cbool is_mousemove, int rawx, int rawy)
{
	cbool is_attached = inps.have_mouse || (key_dest == key_menu && sMenu.keys_bind_grab); // Baker Mar 12 2018 - I am skeptical inps.have_mouse didn't already consider menu grab mouse.
	
#ifdef WINQUAKE_RENDERER_SUPPORT
	if (!vid.is_screen_portrait) {
		rawx /= vid.stretch_x;
		rawy /= vid.stretch_y;
	}
#endif // WINQUAKE_RENDERER_SUPPORT

	if (host_initialized) //inps.have_mouse || (key_dest == key_menu && sMenu.keys_bind_grab) )
	{  // perform button actions
		{ int n; for (n = 0 ; n < INPUT_NUM_MOUSE_BUTTONS ; n ++) {
			int button_bit = (1 << n);
			cbool button_pressed  =  (mouse_button_bits & button_bit) && !(inps.mouse_old_button_state & button_bit);
			cbool button_released = !(mouse_button_bits & button_bit) &&  (inps.mouse_old_button_state & button_bit);
			cbool nochange		  =  ((mouse_button_bits & button_bit) == (inps.mouse_old_button_state & button_bit));
			int reference		  = inps.mouse_old_button_state;

			// Modal message is a clusterfuck, we can re-enter here recursively and also have statements not taking effect.
			// A potential for chaos and double fire is possible.
			// What should really happen is the modal dialog should be its own thing like key_menu
			// But for simplicity, maybe should be own menu item that knows where it came from?
			// That creates a separate problem that keyboard binds could fire making us leave that screen.
			// Still, the quit menu is quite effective.
			// However, the quit menu is non-blocking so we would need to feed it a function to run on a YES.

			// mousecancel?
			if (button_pressed)		Key_Mouse_Button (K_MOUSE1 + n,       mouseaction_down_0, is_attached, rawx, rawy);
			if (is_mousemove)		Key_Mouse_Button (mouse_button_bits,  mouseaction_move,  is_attached, rawx, rawy); // Touches are independent, though.
			if (button_released)	Key_Mouse_Button (K_MOUSE1 + n,       mouseaction_up,    is_attached, rawx, rawy);
		}}
		inps.mouse_old_button_state = mouse_button_bits;
	}
}

// This re-centers the mouse, so it means more than simple build-up of accumulation alone.
// S_ExtraUpdate calls this, which is called several places.
// The only other caller is Input_Mouse_Move (us!)
// In perfect work, something like DirectInput would always be used making this unnecessary.
void Input_Mouse_Accumulate (void)
{
	static int last_key_dest;
#ifndef INPUT_RELATIVE
	int new_mouse_x, new_mouse_y;
#endif // INPUT_RELATIVE

	Input_Think ();

	if (inps.have_mouse)
	{
		cbool nuke_mouse_accum = false;

		// Special cases: fullscreen doesn't release mouse so doesn't clear accum
		// when entering/exiting the console.  I consider those input artifacts.  Also
		// we simply don't want accum from fullscreen if not key_dest == key_game.
		if (vid.screen.type == MODESTATE_FULLSCREEN)
		{
			if (cl.paused)
				nuke_mouse_accum = true;
			else
			{
				cbool in_game_or_message = (key_dest == key_game || key_dest == key_message);
				cbool was_in_game_or_message = (last_key_dest == key_game || last_key_dest == key_message);
				cbool entered_game_or_message = in_game_or_message && !was_in_game_or_message;
				if (entered_game_or_message || !in_game_or_message)
					nuke_mouse_accum = true;
			}
		}

#ifdef INPUT_RELATIVE // In particular SDL uses this
		inps.mouse_accum_x += input_accum_x; input_accum_x = 0;
		inps.mouse_accum_y += input_accum_y; input_accum_y = 0;
#else // ^^^ INPUT_RELATIVE
		Input_Local_Mouse_Cursor_GetPos (&new_mouse_x, &new_mouse_y, false); // GetCursorPos (&current_pos);

		inps.mouse_accum_x += new_mouse_x - inps.mouse_clip_screen_rect.center_x;
		inps.mouse_accum_y += new_mouse_y - inps.mouse_clip_screen_rect.center_y;
		// Re-center the mouse cursor
		Input_Local_Mouse_Cursor_SetPos (inps.mouse_clip_screen_rect.center_x, inps.mouse_clip_screen_rect.center_y);
#endif // !INPUT_RELATIVE


		if (nuke_mouse_accum)
			inps.mouse_accum_x = inps.mouse_accum_y = 0;
	}
	last_key_dest = key_dest;
}

void Input_Mouse_Move (usercmd_t *cmd)
{
	Input_Mouse_Accumulate ();

	if (inps.mouse_accum_x || inps.mouse_accum_y)
	{
		int	mouse_x = inps.mouse_accum_x *= sensitivity.value;
		int mouse_y = inps.mouse_accum_y *= sensitivity.value;
	// add mouse X/Y movement to cmd
		if ( (in_strafe.state & 1) || (lookstrafe.value && MOUSELOOK_ACTIVE ))
			cmd->sidemove += m_side.value * mouse_x;
		else cl.viewangles[YAW] -= m_yaw.value * mouse_x;

		if (MOUSELOOK_ACTIVE)
			View_StopPitchDrift ();

		if ( MOUSELOOK_ACTIVE && !(in_strafe.state & 1))
		{
			cl.viewangles[PITCH] += m_pitch.value * mouse_y;

			CL_BoundViewPitch (cl.viewangles);
		}
		else
		{
			if ((in_strafe.state & 1) && cl.noclip_anglehack)
				cmd->upmove -= m_forward.value * mouse_y;
			else cmd->forwardmove -= m_forward.value * mouse_y;
		}
		inps.mouse_accum_x = inps.mouse_accum_y = 0;
	}
}
#endif // !PLATFORM_OSX

void Input_Move (usercmd_t *cmd)
{
#ifdef DIRECT_INPUT_QUAKE
	if (inps.have_mouse) Input_DirectInput_Mouse_Move (inps.mouse_old_button_state);
#endif // DIRECT_INPUT_QUAKE
    Input_Mouse_Move (cmd);
    Input_Joystick_Move (cmd);
}


cbool joy_avail;

/*
===========
IN_JoyMove
===========
*/

//#ifdef _WIN32
//#include "winquake.h"
//#endif // _WIN32

void Input_Joystick_Move (usercmd_t *cmd)
{
	Input_Local_Joystick_Move (cmd);
}

void Input_Commands (void)
{
#if defined(PLATFORM_OSX) || defined(PLATFORM_IOS)
void Key_Console_Repeats (void);
	Key_Console_Repeats ();
#endif // PLATFORM_OSX
	Input_Local_Joystick_Commands ();

}

void Input_Joystick_Init (void)
{
	Input_Local_Joystick_Startup();

	// We are always adding the commands and cvars now.
	Cmd_AddCommands (Input_Joystick_Init);
}

void Input_Init (void)
{
	focus0.phave_mouse = &inps.have_mouse;
	Cmd_AddCommands (Input_Init);

#pragma message ("Baker: Implement m_filter on Windows")

	// This doesn't work because the config.cfg will be read and just override it.
	// Now we do it earlier -- see video startup --- no we had to bail on that, now we use command line parm
	//if (COM_CheckParm ("-nomouse"))
	//	Cvar_SetValueQuick (&in_nomouse, 1);
#ifdef DIRECT_INPUT_QUAKE
	Input_Local_Mouse_Init ();
#endif // DIRECT_INPUT_QUAKE

//	if (!COM_CheckParm ("-nojoy"))
	Input_Joystick_Init ();

	Input_Local_Init (); // Mac

	inps.initialized = true;
	Input_Think ();
	Con_PrintLinef ("Input initialized");
}

void Input_Shutdown (void)
{
	Input_Local_Shutdown (); // Mac

	inps.initialized = false;
	Input_Think (); // Will shut everything off
}

void Input_System_Enhanced_Keys_Changed (cvar_t *var)
{
	// Too late, remember this reads early in SND_Read_Early_Cvars
	if (host_post_initialized) {
		Con_PrintLinef ("System enhanced keys changed.  Requires engine restart to take effect.");
	}
}


// We have a touch.  It may be up, down or move.
// We came from system or we are a mouse event without the mouse (menu, console, messagemode, demoplay, intermission)

#pragma message ("I don't want hover highlight in touchscreen mode?  Or just on true mobile?")
#pragma message ("Lets do some pointless dragging and see what happens?")

// To support proper mouse, we must allow for mousemoves without a button.
void Key_Surface_Action (touch_stamp_t *touche, mouseaction_e mouseaction, int x, int y)
{	

#pragma message ("MAKE SURE MOUSE 2 ESC WORKS!")
	#pragma message ("MAKE SURE MOUSE 2 ESC WORKS!")
	#pragma message ("MAKE SURE MOUSE 2 ESC WORKS!")
	#pragma message ("MAKE SURE MOUSE 2 ESC WORKS!")
	#pragma message ("MAKE SURE MOUSE 2 ESC WORKS!")
	#pragma message ("MAKE SURE MOUSE 2 ESC WORKS!")

	// Find or assign a touch stamp idx.
	
	if (focus0.touch_buttons_num_down > 1)
		focus0.touch_buttons_num_down = focus0.touch_buttons_num_down;
	
//	if (mouseaction == mouseaction_move && stamp_idx == IDX_NOT_FOUND_NEG1) {
//		Con_PrintLinef ("Unwanted mouse move");
//		return;
//	}
	
	// 1. TOUCH SCREEN GAME CONTROLS
	if (vid.touch_screen_game_controls_on && touche != NULL /*exclude mouse move with no button from going here*/) {
		// Active game canvas and tablet mode

		if (mouseaction == mouseaction_down_0) {
			Touch_Button_Hit_Set_Stamp (touche, x, y);
			switch (touche->touch_button_idx) {
			default:							Key_Game_Button_Action(touche->touch_button_idx, /*is down*/ true);
			case_break IDX_NOT_FOUND_NEG1:		// Did not hit a button or the touch was rejected (probably being 2nd touch on same button)
			case_break touch_button_canvas_0:	Key_Game_Canvas_PressDown(x, y);
			}												
		}

		else if (mouseaction == mouseaction_move) {
			switch (touche->touch_button_idx) {
			default:							// The buttons do not do moves
			case_break IDX_NOT_FOUND_NEG1:		// Not paired.  Do nothing.
			case_break touch_button_canvas_0:	Key_Game_Canvas_PressMove (x, y); // Canvas performs moves.
			}
		}

		else if (mouseaction == mouseaction_up) {
			
			switch (touche->touch_button_idx) {
			default:							Key_Game_Button_Action (touche->touch_button_idx, /*is down*/ false);												

			case_break IDX_NOT_FOUND_NEG1:		// Check for ESC box collision.  Toggle menu if so.

												if (ESCAPE_BOX_COLLISION(x, y)) {
													if (vid.is_mobile && vid.mobile_bluetooth_keyboard_entry) {
														vid.mobile_bluetooth_keyboard_entry = false;
													}
													M_ToggleMenu_f (NULL);
												}

			case_break touch_button_canvas_0:	Key_Game_Canvas_PressMove (x, y); // Give canvas a final move
												Key_Game_Canvas_PressUp (); // Do the up
			}


			if (touche->touch_button_idx != IDX_NOT_FOUND_NEG1) {
				touch_button_t *btn = &focus0.touch_buttons[touche->touch_button_idx];
				Key_Game_Button_UnPair (btn);
			}

			
		}
		
		else {
			x=x; // Can we end up here?
		
		}

	}


	// 2. ESCAPE Super hotspot always available in touch-screen mode.
	// Also if demoplayback any key brings up menu
	else if (vid.touch_screen_active && ((key_dest == key_game && cls.demoplayback) || ESCAPE_BOX_COLLISION(x, y)) && mouseaction == mouseaction_up) {
		// We do nothing.
		switch (key_dest) {
		default:				// Can't happen
		case_break key_message:	Key_Message (K_ESCAPE);
		case_break key_menu:	M_KeyPress (K_ESCAPE, NO_HOTSPOT_HIT_NEG1);
		case_break key_game:	if (vid.is_mobile && vid.mobile_bluetooth_keyboard_entry) {
									vid.mobile_bluetooth_keyboard_entry = false; 
								}
								M_ToggleMenu_f (NULL); // Mobile entry off
								
		case_break key_console:	M_ToggleMenu_f (NULL);
		}
	}

	// 3. TOUCH SCREEN + CONSOLE tap release brings up
	else if (vid.touch_screen_active && (key_dest == key_console || (key_dest == key_game && console1.forcedup))) {
		if  (mouseaction == mouseaction_up) {
			// Active game canvas and tablet mode
			Mnu_OnScreenKeyboard_PromptText (
				"Console", NULL, /*buffer to fill*/ &history_lines[edit_line][1], /*sizeof buf*/ 24,
				menu_state_None_0
			);
		}
	}

	// 4. TOUCH SCREEN + INTERMISSION TAP tap release brings up
	else if (vid.touch_screen_active && key_dest == key_game && cl.intermission) {
		if  (mouseaction == mouseaction_up) {
			// Active game canvas and tablet mode
			Cbuf_AddTextLinef ("+jump; wait;");
			Cbuf_AddTextLinef ("-jump");
		}
	}


	// 4. NOT IN MENU and not any of the above -- so just get out.
	else if (key_dest != key_menu) {
		// Get out.  Nothing to do for message mode or game.

	}

	// 5. MENU
	else {
		hotspot_menu_group_t *f = &hotspot_menu_group;
		// If we don't have focus on something, check for a menu nav super-hotspot.
		if (vid.touch_screen_active && mouseaction == mouseaction_up && !f->focus) {
			if (sMenu.menu_state == menu_state_NameMaker) {
				if (RECT_HIT (focus0.menu_up, x, y))	{	M_KeyPress (K_UPARROW,		NO_HOTSPOT_HIT_NEG1); return; } // Return is ok right?
				if (RECT_HIT (focus0.menu_enter, x, y))	{	M_KeyPress (K_ENTER,		NO_HOTSPOT_HIT_NEG1); return; } // Return is ok right?
				if (RECT_HIT (focus0.menu_down, x, y))	{	M_KeyPress (K_DOWNARROW,	NO_HOTSPOT_HIT_NEG1); return; } // Return is ok right?
				if (RECT_HIT (focus0.menu_left, x, y))	{	M_KeyPress (K_LEFTARROW,	NO_HOTSPOT_HIT_NEG1); return; } // Return is ok right?
				if (RECT_HIT (focus0.menu_right, x, y))	{	M_KeyPress (K_RIGHTARROW,	NO_HOTSPOT_HIT_NEG1); return; } // Return is ok right?
				if (RECT_HIT (focus0.menu_backsp, x, y)){	M_KeyPress (K_BACKSPACE,	NO_HOTSPOT_HIT_NEG1); return; } // Return is ok right?
			}

			else {
				if (RECT_HIT (focus0.menu_up, x, y))	{	M_KeyPress (K_UPARROW,		NO_HOTSPOT_HIT_NEG1); return; } // Return is ok right?
				if (RECT_HIT (focus0.menu_enter, x, y))	{	M_KeyPress (K_ENTER,		NO_HOTSPOT_HIT_NEG1); return; } // Return is ok right?
				if (RECT_HIT (focus0.menu_down, x, y))	{	M_KeyPress (K_DOWNARROW,	NO_HOTSPOT_HIT_NEG1); return; } // Return is ok right?
				if (RECT_HIT (focus0.menu_left, x, y))	{	M_KeyPress (K_LEFTARROW,	NO_HOTSPOT_HIT_NEG1); return; } // Return is ok right?
			}
		}

		{			
			hotspot_menu_item_t *hit = menux[sMenu.menu_state].hover = 
				Menu_Hotspot_Refresh_For_Mouse (x, y, &menux[sMenu.menu_state].hoverx, &menux[sMenu.menu_state].hovery);		
			int left = menux[sMenu.menu_state].hoverx, top  = menux[sMenu.menu_state].hovery;
	
			// We do this here to degrade it to a mousemove
			if (mouseaction == mouseaction_up && f->focus) {
				// No need to degrade it to a mousemove.  Above should hover it ok.
#if defined(GLQUAKE_RENDERER_SUPPORT) && !defined (DIRECT3D9_WRAPPER) // May 7 2018
				if (f->focus->is_gamma) {
					// Kick the gamma
					extern double gamma_timer;
					gamma_timer = realtime + 0.35;
				}
#endif // GLQUAKE + !DIRECT3D9_WRAPPER
				f->focus = NULL; // Wipe it
				return; // GET OUT
			}
	
	
			switch (mouseaction) 
			{
			default: break;
			case_break mouseaction_move:
	
				//
				// MOUSEMOVE - Mostly scrollbars
				//
	
				// CHECK FOR FOCUS HERE!
				if (!f->focus)
					break; // We have no focus
	
				switch (f->focus->hotspottype) {
				default:							break; // If not a vscroll we do nothing special.
				case_break hotspottype_vscroll: {
					int local_top;
	
					if (f->focus_part != focus_part_thumb)
						break; // Do not want!
					
					f->focus_move_y				= top;
					f->focus_move_thumb_offset	= f->focus_down_thumb_offset; // 7 or such
					f->focus_move_thumb_top		= top - f->focus_down_thumb_offset; // Thumb is higher than top.  This is screen Y.
					local_top					= f->focus_move_thumb_top - f->focus_down_track_top; // 0 is true top.
					local_top					= CLAMP (0, local_top, f->focus_down_track_usable - 1);
					f->focus_move_thumb_top		= f->focus_down_track_top + local_top;
	
					// Pencil in event
					f->focus_event				= focus_event_thumb_position;
					//f->focus_event_msgtime_ext	= 0;
					f->focus_event_thumb_top	= local_top;
					//VID_Set_Window_Title ("New thumb top relative %d", f->focus_event_thumb_top);
				
				} // End of vscroll
				case_break hotspottype_hscroll: {
					int local_top;
	
					if (f->focus_part != focus_part_thumb)
						break; // Do not want!
					
					f->focus_move_y				= left;
					f->focus_move_thumb_offset	= f->focus_down_thumb_offset; // 7 or such
					f->focus_move_thumb_top		= left - f->focus_down_thumb_offset; // Thumb is higher than top.  This is screen Y.
					local_top					= f->focus_move_thumb_top - f->focus_down_track_top; // 0 is true top.
					local_top					= CLAMP (0, local_top, f->focus_down_track_usable - 1);
					f->focus_move_thumb_top		= f->focus_down_track_top + local_top;
	
					// Pencil in event
					f->focus_event				= focus_event_thumb_position;
					//f->focus_event_msgtime_ext	= 0;
					f->focus_event_thumb_top	= local_top;
					//VID_Set_Window_Title ("New thumb top relative %d", f->focus_event_thumb_top);
				
				} // End of hscroll
				} // End of hotspot switch
	
			case_break mouseaction_up: {
	
	#pragma message ("This should be also if we have no current focus, like scrollbar in progress")
				//
				// MOUSEUP - About everything
				//
				// CHECK FOR FOCUS HERE!
				hotspottype_e hotspottype = hit ? hit->hotspottype : hotspottype_none_0;
	
	
				switch (hotspottype) {
				default:	break; // I guess
				case_break hotspottype_none_0:		// No hotspot was hit
				case_break hotspottype_inert:		// An inert (placeholder) was hit.  We do nothing.
				case_break hotspottype_toggle:		M_KeyPress (K_MOUSE1, hit->idx); // 2 or more choices.  We aren't supporting reverse.
				case_break hotspottype_slider:		
													//if (in_range (hit->slider_start, menux[sMenu.menu_state].hoverx, hit->slider_end)) {
													//	key_scancode_e emitk = 0;
													//	if      (menux[sMenu.menu_state].hoverx < hit->slider_midstart)	emitk = K_LEFTARROW;
													//	else if (menux[sMenu.menu_state].hoverx < hit->slider_midbeyond)	emitk = 0; // Ignore
													//	else														emitk = K_RIGHTARROW;
													//	if (emitk) 
													//		M_KeyPress (emitk, hit->idx); // Slider.					Responds to left and right.
													//}
				
				case_break hotspottype_button:		//
													M_KeyPress (K_MOUSE1, hit->idx);	// Execute.					Example: Single Player: Reset Defaults
				case_break hotspottype_button_line:	//if (scancode == K_MOUSE1) M_KeyPress (K_ENTER, hit->idx);	// Execute.					Example: Single Player: Reset Defaults
													M_KeyPress (K_MOUSE1, hit->idx);	// Execute.					Example: Single Player: Reset Defaults
	
				
				case_break hotspottype_listitem:	// if (scancode == K_MOUSE1) M_KeyPress (K_ENTER, HS_LIST_LISTINDEX_ENCODE(hit->listindex) ); // A list item
													M_KeyPress (K_MOUSE1, HS_LIST_LISTINDEX_ENCODE(hit->listindex) ); // A list item
				case_break hotspottype_text:		// Do nothing.  Used.
				case_break hotspottype_textbutton:	//M_KeyPress (K_ENTER, hit->idx); // Used for "your name" to go to name maker.
													M_KeyPress (K_MOUSE1, hit->idx); // Used for "your name" to go to name maker.
													
				case_break hotspottype_screen:		M_KeyPress (K_MOUSE1, hit->idx); // Help menu pages.
				case_break hotspottype_vscroll:		// What about other parts of the scrollbar?  Track2, button1, etc.
													break;
				case_break hotspottype_hscroll:		break;
				} // End of switch hotspottype
	
			} // End of mouseup
	
			case_break mouseaction_down_0:
				
	#pragma message ("Not only clear hover, but clear is_down?  Do we need to erase the members?")
	#pragma message ("Also keep in mind as we scroll number of hotspots can change!  Perhaps we paint inert ones when a scroll bar is involved?")
				//
				// MOUSEDOWN - Mostly scrollbars
				//
	
				if (!hit)
					break; // Does this get us out?  //goto dont_have_mouse_done; // Not sure what to do.
	
	#ifdef _DEBUG
					if (f->focus == hit) { alert ("Already attached?"); break; }
					if (f->focus != NULL) { alert ("Already have focus?"); break; }
	#endif // _DEBUG
	
				if (isin2(hit->hotspottype, hotspottype_vscroll, hotspottype_hscroll)) {
					f->focus					= hit;
					f->focus_menu_state			= sMenu.menu_state;
					f->focus_idx				= hit->idx;
					f->focus_part				= Focus_Part (hit, left, top);
	
					f->focus_event				= focus_event_none_0;	// Down does not have a message.
					f->focus_event_msgtime_ext	= 0;					// Down does not have a time.
					f->focus_event_thumb_top	= 0;					// For completion.
	
					// Event vars for when move happens.
					f->focus_move_y				= 0;
					f->focus_move_thumb_offset	= 0;
					f->focus_move_thumb_top		= 0;
	
					f->focus_down_possibles		= hit->possible_first_rows;
	
					// Information at the time of down.  Where mouse was.  Thumb top.  The offset by extension.
					switch (hit->hotspottype) {
					default: break; // If not a vscroll we do nothing special.
					case_break hotspottype_vscroll:
						f->focus_down_y				= top;
						f->focus_down_thumb_top		= hit->r_thumb.top;
						f->focus_down_thumb_offset	= RECT_HIT_Y_OFFSET(hit->r_thumb, top);
						f->focus_down_track_top		= hit->r_track.top;
						f->focus_down_track_usable	= hit->r_track.height - hit->r_thumb.height + 1;						
					
					case_break hotspottype_hscroll:
						f->focus_down_y				= left;
						f->focus_down_thumb_top		= hit->r_thumb.left;
						f->focus_down_thumb_offset	= RECT_HIT_X_OFFSET(hit->r_thumb, left);
						f->focus_down_track_top		= hit->r_track.left;
						f->focus_down_track_usable	= hit->r_track.width - hit->r_thumb.width + 1;						
						
					} // End of switch
					//VID_Set_Window_Title ("Focus gained down at %d, thumb %d offset %d", f->focus_down_y, f->focus_down_thumb_top, f->focus_down_thumb_offset);
				} // End of hotspot switch
	
			} // End of if hscroll/vscroll block
		}	
	} // End of key menu

} // End of don't have mouse




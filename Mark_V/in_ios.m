#ifndef CORE_SDL
#include "environment.h"
#ifdef PLATFORM_GUI_IOS

/*
Copyright (C) 2013-2014 Baker

*/
// in_ios.m -- cd



#include "quakedef.h"
#include "iphonequake.h"
#include "input.h"	// Courtesy

///////////////////////////////////////////////////////////////////////////////
//  PLATFORM: DISPATCH.  AT LEAST THE DEFAULT ONE.
///////////////////////////////////////////////////////////////////////////////


int Input_Local_Capture_Mouse (cbool bDoCapture)
{
//	static cbool captured = false;
//
//	if (bDoCapture && !captured)
//	{
////	    SDL_WM_GrabInput(SDL_GRAB_ON);
////	    SDL_SetRelativeMouseMode(SDL_TRUE);
////		ShowCursor (FALSE); // Hides mouse cursor
////		SetCapture (sysplat.mainwindow);	// Captures mouse events
//#ifdef INPUT_RELATIVE
//		SDL_SetRelativeMouseMode (SDL_TRUE);
//#else // ^^^ INPUT_RELATIVE
//		SDL_ShowCursor (SDL_DISABLE /*0 FALSE*/); // Hides mouse cursor
//		SDL_CaptureMouse (SDL_ENABLE /* 1 TRUE*/); //  Use this function to capture the mouse and to track input outside an SDL window.
//#endif // !INPUT_RELATIVE
//		Con_DPrintLinef ("Mouse Captured");
//		captured = true;
//	}
//
//	if (!bDoCapture && captured)
//	{
////	    SDL_WM_GrabInput(SDL_GRAB_OFF);
////	    SDL_SetRelativeMouseMode(SDL_FALSE);
//		//ShowCursor (TRUE); // Hides mouse cursor
//		//ReleaseCapture ();
//		//ClipCursor (NULL); // Can't hurt
//#ifdef INPUT_RELATIVE
//		SDL_SetRelativeMouseMode (SDL_FALSE);
//#else // ^^^ INPUT_RELATIVE
//		SDL_CaptureMouse (SDL_DISABLE /* 0 FALSE*/); //  Use this function to capture the mouse and to track input outside an SDL window.
//		SDL_ShowCursor (SDL_ENABLE /* 1 TRUE*/); // Hides mouse cursor
//#endif // !INPUT_RELATIVE
//		Con_DPrintLinef ("Mouse Released");
//		captured = false;
//	}
//
	return 1; // Accepted
}


cbool Input_Local_Update_Mouse_Clip_Region_Think (mrect_t* mouseregion)
{
//// What's the plan here?
//	mrect_t oldregion = *mouseregion;
//	// This should be the client area ...  does it return negative too I hope?
//	SDL_GetWindowPosition (sysplat.mainwindow, &mouseregion->left, &mouseregion->top);
//	SDL_GetWindowSize (sysplat.mainwindow, &mouseregion->width, &mouseregion->height);
//
//	// What about +1 possibility?
//
//	if (memcmp (mouseregion, &oldregion, sizeof(mrect_t) ) != 0)
//	{  // Changed!
//		mouseregion->bottom = mouseregion->top + mouseregion->height;
//		mouseregion->right = mouseregion->left + mouseregion->width;
//		mouseregion->center_x = (mouseregion->left + mouseregion->right) / 2;
//		mouseregion->center_y = (mouseregion->top + mouseregion->bottom) / 2;
//		//ClipCursor (&windowinfo.rcClient); // SDL can't.  Also if cursor is invisible should Windows even try?  Yeah probably.
//		return true;
//	}
	return false;
}

void Input_Local_Mouse_Cursor_SetPos (int x, int y)
{
////	SetCursorPos (x, y);
//	if (SDL_WarpMouseGlobal (x,y) != 0) // Returns 0 on success.
//		log_debug ("Unable to set mouse position");
}

void Input_Local_Mouse_Cursor_GetPos (required int *px, required int *py, cbool towindow)
{
//	int x, y;
//	if (SDL_GetGlobalMouseState (&x, &y) != 0) // Returns 0 on success
//		log_debug ("Unable to get mouse position");
//
	REQUIRED_ASSIGN (px, -1);
	REQUIRED_ASSIGN (py, -1);  // Let's set -1 here and we will just see what happens
}


//cbool SDLQ_IN_ReadInputMessages (void *_sdl_event)
//{
//	SDL_Event  *e    = _sdl_event;
//	cbool down = false;
//
//	int button_bits = 0;
//
//	switch (e->type) {
////
//// Input events
////
//
//// Keyboard character emission
//	case SDL_TEXTINPUT:
//		if (1) {
//#pragma message ("TODO: emit UTF-16 or UCS2 or something")
//			char *ch; for (ch = e->text.text; *ch; ch++) {
//				int unicode = *ch;
//				int ascii 	= in_range (32, unicode, 126) ? unicode : 0;
//				// We do not do control characters here.
//				Key_Event_Ex (NO_WINDOW_NULL, SCANCODE_0,  /*down*/ true, ascii, unicode, shiftbits (e) ); // Future, convert this to utf-32
//			}
//		}
//		return true;
//
////
//// Keyboard scancode emission
////
//
//	case SDL_KEYDOWN:
//		down = true;  // Fall through ...
//
//	case SDL_KEYUP:
//
//
//		if (1) {
//			int theirs				= e->key.keysym.scancode;
//			key_scancode_e scancode	= keymap[theirs][2];
//			// Keypad HOME, END, etc are screwed for now and won't emit a "keypress" (but will emit a keydown)
//			// Reason?  We don't have a way to know from the scan code whether it should emit a number or a cursor move
//			// And SDL numlock reports wrong.
//			// I'm not going to write a per platform numlock checker to work around this SDL limitation.
//			// But on windows is a simple GetKeyState (VK_NUMLOCK) != 0;
//			cbool should_emit 		= down && (!in_range (32, scancode, 126) && !in_range (K_NUMPAD_0, scancode, K_NUMPAD_DIVIDE));
//			int ascii				= should_emit ? scancode : 0;
//			if (scancode)
//				//Key_Event_Ex (NO_WINDOW_NULL, scancode, down, ascii, /*unicode*/ 0, shiftbits (e) ); // Future, convert this to utf-32
//				Key_Event_Ex (NO_WINDOW_NULL, scancode, down, /*should_emit*/  ASCII_0, /*unicode*/ UNICODE_0, shiftbits (e) ); // Future, convert this to utf-32
//
//		}
//
//		return true; // handled
//
//	case SDL_MOUSEWHEEL:
//		if (1) {
//			cbool direction = !(e->wheel.y > 0);
//			key_scancode_e scancode = direction ? K_MOUSEWHEELDOWN : K_MOUSEWHEELUP;
//			Key_Event_Ex (NO_WINDOW_NULL, scancode, /*down */ true, ASCII_0 , UNICODE_0, shiftbits(e));
//			Key_Event_Ex (NO_WINDOW_NULL, scancode, /*up   */ false, ASCII_0 , UNICODE_0, shiftbits(e));
//		}
//		return true; // handled
//
//	case SDL_MOUSEBUTTONDOWN:
//	case SDL_MOUSEBUTTONUP:
////	case SDL_MOUSEMOTION:	// Yes mouse move is in here
//
//		if (1) {
//			int buttons, shift, x, y;
//
//			getmousebits (e, &buttons, &shift, &x, &y);
//			Input_Mouse_Button_Event (buttons);
//		}
//		return true; // handled
//
//#ifdef INPUT_RELATIVE
//	case SDL_MOUSEMOTION:
//		input_accum_x += e->motion.xrel;
//		input_accum_y += e->motion.yrel;
//		return true;
//#endif // INPUT_RELATIVE
//	
//
////	case WM_MOUSELEAVE:	 // Mouse cancel
//	//	return true; // handled
//
//	default:
//		return false; // not handled
//	}
//}



void Input_Local_Joy_AdvancedUpdate_f (lparse_t *unused)
{
}

cbool Input_Local_Joystick_Startup (void)
{

	return false;
}



void Input_Local_Joystick_Commands (void)
{

}

cbool Input_Local_Joystick_Read (void)
{
    return false; // I guess?
}



void Input_Local_Joystick_Shutdown (void)
{
	// We don't have to do anything here

}


void Input_Local_Init (void)
{
//    Not needed?
//    if (SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL) == -1)
//        Con_WarningLinef ("Warning: SDL_EnableKeyRepeat() failed.");
}

void Input_Local_Shutdown (void)
{
}


// Baker: On Windows these might not only be key events.
void Input_Local_SendKeyEvents (void)
{
//	int			done = 0;
//	SDL_Event	my_event;
//
//	while (SDL_PollEvent (&my_event)) {
//		int should_quit = Session_Dispatch (&my_event);
//		if (should_quit) {
//			System_Quit();
//		}
//	}
//    
//	SDL_Delay(1);
	// UI
	dispatch_queue_t queue = dispatch_queue_create("com.example.MyQueue", NULL);
		dispatch_async(queue, ^{
		  // Do some computation here.

		  // Update UI after computation.
		  dispatch_async(dispatch_get_main_queue(), ^{
			// Update the UI on the main thread.
		  });
	});
//	NSLog (@"k"); // Called by cl_parse.c Install command, console.c Con_NotifyBox, Modal message.
}

// Baker: Stops drag flag on Mac (when activation is received by a mouseclick on title bar and user drags it.
//  On Windows do this too.
void Input_Local_Deactivate (void)
{


}


#endif // PLATFORM_GUI_IOS
#endif // ! CORE_SDL



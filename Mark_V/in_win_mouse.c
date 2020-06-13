#ifndef CORE_SDL
#include "environment.h"
#ifdef PLATFORM_WINDOWS // Has to be here, set by a header


/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2012 John Fitzgibbons and others
Copyright (C) 2009-2014 Baker and others

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; iDirectInputCreateeither version 3
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

#define DIRECTINPUT_VERSION 0x700
#include "quakedef.h"
#include "winquake.h"

#include <dinput.h>

#ifdef DIRECT3D9_WRAPPER
#pragma comment (lib, "dinput8.lib")
#else
#pragma comment (lib, "dinput.lib")
#endif
#pragma comment (lib, "dxguid.lib")

#include "in_win_directinput_overhead.h"

static unsigned int	mstate_di;

static HRESULT			hr;
static HINSTANCE		hInstDI;
static HRESULT			(WINAPI *pDirectInputCreate)(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUT * lplpDirectInput, LPUNKNOWN punkOuter);

typedef struct {
	LPDIRECTINPUT			g_pdi;
	LPDIRECTINPUTDEVICE		g_pMouse;
} di7_t;

static di7_t mdi7;



int Input_Local_Capture_Mouse (cbool bDoCapture)
{
	static cbool captured = false;

	if (bDoCapture && !captured)
	{
		ShowCursor (FALSE); // Hides mouse cursor
		SetCapture (sysplat.mainwindow);	// Captures mouse events
		// Con_DPrintLinef ("Mouse Captured");
		IDirectInputDevice_Acquire (mdi7.g_pMouse);
		captured = true;
	}

	if (!bDoCapture && captured)
	{
		IDirectInputDevice_Unacquire (mdi7.g_pMouse);
		ShowCursor (TRUE); // Hides mouse cursor
		ReleaseCapture ();
		ClipCursor (NULL); // Can't hurt
		// Con_DPrintLinef ("Mouse Released");
		captured = false;
	}

	return 1; // Accepted
}


cbool Input_Local_Update_Mouse_Clip_Region_Think (mrect_t *mouseregion)
{
	mrect_t oldregion = *mouseregion;
	WINDOWINFO windowinfo;
	windowinfo.cbSize = sizeof (WINDOWINFO);
	GetWindowInfo (sysplat.mainwindow, &windowinfo);	// client_area screen coordinates

	// Fill in top left, bottom, right, center
	mouseregion->left = windowinfo.rcClient.left;
	mouseregion->right = windowinfo.rcClient.right;
	mouseregion->bottom = windowinfo.rcClient.bottom;
	mouseregion->top = windowinfo.rcClient.top;

	if (memcmp (mouseregion, &oldregion, sizeof(mrect_t) ) != 0)
	{  // Changed!
		mouseregion->width = mouseregion->right - mouseregion->left;
		mouseregion->height = mouseregion->bottom - mouseregion->top;
		mouseregion->center_x = (mouseregion->left + mouseregion->right) / 2;
		mouseregion->center_y = (mouseregion->top + mouseregion->bottom) / 2;
		ClipCursor (&windowinfo.rcClient);
		return true;
	}
	return false;
}

void Input_Local_Mouse_Cursor_SetPos (int x, int y)
{
	SetCursorPos (x, y);  // We do it sparingly
}

void Input_Local_Mouse_Cursor_GetPos (required int *px, required int *py, cbool towindow)
{
	POINT current_pos;
	GetCursorPos (&current_pos);

	if (towindow)
		ScreenToClient(sysplat.mainwindow, &current_pos);

	REQUIRED_ASSIGN (px, current_pos.x);
	REQUIRED_ASSIGN (py, current_pos.y);
}

void Input_Local_Mouse_Shutdown (void)
{
	// Never actually called?
}


	
void Input_DirectInput_Mouse_Move (int oldstate)
{
	DIDEVICEOBJECTDATA	od;
	int newstate = oldstate;

	while (1) {
		DWORD dwElements = 1;
		
		hr = IDirectInputDevice_GetDeviceData (mdi7.g_pMouse, sizeof(DIDEVICEOBJECTDATA), &od, &dwElements, 0);

		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED)) { 
			Con_DPrintLinef ("DirectInput reacquire");
			IDirectInputDevice_Acquire (mdi7.g_pMouse);
			break;
		}

		// No data available or unable to read data
		if (FAILED(hr) || dwElements == 0) {
			break; 
		}  

		// Look at the element to see what happened
		switch (od.dwOfs) {
		default:				/*nothing*/
		case_break DIMOFS_X:		input_accum_x += od.dwData;
		case_break DIMOFS_Y:		input_accum_y += od.dwData;
		case_break DIMOFS_Z: {		int wheel_key = od.dwData & 0x80 ? K_MOUSEWHEELDOWN : K_MOUSEWHEELUP;
									Key_Event_Ex (NO_WINDOW_NULL, wheel_key, true, ASCII_0 , UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
									Key_Event_Ex (NO_WINDOW_NULL, wheel_key, false,   ASCII_0 , UNICODE_0, CORE_SHIFTBITS_UNREAD_NEG1);
		}
		case_break DIMOFS_BUTTON0:	if (od.dwData & 0x80) newstate |= (1 << 0); else newstate &= ~(1 << 0);
		case_break DIMOFS_BUTTON1:	if (od.dwData & 0x80) newstate |= (1 << 1); else newstate &= ~(1 << 1);
		case_break DIMOFS_BUTTON2:	if (od.dwData & 0x80) newstate |= (1 << 2); else newstate &= ~(1 << 2);
		case_break DIMOFS_BUTTON3:	if (od.dwData & 0x80) newstate |= (1 << 3); else newstate &= ~(1 << 3);
		case_break DIMOFS_BUTTON4:	if (od.dwData & 0x80) newstate |= (1 << 4); else newstate &= ~(1 << 4);					
		} // end switch
	} // End while

	if (newstate != oldstate)
		Input_Mouse_Button_Event (newstate, false /*not mouse move*/, /*xy*/ -1, -1);
	// End loop
}


void Input_Local_Mouse_Init (void)
{
	#define DI8_FLAGS DISCL_EXCLUSIVE | DISCL_FOREGROUND

	if ((hInstDI = LoadLibrary ("dinput.dll")) == NULL)														System_Error ("Couldn't load dinput.dll");	
	if ((pDirectInputCreate = (void *)GetProcAddress(hInstDI,"DirectInputCreateA")) == NULL)				System_Error ("Couldn't get DI proc addr");
	if (FAILED(hr = iDirectInputCreate(sysplat.hInstance, DIRECTINPUT_VERSION, &mdi7.g_pdi, NULL)))			System_Error ("iDirectInputCreate failed");
	if (FAILED(hr = IDirectInput_CreateDevice(mdi7.g_pdi, &GUID_SysMouse, &mdi7.g_pMouse, NULL)))			System_Error ("Couldn't open DI mouse device");
	if (FAILED(hr = IDirectInputDevice_SetDataFormat(mdi7.g_pMouse, &df)))									System_Error ("Couldn't set DI mouse format");
	if (FAILED(hr = IDirectInputDevice_SetCooperativeLevel(mdi7.g_pMouse, sysplat.mainwindow, DI8_FLAGS)))	System_Error ("Couldn't set DI coop level");
	if (FAILED(hr = IDirectInputDevice_SetProperty(mdi7.g_pMouse, DIPROP_BUFFERSIZE, &dipdw.diph)))			System_Error ("Couldn't set DI buffersize");

//	alert ("OK!");
}




#endif // PLATFORM_WINDOWS

#endif // !CORE_SDL

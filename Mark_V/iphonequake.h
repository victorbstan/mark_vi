/*
Copyright (C) 2017 Baker and others

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
// iphonequake.h: iOS-specific Quake header file

#ifndef __IPHONEQUAKE_H__
#define __IPHONEQUAKE_H__
////////////////////////////////////////////////////////////////////
// Actual shared
////////////////////////////////////////////////////////////////////

// General ...
typedef struct
{
	char			lpCmdLine[SYSTEM_STRING_SIZE_1024];	// A nothing-burger
	sys_handle_t	CoreVidWindowObj;					// CoreVidWindow.  We are ARC, must abuse void.
	
	int				statusbar_height;
	float			content_scale;						// Content scale.  For retina display.
	float			fps_desired;						// Set this to 73?
	device_type_e	iphone_device_type;					// Let's face it.
	char			phone_device_description[128];			// Perhaps?

	crect_t			desktop_rect, window_rect;
	int				displayrect;
} sysplat_t;


extern sysplat_t sysplat;

// Ugh.  Since for iphone we can't go all out in "int main", these cannot be static within main ()
// So they must be global.




#endif // __IPHONEQUAKE_H__

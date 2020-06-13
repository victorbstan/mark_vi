
#ifndef CORE_SDL
#if !defined(GLQUAKE) && defined(CORE_GL)  // This is a WinQuake over GL file
#include "environment.h"
#ifdef PLATFORM_GUI_IOS // Header level define, must be here
/*
Copyright (C) 2013-2014 Baker

*/
// vid_ios.m -- video

#include "quakedef.h"
#include "iphonequake.h"
#include "vid.h"	// Courtesy

static void VID_WinQuake_AdjustBuffers (vmode_t *mode);
//static cbool VID_CreateDIB (int width, int height, byte *palette);

//
// miscelleanous init
//

// Runs once.
void VID_Local_Window_PreSetup (void)
{
	//WIN_Vid_Init_Once_CreateClass (); // Class, icon, etc.
}


vmode_t VID_Local_GetDesktopProperties (void)
{
// Only supporting one resolution.  I'm hoping this is landscape but I'm not quite sure.
	vmode_t desktop = {0};
	
	Vid_Display_Properties_Get (NULL, NULL, &desktop.width, &desktop.height, &desktop.bpp);
	desktop.type = MODESTATE_FULLSCREEN;
	return desktop;
}

//
// vsync
//

// Sole caller is GL_CheckExtensions.  No fucking kidding.  It's true.
cbool VID_Local_Vsync_Init (const char *gl_extensions_str)
{
	return false; // It's worse than that!  I think vsync is required.
}

void VID_Local_Vsync_f (cvar_t *var)
{
	VID_Local_Vsync ();
}

void VID_Local_Vsync (void)
{

}

void VID_Local_Multisample_f (cvar_t *var) 
{
	// Unsupported - Wouldn't do anything in WinQuake GL anyway
}


//
// vid modes
//


void VID_Local_AddFullscreenModes (void)
{
	vid.modelist[1] = VID_Local_GetDesktopProperties (); // Set it VID_Local_GetDesktopProperties
	vid.nummodes = 2; // Right?
}


// Baker: begin resize window on the fly
// VID_BeginRendering ->
//   VID_BeginRendering_Resize_Think ->
//    VID_BeginRendering_Resize_Think_Resize_Act
void VID_BeginRendering_Resize_Think_Resize_Act (void)
{
	// We need to fill this somewhere? --> vid.client_window.left
	// Anyway, iPhone doesn't need this right?
	// We aren't going to change video modes "for real"
	// We are going to set one.
	// Since apparently portrait is the default ... ugh
	// We'll have to matrix rotate somewhere, right?
	
	// Portrait to landscape.
	vid.screen.width = vid.desktop.height;
	vid.screen.height = vid.desktop.width;
	
}


// Returns false if need to do GL setup again.
cbool VID_Local_SetMode (int modenum)
{
	vmode_t *p 			= &vid.modelist[modenum];
	
	if (!sysplat.CoreVidWindowObj)
	{
		// iphone - we are going to set this once.
		sys_handle_t drawcontext_dummy; // Because we don't care and our iphone way doesn't use.
		sys_handle_t glcontext_dummy;
		crect_t window_rect = {0};  // I don't think this really matters for iphone
		int request_style = 0; // Doesn't matter?
		
		sysplat.CoreVidWindowObj = Vid_Handle_Create (NULL, ENGINE_NAME, window_rect, request_style, false /*no menu for solo*/, &drawcontext_dummy, &glcontext_dummy);
	
		vid.canalttab = true;
	}

	VID_WinQuake_AdjustBuffers (p);

	return true; // Re-used context ok!
}

//
// in game
//

void VID_Local_SwapBuffers (void)
{	
	// Done automatically.  If we called a function, it would be an empty one for iphone.
	if (sysplat.CoreVidWindowObj) {
		CoreVidWindow * myWindow = (__bridge CoreVidWindow *)sysplat.CoreVidWindowObj;
		[myWindow swapBufferz];
	}
}


void VID_Local_Suspend (cbool bSuspend)
{
	// In windows this would revert the video mode to desktop one or set video mode to Quake's
	// And move a fullscreen window to 0,0 on resume

}

//
// window setup
//


static pixel_t /*byte*/ my_static_byte[1024][512]; // Legal?
static unsigned int /*rgba*/ my_static_rgba[1024][512]; // Legal?
void VID_CreateDIB (int newwidth, int newheight, byte *palette)
{
	if (vid.wingl.texslot) {
		// free and zero related resources.  glDeleteTextures does not zero it out, we must do that.
		eglDeleteTextures (1, &vid.wingl.texslot); vid.wingl.texslot = 0;
		if (vid.wingl.rgbabuffer) {
			//free (vid.wingl.rgbabuffer);
			vid.wingl.rgbabuffer = NULL;
		}
		if (vid.wingl.pixelbytes) {
			//free (vid.wingl.pixelbytes);
			vid.wingl.pixelbytes = vid.buffer = NULL;
			vid.wingl.s1 = vid.wingl.t1 = vid.wingl.width_pow2 = vid.wingl.height_pow2 = 0;
		}
	}

	// Note: vid.conwidth, vid.conheight = newwidth and newheight
	// GLenum glerrorcode = 0; // glGetError
	
	vid.wingl.w				= vid.rowbytes = newwidth;															// Must set vid.rowbytes
	vid.wingl.h				= newheight;
	vid.wingl.width_pow2	= 1024; //Image_Power_Of_Two_Size (newwidth);
	vid.wingl.height_pow2	= 512;  //Image_Power_Of_Two_Size (newheight);

	//vid.wingl.numpels		= vid.wingl.w * vid.wingl.h;
	vid.wingl.numpels		= vid.wingl.width_pow2 * vid.wingl.height_pow2; // 1024 x 512

	vid.wingl.s1			= newwidth  / (float)vid.wingl.width_pow2;
	vid.wingl.t1			= newheight / (float)vid.wingl.height_pow2;

	if (!vid.wingl.texslot) {
		eglGenTextures (1, &vid.wingl.texslot);
GL_ErrorPrint;
		if (!vid.wingl.texslot)
			System_Error ("No texture slot for WinQuake GL");
	}
	
	vid.wingl.rgbabuffer	= my_static_rgba; //calloc (vid.wingl.numpels, sizeof(unsigned));
	vid.wingl.pixelbytes	= vid.buffer = my_static_byte; //calloc (vid.wingl.numpels, sizeof(pixel_t) /*which is byte*/ );		// Must set vid.buffer
GL_ErrorPrint;
	eglBindTexture		(GL_TEXTURE_2D, vid.wingl.texslot);
GL_ErrorPrint;
	eglEnable			(GL_TEXTURE_2D);
GL_ErrorPrint;
	eglTexParameteri	(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
GL_ErrorPrint;
	eglTexParameteri	(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
GL_ErrorPrint;
	eglTexParameteri	(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
GL_ErrorPrint;
	eglTexParameteri	(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
GL_ErrorPrint;
	eglTexImage2D		(GL_TEXTURE_2D, /* mip level */ 0, GL_RGBA, vid.wingl.width_pow2, vid.wingl.height_pow2, /* border */0, GL_RGBA, GL_UNSIGNED_BYTE, /* pixels */ vid.wingl.rgbabuffer);
GL_ErrorPrint;
	// Presumably we have a buffer
}


//
// window teardown
//

void VID_Local_Window_Renderer_Teardown (int destroy, cbool reset_video_mode)
{
	// iPhone
	// We won't do anything here for now.
	// Would be interesting to add video restart.
	// Vid_Handle_Destroy
}






static void VID_WinQuake_AdjustBuffers (vmode_t *p)
{
	// Uses screen.

	// If the following doesn't scream r_misc.c I don't know what does?

	// Find best integral factor, set both the x and the y.  This wants to be 1.  But a giant mode like 6000 x 2000 would generate 2.

	for (vid.stretch_x = 1; p->width  / vid.stretch_x > WINQUAKE_MAX_WIDTH_3000 ; vid.stretch_x ++);
	for (vid.stretch_y = 1; p->height / vid.stretch_y > WINQUAKE_MAX_HEIGHT_1080; vid.stretch_y ++);

	vid.stretch_old_cvar_val = (int)vid_sw_stretch.value; // This isn't the actual stretch, but the cvar value attempted.
	// Ok we need to validate this.
	// Let's say I want 4.  I can't have 4 in 640x480.  /320  /240  highx = (int)(p->width / 320);

	vid.stretch_x = vid.stretch_y = c_max (vid.stretch_x, vid.stretch_y); // Take the larger of the 2.  Lowest it can be.
	{
		int high_x   = (int)(p->width  / 320);
		int high_y   = (int)(p->height / 240);
		int high_any = c_min (high_x, high_y);

		//int stretch_try = vid.stretch_old_cvar_val;
#if defined(PLATFORM_IOS) // Remember this is an IOS specific file right now ...
		int stretch_try = CLAMP(2, vid.stretch_old_cvar_val, 2);
#elif 1 // WINQUAKE-GL EXCEPTION since it is so fucking slow ...
		int stretch_try = CLAMP(1, vid.stretch_old_cvar_val, 2);
#else
		int stretch_try = CLAMP(0, vid.stretch_old_cvar_val, 2);
#endif
		
		switch (stretch_try) {
		case 0:	stretch_try = 1; break;
		case 2:	stretch_try = 9999; break;
		case 1:	stretch_try = (int)(high_any / 2.0 + 0.5); break;
		}

		if (stretch_try > high_any)
			stretch_try = high_any;

		if (stretch_try < vid.stretch_x)
			stretch_try = vid.stretch_x;

		vid.stretch_x = vid.stretch_y = stretch_try;
	}
	
	vid.conwidth  = p->width  / vid.stretch_x;
	vid.conheight  = p->height  / vid.stretch_y;

	vid.aspect = ((float) vid.conwidth / (float) vid.conheight) * (320.0 / 240.0); // toxic

// UH?  NO!! 	VID_Local_Window_Renderer_Teardown (TEARDOWN_NO_DELETE_GL_CONTEXT_0, false /*do not reset video mode*/); // restart the DIB

	VID_CreateDIB (vid.conwidth, vid.conheight, vid.curpal);

	VID_WinQuake_AllocBuffers_D_InitCaches (vid.conwidth, vid.conheight); // It never returns false. 


}

//
//
//
// Equivalent of swap buffers
//
//
//


void TexQuad_Draw (float s1, float t1, float left, float top, float right, float bottom)
{
	const GLfloat quad_texcoords[] = {
		/* top    */ 0,  0, 
		/* right: */ s1, 0, 
		/* bottom */ s1, t1, 
		/* left:  */ 0,  t1,
	};

	GLfloat verts2d [] =
	{
		left,  top,
		right, top,
		right, bottom,
		left,  bottom,
	};
//	GLfloat verts2d [] =
//	{
//		left_0,  top_0,
//		right_1, top_0,
//		right_1, bottom_1,
//		left_0,  bottom_1,
//	};

	#define stride_0 0 // Lets not get cute with stride for WinGL
	
	glVertexPointer(2, GL_FLOAT, stride_0, verts2d);
	glTexCoordPointer(2, GL_FLOAT, stride_0, quad_texcoords);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Draw_Rect2D (float left, float top, float bottom, float right)
{
	GLfloat verts2d [] =
	{
		left,  top,
		right, top,
		right, bottom,
		left,  bottom,
	};
	
	glDisableClientState	(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState		(GL_VERTEX_ARRAY);
	glVertexPointer			(2, GL_FLOAT, 0, verts2d);
	glDrawArrays			(GL_TRIANGLE_FAN, 0, 4);
}

void CGL_Triangle_Test_Texture (GLuint ts, float s1, float t1)
{
	// Clear buffer
	glClearColor			(0, 1, 1, 1);
	glClear					(GL_COLOR_BUFFER_BIT);

	// Matrix
	if (vid.is_mobile && vid.is_screen_portrait)
		eglViewport			(0, 0, vid.desktop.width, vid.desktop.height);// * sysplat.content_scale, vid.screen.height * sysplat.content_scale);
	else
		eglViewport			(0, 0, vid.screen.height, vid.screen.width);// * sysplat.content_scale, vid.screen.height * sysplat.content_scale);
	glMatrixMode			(GL_PROJECTION);
	glLoadIdentity			();
	
	if (vid.is_screen_portrait)
		eglRotatef			(90, 0, 0, 1);
	eglOrtho				(0, 1, 1, 0, -99999, 999999);
	glMatrixMode			(GL_MODELVIEW);
	glLoadIdentity			();
	{
		sysplat.content_scale	= [[UIScreen mainScreen] scale];
CGRect screenBounds = [[UIScreen mainScreen] bounds];
		int vp[4];
		glGetIntegerv(GL_VIEWPORT, vp);
		glGetIntegerv(GL_VIEWPORT, vp);
	}
	// set 2s

	glDisable				(GL_CULL_FACE);
	glDisable				(GL_DEPTH_TEST);
	glDisable				(GL_BLEND);
	glTexEnvf				(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	const GLfloat verts2d[] = {0, 0, 1, 0, 1, 1, 0, 1};

	const GLfloat typical_quad_texcoords[] = {
		/* top    */ 0,   0,
		/* right: */ s1,  0,
		/* bottom */ s1,  t1,
		/* left:  */ 0,   t1,
	};
	

	glEnable				(GL_TEXTURE_2D);
	glBindTexture			(GL_TEXTURE_2D, ts);	// Draw the front face
	glEnableClientState		(GL_TEXTURE_COORD_ARRAY);

	glEnableClientState		(GL_VERTEX_ARRAY);
	glVertexPointer			(2, GL_FLOAT, 0, verts2d);
	glTexCoordPointer		(2, GL_FLOAT, 0, typical_quad_texcoords);
	glDrawArrays			(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState	(GL_VERTEX_ARRAY);
}



// Called exclusively by vid.c VID_SwapBuffers
void VID_Update (vrect_t *rects)
{
	// For now, we are going to lazily update the whole thing.
    if (!vid.wingl.rgbabuffer)
        return; // Don't have one.
	
	if (1) {
		// copy the rendered scene to the texture buffer:
		//#pragma message ("There may be room for optimization here in the future.  Then again POW2?")
		int n; for (n = 0; n < vid.wingl.numpels; n ++) {
			vid.wingl.rgbabuffer[n] = vid.wingl.rgbapal[vid.wingl.pixelbytes[n]];
		}
	}
	eglBindTexture		(GL_TEXTURE_2D, vid.wingl.texslot);
GL_ErrorPrint;
	eglTexSubImage2D	(GL_TEXTURE_2D, 0 /*level*/, /*offset x, y */ 0, 0, vid.wingl.w, vid.wingl.h, GL_RGBA, GL_UNSIGNED_BYTE, vid.wingl.rgbabuffer);


	CGL_Triangle_Test_Texture (vid.wingl.texslot, vid.wingl.s1, vid.wingl.t1);
	return;
	
	
	// Upload

GL_ErrorPrint;
//	eglViewport			(0, 0, vid.screen.width, vid.screen.height);
GL_ErrorPrint;
	eglMatrixMode		(GL_PROJECTION);
GL_ErrorPrint;
	eglLoadIdentity		();
GL_ErrorPrint;
	eglOrtho			(0, 1, 1, 0, -1, 1); // Left right bottom top, near far
GL_ErrorPrint;
	// We need a rotation in here.
	eglMatrixMode		(GL_MODELVIEW);
GL_ErrorPrint;
	eglLoadIdentity		();
	eglClearColor		(1, 0, 0, 0);
	eglClear			(GL_COLOR_BUFFER_BIT);
	
	if (0){
		glColor4f		(0,1,0,1);
		glDisable (GL_TEXTURE_2D);
		Draw_Rect2D (0,0,1,1);
		
	}
	else {
	
	eglEnable			(GL_TEXTURE_2D);
	eglBindTexture		(GL_TEXTURE_2D, vid.wingl.texslot);
GL_ErrorPrint;
	TexQuad_Draw		(vid.wingl.s1, vid.wingl.t1, 0, 1, 0, 1);
}
	// Done.  Still need to flip
GL_ErrorPrint;
	NSLog (@"Flipped");
}


//
//
//
// Palette Set
//
//
//

// Startup/Gamedir change initiates this.  MH Windows WinQuake has to play with the palette here.  All WinQuake implementations must generate alpha50 map.
void VID_Local_Modify_Palette (byte *palette)
{
	// Although everyone needs to generate an alphamap table.
#ifdef WINQUAKE_QBISM_ALPHAMAP
	R_WinQuake_Generate_Alpha50_Map (vid.alpha50map);
#endif // WINQUAKE_QBISM_ALPHAMAP
}

// This gets called by vid.c - SetMode and ShiftPalette(View_UpdateBlend)
void VID_Local_SetPalette (byte *palette)
{
	int c;
	for (c = 0; c < PALETTE_COLORS_256; c ++) {
        byte red				= palette[c * 3 + 0];
        byte green				= palette[c * 3 + 1];
        byte blue				= palette[c * 3 + 2];
        byte alpha				= 0xFF;
        unsigned color			= (red <<  0) + (green <<  8) + (blue << 16) + (alpha << 24);
        vid.wingl.rgbapal[c]	= color;
    }
}


//
//  Window
//


void _VID_Local_Set_Window_Title (const char *text)
{
// Can ios even do this?  Hmmmm.
/*	
	if (!sysplat.mainwindow)
		return;

	#pragma message ("Let's slam this into vid.c and call vidco set window caption or something.  Please!")
	SetWindowText (sysplat.mainwindow, text);
*/
}


void VID_Local_Shutdown (void)
{
	VID_Local_Window_Renderer_Teardown (TEARDOWN_FULL_1, true /*reset video mode*/);
}


// This function gets called before anything happens
void VID_Local_Init (void)
{
	VID_Renderer_Setup (); // Hooks up our GL functions
// Early
#ifdef WINQUAKE_RENDERER_SUPPORT
	VID_Palette_NewGame ();
#endif // WINQUAKE_RENDERER_SUPPORT


}


#endif // PLATFORM_GUI_IOS
#endif // !GLQUAKE && CORE_GL /*win thru gl*/  This is a WinQuake over GL file
#endif // ! CORE_SDL

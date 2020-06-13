#include "environment.h"
#if defined(GLQUAKE) && defined(PLATFORM_ANDROID)

/*
Copyright (C) 2009-2013 Baker

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
// gl_renderer_x.c


#include "quakedef.h"
#include "gl_renderer.h" // Courtesy

floatsamx_t		_ray_base[256 * 1024]; // 262144
floatsamx_t		*ray;

unsigned short	_idx_base[MAX_UNSIGNED_SHORT_65536];
unsigned short	*idx;
int				_idx_num;
int				ray_begin_mode;
cbool			ray_is_mtex;
cbool			ray_is_color;
int				ray_count;

//void (APIENTRY *Portrait_Viewport) (GLint _x, GLint _y, GLsizei _width, GLsizei _height)
//{
//	// We have to false viewport.  Which is to say we need to rotate it 90.
//	// We may need to switch x and width?
//	// Keep in mind desktop and screen width have been flipped.
//	int x = vid.screen.height - _y;
//	int y = _x;
//	width = _height;
//	height = _width;
//	glViewPort (x, y, width, height);
//}
//
//// What about perspective?
//void (APIENTRY *Portrait_Ortho) (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
//{
//	glRotatef	(90, 0, 0, 1); // Portrait rotate it.
//	glOrtho		(left, right, bottom, top, zNear, zFar);
//}


//
//void APIENTRY OpenGLES_Fake_glBegin (GLenum mode)
//{
//	glBegin (mode);
//}
//
//
////void APIENTRY OpenGLES_Fake_glColor3f (GLfloat red, GLfloat green, GLfloat blue)
////{
////	glColor4f (red, green, blue, 1.0);
////
////}
//
//// Fog and sky
////void APIENTRY OpenGLES_Fake_glColor3fv (const GLfloat *v)
////{
////	glColor4f (v[0], v[1], v[2], 1.0);
////}
//
////void APIENTRY OpenGLES_Fake_glColor3ubv (const GLubyte *v)
////{
////	glColor4f (v[0], v[1], v[2]);
////}
//
//// Used once in R_EmitBox
////void APIENTRY OpenGLES_Fake_glColor4fv (const GLfloat *v)
////{
////	glColor4f (v[0], v[1], v[2], v[3]);
////}
//
//// Particles and QMB
////void APIENTRY OpenGLES_Fake_glColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
////{
////	glColor4f (red/255.0, green/255.0, blue/255.0, alpha/255.0);
////}
//
////void APIENTRY OpenGLES_Fake_glColor4ubv (const GLubyte *v)
////{
////	glColor4f (v[0]/255.0, v[1]/255.0, v[2]/255.0, v[3]/255.0);
////}
//
////void APIENTRY OpenGLES_Fake_glDrawBuffer (GLenum mode)
////{
////	glDrawBuffer (mode);
////}
//
//void APIENTRY OpenGLES_Fake_glEnd (void)
//{
//	glEnd ();
//}
//
//void APIENTRY OpenGLES_Fake_glFogi (GLenum pname, GLint param) // Keep
//{
//	glFogf (pname, param);
//}
//
////void APIENTRY OpenGLES_Fake_glFogiv (GLenum pname, const GLint *params)
////{
////	glFogf (pname, params);
////}
//
////void APIENTRY OpenGLES_Fake_glGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels)
////{
////	glGetTexImage (target, level, format, type, pixels);
////}
//
//#pragma message ("Probably just make this an empty function")
//
////void APIENTRY OpenGLES_Fake_glPolygonMode (GLenum face, GLenum mode)
////{
////	glPolygonMode (face, mode);
////}
//
////void APIENTRY OpenGLES_Fake_glReadBuffer (GLenum mode)
////{
////	glReadBuffer (mode);
////}
//
//
//void APIENTRY OpenGLES_Fake_glTexCoord2f (GLfloat s, GLfloat t)
//{
//	glTexCoord2f (s,t);
//}
//
//void APIENTRY OpenGLES_Fake_glTexCoord2fv (const GLfloat *v)
//{
//	glTexCoord2fv (v);
//}
//
//// https://www.khronos.org/registry/OpenGL-Refpages/es1.1/xhtml/
//// Seems to suggest both glTexEnvf and glTexEnvi are available.
////void APIENTRY OpenGLES_Fake_glTexEnvf (GLenum target, GLenum pname, GLfloat param)
////{
////	glTexEnvf (target, pname, param);
////}
////
////void APIENTRY OpenGLES_Fake_glTexEnvi (GLenum target, GLenum pname, GLint param)
////{
////	// Just re
////	glTexEnvi (target, pname, param);
////}
//
//
//void APIENTRY OpenGLES_Fake_glVertex2f (GLfloat x, GLfloat y)
//{
//	glVertex2f (x, y);
//}
//
//void APIENTRY OpenGLES_Fake_glVertex2fv (const GLfloat *v)
//{
//	glVertex2fv (v);
//}
//
//void APIENTRY OpenGLES_Fake_glVertex3f (GLfloat x, GLfloat y, GLfloat z)
//{
//	glVertex3f (x, y, z);
//}
//
//void APIENTRY OpenGLES_Fake_glVertex3fv (const GLfloat *v)
//{
//	glVertex3fv (v);
//}
//
//

#endif // GLQUAKE + PLATFORM_ANDROID

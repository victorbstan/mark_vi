/*
Copyright (C) 2013-2014 Baker

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
// math_general.h -- basic


#ifndef __MATH_GENERAL_H__
#define __MATH_GENERAL_H__

#define DEGREES_TO_RADIANS(__ANGLE)		((__ANGLE) / 180.0 * M_PI)	 // 1 degrees =  0.0174532925 radians
#define RADIANS_TO_DEGREES(__RADIANS)	((__RADIANS) * (180.0 / M_PI)) // 1 radians = 57.2957795    degrees

#define RANDOM_FLOAT_0_1 ((float)rand()/(float)RAND_MAX)
#define RANDOM_INT
      
int random_int_low_high (int low, int high);

cbool is_pow2 (unsigned int x); // Zero returns false.
//unsigned int NextPowerOfTwo (unsigned int v);
//unsigned int roundup_pow2 (unsigned int x);
unsigned int roundup_pow2_or_next_pow2 (unsigned int x); // 0 = 0, 1=1, 2=2, 4=4, 5=8,	8=8.  This expects positive non-zero integers.
unsigned int roundup_batch (unsigned int n, unsigned int batchsize_pow2);

//#pragma message ("I want this roundup_next_pow2 definitively defined.. What does it do to 1, 2, 16 0 ?")

#define roundup_256(n)	(((n) + 255) & ~255)
#define roundup_16(n) (((n) + 15) & ~15)
#define roundup_8(n) (((n) + 7) & ~7)
int hex_char_to_int (char ch);

int math_sign (int x);

#define Math_KiloBytesDouble(_size) (_size / 1024.0)

#define Math_IsOdd(_yourint) !!((_yourint) % 1)
#define Math_IsMultipleOfInteger(_yourint, _multof) ((_yourint % _multof) == 0)
#define Math_IsWholeNumber64(YOUR_FLOAT) ((YOUR_FLOAT) == (int64_t)(YOUR_FLOAT))

#define ONE_DIV_16 (1/16.0f) // Commonly needed in calculations here so made it a constant.  Is necessary?


float Math_Largest_Multiple_For_Screen (float grainsize, cbool rotated_90, int wanted_width_, int wanted_height_, int client_width, int client_height, reply int *offset_x_out, reply int *offset_y_out, reply int *new_width_out, reply int *new_height_out);


double Cycle_Circular_In_X_Seconds (double raw_clocksecs, double cycle_in_x_seconds, double low, double high);
double angle_maybe_wrap (double angle, reply cbool *did_wrap_out);
double angledelta_maybe_wrap (double angle, reply cbool *did_wrap_out);



// The low should be 1 and the high should be zero to signal uninitialized
#define RANGE_MIN_START 1
#define RANGE_MAX_START 0
cbool range_extend_d (double value, required double *lo, required double *hi);
cbool range_extend_f (float value, required float *lo, required float *hi);

#define bounds_check_fatal_(_lo, _val, _hi)																		\
	if ( !in_range((_lo), (_val), (_hi)))  {																	\
		log_fatal (SPRINTSFUNC_ "Out of range low %g value %g high %g", __func__, (_lo), (_val), (_hi));			\
	} // Finish

double Math_Degrees_Cos(double degrees);
double Math_Degrees_Sin(double degrees);
double Math_Degrees_Corrected_Adjusted_Fov_X (double fov_x_for_aspect_4_3, double width, double height);
double Math_Degrees_Corrected_Adjusted_Fov_Y (double fov_adjusted_x, double width, double height);


// Do we ever use these next 2 shittily named functions?
// (And what were you thinking?  I know you put a lot of work into them, but the names suck
// and without comments, I can't tell wtf they do)
// Do we ever use these next 2 shittily named functions?

double Math_Degrees_FromDiameterFrac (double diameterFraction);
double Math_Degrees_DiameterFrac (double degrees);


#define cosd(a)					\
	(	a == 0    ?  1 :		\
		a == 90   ?  0 :		\
		a == 180  ? -1 :		\
		a == 270  ?  0 :		\
		a == -90  ?  0 :		\
	cos(a * M_PI / 180.0) )

#define sind(a)					\
	(	a == 0    ?  0 :		\
		a == 90   ?  1 :		\
		a == 180  ?  0 :		\
		a == 270  ? -1 :		\
		a == -90  ? -1 :		\
	sin(a * M_PI / 180.0) )

#define Choose321(_choice1, _choice2, _choice3) ( (_choice1) ? 3 : (_choice2) ? 2 : (_choice3) ? 1 :			0)
#define Choose123(_choice1, _choice2, _choice3) ( (_choice1) ? 1 : (_choice2) ? 2 : (_choice3) ? 3 :			0)
#define  Choose12(_choice1, _choice2)			( (_choice1) ? 1 : (_choice2) ? 2 :								0)
#define  Choose21(_choice1, _choice2)			( (_choice1) ? 1 : (_choice2) ? 2 :								0)

// This is used just once.
#define choose4(x,a,b,c,d) ((x) == (a) ? a :\
							(x) == (b) ? b :\
							(x) == (c) ? c :\
							(x) == (d) ? d :\
							0 )

// These return -1 on failure.
#define Choose3(_val, _value0, _value1, _value2)			((_val) == 0 ? (_value0) : (_val) == 1 ? (_value1) : (_val) == 2 ? (_value2)								: -1)
#define Choose4(_val, _value0, _value1, _value2, _value3)	((_val) == 0 ? (_value0) : (_val) == 1 ? (_value1) : (_val) == 2 ? (_value2) : (_val) == 3 ? (_value3)		: -1)

#define ChoosePointer4(_val, _value0, _value1, _value2, _value3)	((_val) == 0 ? (_value0) : (_val) == 1 ? (_value1) : (_val) == 2 ? (_value2) : (_val) == 3 ? (_value3)		: NULL)
#define ChooseIndex_Or_Null5(_val, _value0, _value1, _value2, _value3, _value4)	((_val) == 0 ? (_value0) : (_val) == 1 ? (_value1) : (_val) == 2 ? (_value2) : (_val) == 3 ? (_value3) : (_val) == 4 ? (_value4)		: NULL)

#define iif(_cond, trueval, falseval)	((_cond) ? (trueval) : (falseval))
#define iif_this_else(x, y) iif(x, x, y)

#define CONVBOOL !!
#define BIT_XOR	^ // Note that unllike && and || it does not normalize the args to boolean, if that matters.
#define BIT_NOT ~
#define LOGICAL_XOR(A,B) (CONVBOOL(A) ^ CONVBOOL (B))
#define ISNOT ! // NOT USED currently


#define isin0(x) (0)					// No, not insane.  You can mark an future list with this.  Beats a comment.
#define isin1(x,a) ((x) == (a))			// No, not insane.  What if you expect more, but can only remember one or only know of 1 at the moment.
#define isin2(x,a,b) ((x) == (a) || (x) == (b))
#define isin3(x,a,b,c) ((x) == (a) || (x) == (b) || (x) == (c))
#define isin4(x,a,b,c,d) ((x) == (a) || (x) == (b) || (x) == (c) || (x) == (d))
#define isin5(x,a,b,c,d,e) ((x) == (a) || (x) == (b) || (x) == (c) || (x) == (d) || (x) == (e))
#define isin6(x,a,b,c,d,e,f) ((x) == (a) || (x) == (b) || (x) == (c) || (x) == (d) || (x) == (e) || (x) == (f))
#define isin7(x,a,b,c,d,e,f,g) ((x) == (a) || (x) == (b) || (x) == (c) || (x) == (d) || (x) == (e) || (x) == (f) || (x) == (g))
#define isin8(x,a,b,c,d,e,f,g,h) ((x) == (a) || (x) == (b) || (x) == (c) || (x) == (d) || (x) == (e) || (x) == (f) || (x) == (g) || (x) == (h))
#define isin9(x,a,b,c,d,e,f,g,h,i) ((x) == (a) || (x) == (b) || (x) == (c) || (x) == (d) || (x) == (e) || (x) == (f) || (x) == (g) || (x) == (h) || (x) == (i))
#define isin10(x,a,b,c,d,e,f,g,h,i,j) ((x) == (a) || (x) == (b) || (x) == (c) || (x) == (d) || (x) == (e) || (x) == (f) || (x) == (g) || (x) == (h) || (x) == (i) || (x) == (j))
#define isin11(x,a,b,c,d,e,f,g,h,i,j,k) ((x) == (a) || (x) == (b) || (x) == (c) || (x) == (d) || (x) == (e) || (x) == (f) || (x) == (g) || (x) == (h) || (x) == (i) || (x) == (j) || (x) == (k))
#define isin12(x,a,b,c,d,e,f,g,h,i,j,k,l) ((x) == (a) || (x) == (b) || (x) == (c) || (x) == (d) || (x) == (e) || (x) == (f) || (x) == (g) || (x) == (h) || (x) == (i) || (x) == (j) || (x) == (k) || (x) == (l))
#define isin13(x,a,b,c,d,e,f,g,h,i,j,k,l,m) ((x) == (a) || (x) == (b) || (x) == (c) || (x) == (d) || (x) == (e) || (x) == (f) || (x) == (g) || (x) == (h) || (x) == (i) || (x) == (j) || (x) == (k) || (x) == (l) || (x) == (m))

#define choose4(x,a,b,c,d) ((x) == (a) ? a :\
							(x) == (b) ? b :\
							(x) == (c) ? c :\
							(x) == (d) ? d :\
							0 )

int aligned_size (int size, int alignsize);


#define RECTRB_REPLY(rectrb) &(rectrb).left, &(rectrb).top, &(rectrb).width, &(rectrb).height, &(rectrb).right, &(rectrb).bottom
#define RECT_REPLY(rect) &(rect).left, &(rect).top, &(rect).width, &(rect).height
#define RECT_SEND(rect) (rect).left, (rect).top, (rect).width, (rect).height
#define PRECT_SEND(rect) (rect)->left, (rect)->top, (rect)->width, (rect)->height
#define PRECT_SEND_INSET(rect, inset) (rect)->left + inset, (rect)->top + inset, (rect)->width - inset * 2, (rect)->height - inset * 2

#define RECT_SEND_LEFTTOP(rect) (rect).left, (rect).top
#define RECT_SEND_MULT(rect,factor) ((rect).left * factor), ((rect).top * factor), ((rect).width * factor), ((rect).height * factor)

//#define PRECT_SEND(prect) (prect)->left, (prect)->top, (prect)->width, (prect)->height  Haven't had to use this yet
#define RECT_SEND_LTRB(rect) (rect).left, (rect).top, (rect).left + (rect).width - 1, (rect).top + (rect).height - 1

#define RECT_SEND_LRBT(rect) (rect).left, (rect).left + (rect).width - 1, (rect).top + (rect).height - 1, (rect).top

#define RECT_RIGHTOF(rect)		((rect).left + (rect).width)			// first pixel BELOW a rectangle that isn't part of it.  "Bottom of" it.
#define RECT_RIGHT(rect)		((rect).left + (rect).width - 1)
#define RECT_BOTTOMOF(rect)		((rect).top + (rect).height)
#define RECT_BOTTOM(rect)		((rect).top + (rect).height - 1)

#define RECT_WIDTH(_rect)		((_rect).right - (_rect).left)
#define RECT_HEIGHT(_rect)		((_rect).bottom - (_rect).top)


#define RECT_SET(rect, _left, _top, _width, _height) (rect).left = _left, (rect).top = _top, (rect).width = _width, (rect).height = _height
#define RECT_SET_LEFT_TOP(rect, _left, _top) (rect).left = _left, (rect).top = _top

#define RECT_HIT_X(rect, x) in_range_beyond((rect).left, x, (rect).left + (rect).width)
#define RECT_HIT_Y(rect, y) in_range_beyond((rect).top, y,  (rect).top  + (rect).height)

#define RECT_HIT(rect, x, y) (RECT_HIT_X(rect, x) && RECT_HIT_Y(rect, y))

#define RECT_HIT_X_OFFSET(rect, x) (x - (rect).left)
#define RECT_HIT_Y_OFFSET(rect, y) (y - (rect).top)

#define PRECT_SET(PRB, LEFT,TOP,WIDTH,HEIGHT) (PRB)->left = LEFT, (PRB)->top = TOP, (PRB)->width = WIDTH, (PRB)->height = HEIGHT //, (PRB)->right = RECT_RIGHT((*(PRB))), (PRB)->bottom = RECT_BOTTOM((*(PRB))) // Ender  

#endif // ! __MATH_GENERAL_H__




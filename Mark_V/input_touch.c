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
// input_touch.c -- input


#include "quakedef.h"
#include "input.h"		// Courtesy
#include "menu_local.h"	// Privledged access



//
// Touch stamps
//

cbool Touch_Button_Hit_Set_Stamp (touch_stamp_t *te, int x, int y)
{
	DEBUG_ASSERT (te->touch_button_idx == IDX_NOT_FOUND_NEG1);

	if (ESCAPE_BOX_COLLISION(x, y)) {
		return false; // REJECTED.  DO NOTHING.  HIT ESCAPE.
	}

	// We hit even showscores
	{ int n; for (n = touch_button_forward_left_1; n < touch_button_COUNT; n++) {
		touch_button_t *btn = &focus0.touch_buttons[n];
		if (focus0.touch_buttons[n].r.width) {
			if (RECT_HIT (btn->r, x, y)) {
				// Ok we have a hit.  But are we accepting them?
				if (btn->touch_stamp)	return false; // REJECTED.  DO NOTHING.  ALREADY A TOUCH ON THIS BUTTON.
				
				// Accepted.  Pair them
				btn->touch_stamp = te->touch_stamp; te->touch_button_idx = n; focus0.touch_buttons_num_down ++;
				return true;
			}
		}
	}}
	
	{
		touch_button_t *e = &focus0.touch_buttons[touch_button_canvas_0];
		if (e->touch_stamp)		return false; // REJECTED.  DO NOTHING.  ALREADY A TOUCH ON THE CANVAS.
		
		// Accepted pair them
		e->touch_stamp = te->touch_stamp; te->touch_button_idx = touch_button_canvas_0; focus0.touch_buttons_num_down ++;
		return true;

	}
}



int Touch_Stamp_New_Idx (void *touch_stamp)
{
	{ int n; for (n = 0; n < ARRAY_COUNT(focus0.touch_stamps); n ++) {
		touch_stamp_t *ts = &focus0.touch_stamps[n];
		if (ts->touch_stamp == NULL) {
			ts->touch_stamp			= touch_stamp;
			ts->is_dead				= false;
			ts->touch_button_idx	= IDX_NOT_FOUND_NEG1;
			ts->idx					= n;
			focus0.touch_stamps_num_down ++;
			fprintf (stderr, "Created stamp %d" NEWLINE, (int)touch_stamp); // To IDE debug console
			return n;
		}
	}}
	
	focus0.touch_stamps_num_down ++;
	Con_PrintLinef ("All touch stamps full, cannot add %d", (int) touch_stamp);
	return IDX_NOT_FOUND_NEG1;
}

void Touch_Stamp_Kill_Idx (int idx)
{
	touch_stamp_t *ts = &focus0.touch_stamps[idx];
	
	DEBUG_ASSERT (ts->touch_button_idx == IDX_NOT_FOUND_NEG1);
	fprintf (stderr, "Destroyed stamp %d" NEWLINE, (int)ts->touch_stamp); // To IDE debug console
	ts->touch_stamp = NULL;
	
	focus0.touch_stamps_num_down --;
}

int Touch_Stamp_Find_Idx (void *touch_stamp)
{
	{ int n; for (n = 0; n < ARRAY_COUNT(focus0.touch_stamps); n ++) {
		touch_stamp_t *ts = &focus0.touch_stamps[n];
		if (ts->touch_stamp == touch_stamp) {
			return n;
		}
	}}

#if 0
	// Stupidly the id seems to change sometimes if a whole lot of finger action happens
	// So let's find any down and call it a day!
	{ int n; for (n = 0; n < ARRAY_COUNT(focus0.touch_stamps); n ++) {
		touch_stamp_t *ts = &focus0.touch_stamps[n];
		if (ts->touch_stamp) {
			Con_PrintLinef ("Hard luck 2nd dibs on %d", (int) touch_stamp);
			return n;
		}
	}}
#endif
	Con_PrintLinef ("Touch stamp %d not found in table", (int) touch_stamp);
	return IDX_NOT_FOUND_NEG1;
}

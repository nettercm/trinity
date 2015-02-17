
#ifndef _logic_h_
#define _logic_h_

#ifdef __cplusplus 
extern "C" {
#endif


#include <windows.h>
#include "messages.h"

void process_user_input(void);


#define KEY_F1			0xe13b
#define KEY_F2			0xe13c
#define KEY_F3			0xe13d
#define KEY_F4			0xe13e
#define KEY_F5			0xe13f
#define KEY_F6			0xe140
#define KEY_F7			0xe141
#define KEY_F8			0xe142
#define KEY_F9			0xe143
#define KEY_F10			0xe144
#define KEY_F11			0xe085
#define KEY_F12			0xe086

#define KEY_SHIFT_F1	0xe154
#define KEY_SHIFT_F2	0xe155
#define KEY_SHIFT_F3	0xe156
#define KEY_SHIFT_F4	0xe157
#define KEY_SHIFT_F5	0xe158
#define KEY_SHIFT_F6	0xe159
#define KEY_SHIFT_F7	0xe15a
#define KEY_SHIFT_F8	0xe15b
#define KEY_SHIFT_F9	0xe15c
#define KEY_SHIFT_F10	0xe15d
#define KEY_SHIFT_F11	0xe087
#define KEY_SHIFT_F12	0xe088

#define KEY_CTRL_LEFT	0xe073
#define KEY_CTRL_RIGHT	0xe074

#define KEY_LEFT		0xe04b
#define KEY_RIGHT		0xe04d
#define KEY_UP			0xe048
#define KEY_DOWN		0xe050


		/*
		0xe0:
		e0 4b  left arrow
		e0 4d right arrow
		e0 48 up arrow
		e0 50 down arrow
		e0 73 ctrl left arrow
		e0 74 ctrl right arrow
		e0 8d ctrl up 
		e0 91 ctrl down
		00 9b alt left
		00 9d alt right
		00 98 alt up
		00 a0 alt down
		   4f end
		   51 pg down
		   49 pg up
		   47 home
		   52 ins
		   53 del
	    00 3b..44 = F1..F10
		00 54..5d = Shift-F1...Shift-F10
		00 5e..67 = Ctrl-F1...Ctrl-F10
		00 68..71 = Alt-F1...Alt-F10

		*/


#ifdef __cplusplus 
}
#endif


#endif

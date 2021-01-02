/* ============================================================== */
/*
 * xconsole_io.c
 *
 * Copyright (c) 2017 David Hammond
 * All Rights Reserved.
 */
/* ============================================================== */
/*
 * NOTICE:  All information contained herein is and remains the
 * property of David Hammond. The intellectual and technical
 * concepts contained herein are proprietary to David Hammond
 * and may be covered by U.S.and Foreign Patents, patents in
 * process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this
 * material is strictly forbidden unless prior written permission
 * is obtained David Hammond.
 */
/* ============================================================== */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <X11/Xlib.h>

#include "common.h"
#include "powermon_logger.h"

void xconsole_login(void)
{
	Display *display = XOpenDisplay(NULL);
	Visual *visual = DefaultVisual(display,0);
	int depth = DefaultDepth(display,0);

	int text_x;
	int text_y;

	XSetWindowAttributes frame_attributes;
	Window frame_window;

    XFontStruct *fontinfo;
    XGCValues gr_values;
    GC graphical_context;

    long event_mask = ExposureMask | StructureNotifyMask | ButtonPressMask;
    XEvent event;
    unsigned int event_processing_loop_exit = FALSE;

    char user_login_string[] = "username:";
    char user_login_length = sizeof(user_login_string);

	frame_attributes.background_pixel = XWhitePixel(display, 0);
	/* create the application window */
	int win_x = 0;
	int win_y = 0;
	unsigned int win_width = 400;
	unsigned int win_height = 100;
	unsigned int border_width = 5;

	frame_window = XCreateWindow(
			display,
			XRootWindow(display, 0),
			win_x, win_y, win_width, win_height, border_width,
			depth,
			InputOutput,
			visual,
			CWBackPixel,
	        &frame_attributes);

	XStoreName(display, frame_window, "User Login");
	XSelectInput(display, frame_window, event_mask);

	fontinfo = XLoadQueryFont(display, "10x20");
	gr_values.font = fontinfo->fid;
	gr_values.foreground = XBlackPixel(display, 0);
	graphical_context = XCreateGC(display, frame_window, GCFont+GCForeground, &gr_values);

	XMapWindow(display, frame_window);

	while ( event_processing_loop_exit == FALSE )
	{
		XNextEvent(display, (XEvent *)&event);
		switch ( event.type )
		{
			case Expose:
			{
				XWindowAttributes window_attributes;
				int font_direction, font_ascent, font_descent;
				XCharStruct text_structure;

				XTextExtents(fontinfo, user_login_string, user_login_length,
							 &font_direction, &font_ascent, &font_descent,
							 &text_structure);

				XGetWindowAttributes(display, frame_window, &window_attributes);

				text_x = (window_attributes.width - text_structure.width)/2;
				text_y = (window_attributes.height -
						  (text_structure.ascent+text_structure.descent))/2;

				XDrawString(display, frame_window, graphical_context,
							text_x, text_y, user_login_string, user_login_length);
				break;
			}
			case ButtonPress:
				POWERMON_LOGGER(USER_IO, DEBUG, "%s: Received a button press.\n", __FUNCTION__);
				event_processing_loop_exit = TRUE;
				break;

			default:
				break;
		}
	}
}

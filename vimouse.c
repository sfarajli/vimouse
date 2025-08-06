#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <stdio.h>
#include <unistd.h>

#define MOVE_AMOUNT 10
#define FAST_AMOUNT 30

int main() {
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Cannot open X display\n");
        return 1;
    }

    Window root = DefaultRootWindow(display);

    // Grab the keyboard for modal operation
    if (XGrabKeyboard(display, root, True, GrabModeAsync, GrabModeAsync, CurrentTime) != GrabSuccess) {
        fprintf(stderr, "Failed to grab keyboard\n");
        return 1;
    }

    printf("Mouse control mode activated. Press 'q' to exit.\n");

    XEvent event;
    KeySym keysym;
    int x, y;

    while (1) {
	static int ishover = 0;
        XNextEvent(display, &event);
        if (event.type != KeyPress) continue;

        keysym = XLookupKeysym(&event.xkey, 0);
        int dx = 0, dy = 0;

        int shift = event.xkey.state & ShiftMask;
        int speed = shift ? FAST_AMOUNT : MOVE_AMOUNT;

	switch (keysym) {
	case XK_h: dx = -speed; break;
	case XK_l: dx = speed;  break;
	case XK_k: dy = -speed; break;
	case XK_j: dy = speed;  break;

	case XK_f: // Left click (tap)
		   XTestFakeButtonEvent(display, 1, True, CurrentTime);
		   XTestFakeButtonEvent(display, 1, False, CurrentTime);
		   break;
	case XK_g: // Hold and Release left button down
		   if (ishover) {
			   XTestFakeButtonEvent(display, 1, False, CurrentTime);
			   ishover = 0;
		   } else {
			   XTestFakeButtonEvent(display, 1, True, CurrentTime);
			   ishover = 1;
		   }
		   break;
	case XK_d: // Right click
		   XTestFakeButtonEvent(display, 3, True, CurrentTime);
		   XTestFakeButtonEvent(display, 3, False, CurrentTime);
		   break;
	case XK_a: // Scroll up
		   XTestFakeButtonEvent(display, 4, True, CurrentTime);
		   XTestFakeButtonEvent(display, 4, False, CurrentTime);
		   break;
	case XK_s: // Scroll down
		   XTestFakeButtonEvent(display, 5, True, CurrentTime);
		   XTestFakeButtonEvent(display, 5, False, CurrentTime);
		   break;
	case XK_q: // Quit
		   printf("Exiting mouse control mode.\n");
		   XUngrabKeyboard(display, CurrentTime);
		   XCloseDisplay(display);
		   return 0;
	}

        if (dx || dy) {
            Window ret_root, child;
            int win_x, win_y;
            unsigned int mask;
            XQueryPointer(display, root, &ret_root, &child, &x, &y, &win_x, &win_y, &mask);
            XWarpPointer(display, None, root, 0, 0, 0, 0, x + dx, y + dy);
        }

        XFlush(display);
    }

    return 0;
}

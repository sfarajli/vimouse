#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define MOVE_AMOUNT 10
#define FAST_AMOUNT 30

static void die(char *);
static void GrabKeyboard(void);
static int is_modifier(KeySym);

static Window root;
static Display *display;

int main() {
    GrabKeyboard();

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

	case XK_f: /* Left click (tap) */
		   XTestFakeButtonEvent(display, 1, True, CurrentTime);
		   XTestFakeButtonEvent(display, 1, False, CurrentTime);
		   break;
	case XK_g: /* Hold and Release left button down */
		   if (ishover) {
			   XTestFakeButtonEvent(display, 1, False, CurrentTime);
			   ishover = 0;
		   } else {
			   XTestFakeButtonEvent(display, 1, True, CurrentTime);
			   ishover = 1;
		   }
		   break;
	case XK_d: /* Right click */
		   XTestFakeButtonEvent(display, 3, True, CurrentTime);
		   XTestFakeButtonEvent(display, 3, False, CurrentTime);
		   break;
	case XK_a: /* Scroll up */
		   XTestFakeButtonEvent(display, 4, True, CurrentTime);
		   XTestFakeButtonEvent(display, 4, False, CurrentTime);
		   break;
	case XK_s: /* Scroll down */
		   XTestFakeButtonEvent(display, 5, True, CurrentTime);
		   XTestFakeButtonEvent(display, 5, False, CurrentTime);
		   break;
	default:
		   if (is_modifier(keysym) || keysym == XK_Escape || keysym == XK_space || keysym == XK_q) {
		   	printf("Exiting mouse control mode.\n");
		   	XUngrabKeyboard(display, CurrentTime);
		   	XCloseDisplay(display);
		   	return 0;
		   }
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

static void
die(char *errmsg)
{
	fprintf(stderr, "%s\n", errmsg);
	exit(1);
}

static void
GrabKeyboard(void)
{
	display = XOpenDisplay(NULL);
	if (!display)
		die("Failed to open Xdisplay");

	struct timespec ts = { .tv_sec = 0, .tv_nsec = 1000000  };

	root = DefaultRootWindow(display);
	/* Try to grab keyboard, we may have to wait for another process to ungrab */
	for (int i = 0; i < 1000; i++) {
		if (XGrabKeyboard(display, root, True, GrabModeAsync, GrabModeAsync, CurrentTime) == GrabSuccess)
			return;

		nanosleep(&ts, NULL);
	}
	die("Failed to grab keyboard");
}

int
is_modifier(KeySym keysym)
{
	/* `XK_Shift_L` is used for changing the speed of the cursor */
	KeySym modifiers[] = {
		XK_Shift_R,
		XK_Control_L,
		XK_Control_R,
		XK_Alt_L,
		XK_Alt_R,
		XK_Meta_L,
		XK_Meta_R,
		XK_Super_L,
		XK_Super_R,
		XK_Hyper_L,
		XK_Hyper_R,
		XK_Caps_Lock,
		XK_Num_Lock,
		XK_Scroll_Lock,
	};

	int n = sizeof(modifiers) / sizeof(modifiers[0]);
	for (int i = 0; i < n; i++)
		if (keysym == modifiers[i])
			return 1;

	return 0;
}

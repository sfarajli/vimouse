
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <stdio.h>
#include <unistd.h>

#define MOVE_AMOUNT 10
#define FAST_AMOUNT 30

// Keycodes for left/right control/shift/alt/super/meta
int is_modifier_keysym(KeySym ks) {
    return ks == XK_Control_L || ks == XK_Control_R ||
           ks == XK_Alt_L     || ks == XK_Alt_R     ||
           ks == XK_Shift_L   || ks == XK_Shift_R   ||
           ks == XK_Super_L   || ks == XK_Super_R   ||
           ks == XK_Meta_L    || ks == XK_Meta_R;
}

int is_any_modifier_pressed(Display *dpy) {
    char keys_return[32];
    XQueryKeymap(dpy, keys_return);

    KeyCode mods[] = {
        XKeysymToKeycode(dpy, XK_Control_L),
        XKeysymToKeycode(dpy, XK_Control_R),
        XKeysymToKeycode(dpy, XK_Alt_L),
        XKeysymToKeycode(dpy, XK_Alt_R),
        XKeysymToKeycode(dpy, XK_Super_L),
        XKeysymToKeycode(dpy, XK_Super_R),
        XKeysymToKeycode(dpy, XK_Meta_L),
        XKeysymToKeycode(dpy, XK_Meta_R),
    };

    for (int i = 0; i < sizeof(mods)/sizeof(mods[0]); ++i) {
        int byte = mods[i] / 8;
        int bit = mods[i] % 8;
        if (keys_return[byte] & (1 << bit)) return 1;
    }
    return 0;
}

int main() {
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Cannot open X display\n");
        return 1;
    }

    Window root = DefaultRootWindow(display);

    if (XGrabKeyboard(display, root, True, GrabModeAsync, GrabModeAsync, CurrentTime) != GrabSuccess) {
        fprintf(stderr, "Failed to grab keyboard\n");
        return 1;
    }

    printf("Mouse control mode activated. Press 'q' to exit.\n");

    XEvent event;
    KeySym keysym;
    int x, y;
    static int ishover = 0;

    while (1) {
        XNextEvent(display, &event);

        // Handle modifier key: ungrab temporarily
        if (event.type == KeyPress) {
            keysym = XLookupKeysym(&event.xkey, 0);
            if (is_modifier_keysym(keysym)) {
                XUngrabKeyboard(display, CurrentTime);

                // Wait until all modifier keys are released
                while (is_any_modifier_pressed(display)) {
                    usleep(10000); // 10 ms
                }

                // Try to re-grab
                while (XGrabKeyboard(display, root, True, GrabModeAsync, GrabModeAsync, CurrentTime) != GrabSuccess) {
                    usleep(10000); // Wait until grab is successful
                }

                continue;
            }
        }

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
        case XK_f:
            XTestFakeButtonEvent(display, 1, True, CurrentTime);
            XTestFakeButtonEvent(display, 1, False, CurrentTime);
            break;
        case XK_g:
            if (ishover) {
                XTestFakeButtonEvent(display, 1, False, CurrentTime);
                ishover = 0;
            } else {
                XTestFakeButtonEvent(display, 1, True, CurrentTime);
                ishover = 1;
            }
            break;
        case XK_d:
            XTestFakeButtonEvent(display, 3, True, CurrentTime);
            XTestFakeButtonEvent(display, 3, False, CurrentTime);
            break;
        case XK_a:
            XTestFakeButtonEvent(display, 4, True, CurrentTime);
            XTestFakeButtonEvent(display, 4, False, CurrentTime);
            break;
        case XK_s:
            XTestFakeButtonEvent(display, 5, True, CurrentTime);
            XTestFakeButtonEvent(display, 5, False, CurrentTime);
            break;
        case XK_q:
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


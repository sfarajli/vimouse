# X11 Keyboard Mouse Control

A minimal C program to control the mouse pointer and simulate mouse
clicks using keyboard keys on X11-based Linux systems.

## Usage

Bind it to a key. (You can use
[sxhkd](https://github.com/baskerville/sxhkd))

-   Move mouse pointer using Vim-style keys:
    -   `h`: move left
    -   `l`: move right
    -   `k`: move up
    -   `j`: move down
-   Hold `Shift` for faster pointer movement.
-   Mouse clicks via keys:
    -   `f`: left click (tap)
    -   `v`: toggle left button hold/release
    -   `d`: right click
-   Scroll wheel simulation:
    -   `a`: scroll up
    -   `s`: scroll down
-   Exit mouse control mode with keys: `q`, `Escape`, `Space`, or any
    modifier keys (except for left shift).

## Requirements

-   Linux with X11 display server
-   Development libraries: `libX11` and `libXtst` (for XTest extension)

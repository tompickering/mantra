#ifndef GUARD_DRAW_H
#define GUARD_DRAW_H

#include <ncurses.h>
#include "win/win.h"

void get_dims(int* r, int* c);
void draw_windows(int r, int c);
void draw_screen();

#endif

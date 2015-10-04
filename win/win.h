#ifndef GUARD_WIN_H
#define GUARD_WIN_H

#include <ncurses.h>

void win_init(WINDOW**);
void win_setup(WINDOW*, int, int, int, int);
void draw_win_bookmarks(int, int, int, int);
void draw_win_pages(int, int, int, int);
void draw_win_helpbar(int, int, int, int);

#endif

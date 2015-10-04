#ifndef GUARD_WIN_H
#define GUARD_WIN_H

#include <ncurses.h>

typedef struct _Win {
    WINDOW* win;
    void (*draw)();
} Win;

const int WIN_IDX_BOOKMARKS = 0;
const int WIN_IDX_PAGES     = 1;
const int WIN_IDX_HELPBAR   = 2;
const int NWIN              = 3;

extern Win** wins;

void win_init_all();
void win_update(int, int, int, int, int);
void win_draw_border();
void draw_windows();
void draw_win_bookmarks();
void draw_win_pages();
void draw_win_helpbar();

#endif

#ifndef GUARD_WIN_H
#define GUARD_WIN_H

#include <ncurses.h>

typedef struct _Win {
    WINDOW* win;
    void (*draw)();
} Win;

extern const int WIN_IDX_BOOKMARKS;
extern const int WIN_IDX_PAGES;
extern const int WIN_IDX_HELPBAR;
extern const int NWIN;

extern Win** wins;

void win_init_all();
void win_update(int, int, int, int, int);
void win_draw_border(Win*);
void win_draw_all();
void draw_win_bookmarks();
void draw_win_pages();
void draw_win_helpbar();

#endif

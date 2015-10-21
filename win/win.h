#ifndef GUARD_WIN_H
#define GUARD_WIN_H

#include <stdbool.h>

#include <ncurses.h>
#include <panel.h>

typedef struct _Win {
    unsigned int r;
    unsigned int c;
    bool can_be_active;
    WINDOW* win;
    PANEL* pnl;
    void (*draw)();
    void (*input)(int);
} Win;

extern const int WIN_COL_PAIR_NORMAL;
extern const int WIN_COL_PAIR_ACTIVE;

extern const int WIN_IDX_BOOKMARKS;
extern const int WIN_IDX_PAGES;
extern const int WIN_IDX_HELPBAR;
extern const int NWIN;

extern Win** wins;
extern int win_act_idx;

void win_init_all();
void win_update(Win*, int, int, int, int);
void win_clear_row(Win*, int);
void win_clear_all();
void win_cycle_active();
void win_set_active(int);
int  win_active();
void win_draw_border(Win*);
void win_draw_all();
void draw_win_bookmarks();
void draw_win_pages();
void draw_win_helpbar();
void input_win_bookmarks(int);
void input_win_pages(int);
Win* active_win();
char* string_clean_buffer(char*, char*, unsigned int);
void open_page(int, char*, int);

#endif

#include "win.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <ncurses.h>
#include <panel.h>

const int WIN_COL_PAIR_NORMAL = 0;
const int WIN_COL_PAIR_ACTIVE = 1;

const int WIN_IDX_BOOKMARKS = 0;
const int WIN_IDX_PAGES     = 1;
const int WIN_IDX_HELPBAR   = 2;
const int WIN_IDX_BOOKPNL   = 3;
const int NWIN              = 4;

Win** wins;
int win_act_idx;

/**
 * Default input handler for windows. It should never be called,
 * but if a bug allows it to be then it's better than a segfault!
 */
void input_default(int n) {
    fprintf(stderr, "Error: This window does not take input.\n");
    exit(1);
}

void win_init_all() {
    int i;
    wins = (Win**) malloc(NWIN * sizeof(Win*));
    for (i = 0; i < NWIN; ++i) {
        wins[i] = (Win*) malloc(sizeof(Win));
        wins[i]->win = newwin(0, 0, 0, 0);
        wins[i]->pnl = new_panel(wins[i]->win);
    }

    wins[WIN_IDX_BOOKMARKS]->draw = draw_win_bookmarks;
    wins[WIN_IDX_PAGES    ]->draw = draw_win_pages;
    wins[WIN_IDX_HELPBAR  ]->draw = draw_win_helpbar;
    wins[WIN_IDX_BOOKPNL  ]->draw = draw_win_bookpnl;

    wins[WIN_IDX_BOOKMARKS]->input = input_win_bookmarks;
    wins[WIN_IDX_PAGES    ]->input = input_win_pages;
    wins[WIN_IDX_HELPBAR  ]->input = input_default;
    wins[WIN_IDX_BOOKPNL  ]->input = input_win_bookpnl;

    wins[WIN_IDX_BOOKMARKS]->can_be_active = true;
    wins[WIN_IDX_PAGES    ]->can_be_active = true;
    wins[WIN_IDX_HELPBAR  ]->can_be_active = false;
    wins[WIN_IDX_BOOKPNL  ]->can_be_active = false;

    hide_panel(wins[WIN_IDX_BOOKPNL]->pnl);

    init_pair(WIN_COL_PAIR_NORMAL, COLOR_WHITE, COLOR_BLACK);
    init_pair(WIN_COL_PAIR_ACTIVE, COLOR_GREEN, COLOR_BLACK);
    win_act_idx = WIN_IDX_BOOKMARKS;

    update_panels();
    doupdate();
}

/**
 * Update a window's position and size attributes.
 */
void win_update(Win* window, int x, int y, int r, int c) {
    WINDOW* win = window->win;
    wresize(win, r, c);
    mvwin(win, y, x);
}

/**
 * Overwrite a row of characters in a window with spaces.
 */
void win_clear_row(Win* win, int r) {
    char wipe_char = ' ';
    char* wiper = (char*) malloc((win->c + 1) * sizeof(char));
    memset(wiper, wipe_char, win->c);
    wiper[win->c] = '\0';
    mvwprintw(win->win, r, 0, wiper);
    free(wiper);
}

/**
 * Completely overwrite a window's area with space characters.
 * This does not call win_clear_row for memory allocation
 * efficiency.
 */
void win_clear_all() {
    int i, j, r, c;
    WINDOW* win;
    char* wiper;
    char wipe_char = ' ';
    getmaxyx(stdscr, r, c);
    wiper = (char*) malloc((c + 1) * sizeof(char));
    memset(wiper, wipe_char, c);
    wiper[c] = '\0';
    for (i = 0; i < NWIN; ++i) {
        win = wins[i]->win;
        getmaxyx(win, r, c);
        for (j = 0; j < r; ++j)
            mvwprintw(win, j, 0, wiper);
    }
    free(wiper);
}

/**
 * Find the next Win for which can_be_active is true
 * and make it the currently-active window.
 */
void win_cycle_active() {
    int idx;
    for (idx = (win_act_idx + 1) % NWIN; idx != win_act_idx; idx = ++idx % NWIN) {
        if (wins[idx]->can_be_active) {
            win_act_idx = idx;
            break;
        }
    }
}

void win_set_active(int idx) {
    win_act_idx = idx;
}

int win_active() {
    return win_act_idx;
}

void win_draw_border(Win* win) {
    int col_pair = WIN_COL_PAIR_NORMAL;
    if (wins[win_act_idx] == win)
        col_pair = WIN_COL_PAIR_ACTIVE;
    wattron(win->win, COLOR_PAIR(col_pair));
    box(win->win, 0, 0);
    wattroff(win->win, COLOR_PAIR(col_pair));
}

/**
 * Iterate over all windows and call their draw method.
 */
void win_draw_all() {
    int i;
    int r, c;
    for (i = 0; i < NWIN; ++i) {
        getmaxyx(wins[i]->win, r, c);
        wins[i]->r = r;
        wins[i]->c = c;
        wins[i]->draw();
    }
}

Win* active_win() {
    return wins[win_act_idx];
}

/**
 * Copy a string into a buffer to a max of len bytes.
 * Furthermore, ensure that the buffer is clean and null-terminated.
 */
char* string_clean_buffer(char* buf, char* src, unsigned int len) {
    int src_len = strlen(src);
    strncpy(buf, src, len);
    if (src_len < len)
        memset(buf + src_len, ' ', len - src_len);
    if (src_len > len)
        buf[len-3] = buf[len-2] = buf[len-1] = '.';
    buf[len] = '\0';
    return buf;
}

/**
 * Spawn a 'man' process displaying the requested page.
 */
void open_page(int sect, char* page, int line) {
    char* cmd = (char*) malloc((strlen(page) + 6) * sizeof(char));
    strncpy(cmd, "man   ", 4);
    strcpy(cmd+6, page);
    cmd[4] = '0' + sect;
    cmd[5] = ' ';
    system(cmd);
    free(cmd);
    endwin();
    refresh();
    win_clear_all();
}

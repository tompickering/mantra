/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * Mantra: Man page bookmarker                                           *
 * Copyright (C) 2015  Tom Pickering                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "win.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <ncurses.h>
#include <panel.h>

#include "../pty.h"

const int WIN_COL_PAIR_NORMAL      = 0;
const int WIN_COL_PAIR_ACTIVE      = 1;
const int WIN_COL_PAIR_PANELS      = 2;
const int WIN_COL_PAIR_BOOKMARK_HL = 3;
const int WIN_COL_PAIR_PAGE_HL     = 4;

const int WIN_IDX_BOOKMARKS = 0;
const int WIN_IDX_PAGES     = 1;
const int WIN_IDX_HELPBAR   = 2;
const int WIN_IDX_BOOKPNL   = 3;
const int NWIN              = 4;

Win** wins;
int win_act_idx;
int pnl_act_idx;

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

    wins[WIN_IDX_BOOKMARKS]->update = update_win_bookmarks;
    wins[WIN_IDX_PAGES    ]->update = update_win_pages;
    wins[WIN_IDX_HELPBAR  ]->update = NULL;
    wins[WIN_IDX_BOOKPNL  ]->update = NULL;

    wins[WIN_IDX_BOOKMARKS]->can_be_active = true;
    wins[WIN_IDX_PAGES    ]->can_be_active = true;
    wins[WIN_IDX_HELPBAR  ]->can_be_active = false;
    wins[WIN_IDX_BOOKPNL  ]->can_be_active = false;

    hide_panel(wins[WIN_IDX_BOOKPNL]->pnl);

    init_pair(WIN_COL_PAIR_NORMAL     , COLOR_WHITE, COLOR_BLACK);
    init_pair(WIN_COL_PAIR_ACTIVE     , COLOR_GREEN, COLOR_BLACK);
    init_pair(WIN_COL_PAIR_PANELS     , COLOR_BLUE , COLOR_BLACK);
    init_pair(WIN_COL_PAIR_BOOKMARK_HL, COLOR_BLUE , COLOR_BLACK);
    init_pair(WIN_COL_PAIR_PAGE_HL    , COLOR_GREEN, COLOR_BLACK);
    win_act_idx = WIN_IDX_BOOKMARKS;
    pnl_act_idx = -1;

    pnl_init_all();

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
    window->r = r;
    window->c = c;
    if (window->update) window->update();
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
    for (idx = (win_act_idx + 1) % NWIN; idx != win_act_idx; idx = (idx + 1) % NWIN) {
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
    Win* pnl = active_pnl();
    if (win == pnl)
        col_pair = WIN_COL_PAIR_PANELS;
    else if (wins[win_act_idx] == win)
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

Win* active_pnl() {
    if (pnl_act_idx < 0)
        return NULL;
    return wins[pnl_act_idx];
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
void open_page(int sect, char* page, char* line) {
    char* cmd;
    int line_str_len;
    char* line_str = "0g";
    int cmd_len = strlen(page) + 7; /* len("man x " "\0") */
    cmd = (char*) malloc(cmd_len * sizeof(char));
    strncpy(cmd, "man   ", 4);
    strcpy(cmd+6, page);
    cmd[4] = '0' + sect;
    cmd[5] = ' ';
    cmd[cmd_len - 1] = '\0';

    if (line) {
        line_str_len = strlen(line);

        /* 2 extra characters for 'g' and \0 */
        line_str = (char*) malloc(line_str_len + 2);
        strcpy(line_str, line);
        line_str[line_str_len] = 'g';
        line_str[line_str_len + 1] = '\0';
    }

    endwin();
    run_pty(cmd, line_str);

    free(cmd);
    refresh();
    win_clear_all();
}

void open_panel(int pnl_idx) {
    Win* win = wins[pnl_idx];
    pnl_act_idx = pnl_idx;
    show_panel(win->pnl);
}

void close_panel() {
    Win* win;
    if (pnl_act_idx >= 0) {
        win = wins[pnl_act_idx];
        hide_panel(win->pnl);
        pnl_act_idx = -1;
    }
}

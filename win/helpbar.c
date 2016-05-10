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

#include "helpbar.h"
#include "win.h"

#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <ncurses.h>
#include <form.h>

#include "pages.h"
#include "bookmarks.h"
#include "../input.h"
#include "../page.h"
#include "../file.h"
#include "../error.h"

FORM *bar_form_bmark;
FIELD *bar_input_bmark;

FORM *bar_form_search;
FIELD *bar_input_search;

BarMode bar_mode = BAR_MODE_IDLE;

const char helpstr[] = "b-Bookmark  /-Search  Tab-Switch";

void clean_helpbar() {
    int cols, rows;
    Win *win = wins[WIN_IDX_HELPBAR];
    char *wiper;

    getmaxyx(win->win, rows, cols);
    wiper = calloc(cols + 1, sizeof(char));
    memset(wiper, ' ', cols);
    mvwprintw(win->win, 0, 0, wiper);
    free(wiper);
}

BarMode bar_get_mode() {
    return bar_mode;
}

void bar_set_mode(BarMode mode) {
    if (bar_mode != mode) {
        clean_helpbar();
        bar_mode = mode;
        if (mode == BAR_MODE_BMARK) {
            post_form(bar_form_bmark);
        } else if (mode == BAR_MODE_SEARCH) {
            post_form(bar_form_search);
        } else if (mode == BAR_MODE_IDLE) {
            unpost_form(bar_form_bmark);
            unpost_form(bar_form_search);
        }
        refresh();
    }
}

void draw_win_helpbar() {
    int cols;
    int xoff;
    Win *win = wins[WIN_IDX_HELPBAR];
    /* Use xoff here to prevent gcc  complaining about unused 'rows' var */
    getmaxyx(win->win, xoff, cols);

    if (bar_mode == BAR_MODE_IDLE) {
        int helplen = strlen(helpstr);
        xoff = (cols - helplen) >> 1;
        if (xoff > 0)
            wmove(win->win, 0, xoff);
        wprintw(win->win, "%s", helpstr);
    }

    wrefresh(win->win);
}

void input_win_helpbar(int ch) {
    switch (bar_mode) {
        case BAR_MODE_BMARK:
            switch (ch) {
                case '\r':
                    _save_bookmark();
                    bar_set_mode(BAR_MODE_IDLE);
                    break;
                case KEY_BACKSPACE:
                    form_driver(bar_form_bmark, REQ_DEL_PREV);
                    break;
                default:
                    form_driver(bar_form_bmark, ch);
            }
            break;
        case BAR_MODE_SEARCH:
            switch (ch) {
                case '\r':
                    perform_search();
                    bar_set_mode(BAR_MODE_IDLE);
                    break;
                case KEY_BACKSPACE:
                    form_driver(bar_form_search, REQ_DEL_PREV);
                    break;
                default:
                    form_driver(bar_form_search, ch);
            }
            break;
        default:
            break;
    }
}

void bar_form_init() {
    Win *win;

    FIELD **fields = (FIELD **)malloc(2 * sizeof(FIELD *));
    fields[1] = NULL;

    win = wins[WIN_IDX_HELPBAR];

    /* FIXME: Handle field dimensions properly */
    bar_input_bmark = new_field(1, 10, 0, 0, 0, 0);
    set_field_type(bar_input_bmark, TYPE_INTEGER, 0, 0, INT_MAX);
    set_field_back(bar_input_bmark, A_UNDERLINE);
    field_opts_off(bar_input_bmark, O_AUTOSKIP);
    fields[0] = bar_input_bmark;
    bar_form_bmark = new_form(fields);
    set_form_win(bar_form_bmark, win->win);
    set_form_sub(bar_form_bmark, derwin(win->win, win->r, win->c, 2, 2));

    /* FIXME: Handle field dimensions properly */
    bar_input_search = new_field(1, 40, 0, 0, 0, 0);
    set_field_type(bar_input_search, TYPE_REGEXP, "*");
    set_field_back(bar_input_search, A_UNDERLINE);
    field_opts_off(bar_input_search, O_AUTOSKIP);
    fields[0] = bar_input_search;
    bar_form_search = new_form(fields);
    set_form_win(bar_form_search, win->win);
    set_form_sub(bar_form_search, derwin(win->win, win->r, win->c, 2, 2));
    post_form(bar_form_search);
}

void bar_init() {
    clean_helpbar();
    bar_form_init();
}

void _save_bookmark() {
    char *bookmark;
    form_driver(bar_form_bmark, REQ_VALIDATION);
    bookmark = field_buffer(bar_input_bmark, 0);
    *(strchr(bookmark, ' ')) = '\0';

    if (active_win() == wins[WIN_IDX_PAGES]) {
        add_bookmark(get_current_page(), bookmark, NULL);
    } else if (active_win() == wins[WIN_IDX_BOOKMARKS]) {
        Bookmark *bm = get_current_bm();
        if (bm) add_bookmark(bm->page, bookmark, bm);
    }

    set_field_buffer(bar_input_bmark, 0, "");
}

void perform_search() {
    char *term;
    form_driver(bar_form_search, REQ_VALIDATION);
    term = field_buffer(bar_input_search, 0);
    *(strchr(term, ' ')) = '\0';

    if (active_win() == wins[WIN_IDX_PAGES]) {
        search_pagewin(true, term);
    } else if (active_win() == wins[WIN_IDX_BOOKMARKS]) {
        search_bmwin(true, term);
    } else {
        die("Search not defined for active window.");
    }

    set_field_buffer(bar_input_search, 0, "");
}

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

#define _GNU_SOURCE /* For strchrnul */

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

FORM *bar_form = NULL;
FIELD *bar_input = NULL;
FIELD **bar_fields = NULL;

BarMode bar_mode = BAR_MODE_IDLE;

const char helpstr[] = "b-Bookmark  /-Search  Tab-Switch";
const char prompt_bmark[] = "Line: ";
const char prompt_search[] = "Search: ";

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
    Win *win = wins[WIN_IDX_HELPBAR];
    if (bar_mode != mode) {
        clean_helpbar();
        bar_mode = mode;
        if (mode == BAR_MODE_BMARK || mode == BAR_MODE_SEARCH) {
            bar_form_init(mode);
            post_form(bar_form);

            /* Save the current location; incremental search means
             * that we may wish to reset to the current position */
            if (mode == BAR_MODE_SEARCH) {
                void (*save)() = active_win() == wins[WIN_IDX_PAGES] ?
                    save_location_page : save_location_bm;
                save();
            }
        } else if (mode == BAR_MODE_IDLE && bar_form != NULL) {
            unpost_form(bar_form);
            free(bar_form);
        }
        wrefresh(win->win);
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
    } else if (bar_mode == BAR_MODE_BMARK) {
        mvwprintw(win->win, 0, 1, "%s", prompt_bmark);
    } else if (bar_mode == BAR_MODE_SEARCH) {
        mvwprintw(win->win, 0, 1, "%s", prompt_search);
    }

    wrefresh(win->win);
}

void input_win_helpbar(int ch) {
    void (*load)() = active_win() == wins[WIN_IDX_PAGES] ?
        load_location_page : load_location_bm;
    switch (bar_mode) {
        case BAR_MODE_BMARK:
            switch (ch) {
                case '\r':
                    _save_bookmark();
                    bar_set_mode(BAR_MODE_IDLE);
                    break;
                case KEY_BACKSPACE:
                    form_driver(bar_form, REQ_DEL_PREV);
                    break;
                default:
                    form_driver(bar_form, ch);
            }
            break;
        case BAR_MODE_SEARCH:
            switch (ch) {
                case '\r':
                    set_field_buffer(bar_input, 0, "");
                    bar_set_mode(BAR_MODE_IDLE);
                    break;
                case KEY_BACKSPACE:
                    if (!strcmp(field_buffer(bar_input, 0), ""))
                        break;
                    load();
                    form_driver(bar_form, REQ_DEL_PREV);
                    perform_search();
                    break;
                default:
                    form_driver(bar_form, ch);
                    perform_search();
            }
            break;
        default:
            break;
    }
}

void bar_form_init(BarMode mode) {
    Win *win;
    int rows, cols;
    int fwidth;
    int prompt_len;

    if (mode == BAR_MODE_IDLE) return;

    if (bar_fields != NULL) free(bar_fields);
    bar_fields = (FIELD **)malloc(2 * sizeof(FIELD *));
    bar_fields[1] = NULL;

    win = wins[WIN_IDX_HELPBAR];

    getmaxyx(win->win, rows, cols);
    fwidth = cols - 2;

    if (mode == BAR_MODE_BMARK) prompt_len = strlen(prompt_bmark);
    else prompt_len = strlen(prompt_search);
    fwidth -= prompt_len;

    if (fwidth <= 0) {
        /* FIXME - Make the reason for this (lack of room)
         * more obvious to the user? */
        bar_set_mode(BAR_MODE_IDLE);
        return;
    }

    bar_input = new_field(1, fwidth, 0, 1, 0, 0);

    if (mode == BAR_MODE_BMARK)
        set_field_type(bar_input, TYPE_INTEGER, 0, 0, INT_MAX);
    else
        set_field_type(bar_input, TYPE_REGEXP, "*");

    set_field_back(bar_input, A_UNDERLINE);
    field_opts_off(bar_input, O_AUTOSKIP);
    bar_fields[0] = bar_input;
    bar_form = new_form(bar_fields);
    set_form_win(bar_form, win->win);
    set_form_sub(bar_form, derwin(win->win, win->r, win->c - prompt_len, 1, 1 + prompt_len));
}

void bar_init() {
    clean_helpbar();
}

void _save_bookmark() {
    char *bookmark;
    form_driver(bar_form, REQ_VALIDATION);
    bookmark = field_buffer(bar_input, 0);
    *(strchr(bookmark, ' ')) = '\0';

    if (active_win() == wins[WIN_IDX_PAGES]) {
        add_bookmark(get_current_page(), bookmark, NULL);
    } else if (active_win() == wins[WIN_IDX_BOOKMARKS]) {
        Bookmark *bm = get_current_bm();
        if (bm) add_bookmark(bm->page, bookmark, bm);
    }

    set_field_buffer(bar_input, 0, "");
}

void perform_search() {
    char *term;
    form_driver(bar_form, REQ_VALIDATION);
    term = strdup(field_buffer(bar_input, 0));
    *(strchrnul(term, ' ')) = '\0';

    if (active_win() == wins[WIN_IDX_PAGES]) {
        search_pagewin(true, term);
    } else if (active_win() == wins[WIN_IDX_BOOKMARKS]) {
        search_bmwin(true, term);
    } else {
        die("Search not defined for active window.");
    }

    free(term);
}

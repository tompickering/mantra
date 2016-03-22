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
#include <limits.h>

#include <ncurses.h>
#include <form.h>

#include "pages.h"
#include "bookmarks.h"
#include "../input.h"
#include "../page.h"
#include "../file.h"
#include "../error.h"

FORM *bookpnl_form;
FIELD *bookpnl_field_bookmark;

FORM *searchpnl_form;
FIELD *searchpnl_field_search;

void pnl_init_all() {
    Win *win;

    FIELD **fields = (FIELD **)malloc(2 * sizeof(FIELD *));
    fields[1] = NULL;

    win = wins[WIN_IDX_BOOKPNL];
    /* FIXME: Handle field dimensions properly */
    bookpnl_field_bookmark = new_field(1, 10, 2, 2, 0, 0);
    set_field_type(bookpnl_field_bookmark, TYPE_INTEGER, 0, 0, INT_MAX);
    set_field_back(bookpnl_field_bookmark, A_UNDERLINE);
    field_opts_off(bookpnl_field_bookmark, O_AUTOSKIP);
    fields[0] = bookpnl_field_bookmark;
    bookpnl_form = new_form(fields);
    set_form_win(bookpnl_form, win->win);
    set_form_sub(bookpnl_form, derwin(win->win, win->r, win->c, 2, 2));
    post_form(bookpnl_form);

    win = wins[WIN_IDX_SEARCHPNL];
    /* FIXME: Handle field dimensions properly */
    searchpnl_field_search = new_field(1, 40, 2, 2, 0, 0);
    set_field_type(searchpnl_field_search, TYPE_REGEXP, "*");
    set_field_back(searchpnl_field_search, A_UNDERLINE);
    field_opts_off(searchpnl_field_search, O_AUTOSKIP);
    fields[0] = searchpnl_field_search;
    searchpnl_form = new_form(fields);
    set_form_win(searchpnl_form, win->win);
    set_form_sub(searchpnl_form, derwin(win->win, win->r, win->c, 2, 2));
    post_form(searchpnl_form);
}

void draw_win_bookpnl() {
    Win *win = wins[WIN_IDX_BOOKPNL];
    if (panel_hidden(win->pnl))
        return;
    win_draw_border(win);
    wrefresh(win->win);
}

void draw_win_searchpnl() {
    Win *win = wins[WIN_IDX_SEARCHPNL];
    if (panel_hidden(win->pnl))
        return;
    win_draw_border(win);
    wrefresh(win->win);
}

void _save_bookmark() {
    char *bookmark;
    form_driver(bookpnl_form, REQ_VALIDATION);
    bookmark = field_buffer(bookpnl_field_bookmark, 0);
    *(strchr(bookmark, ' ')) = '\0';

    if (active_win() == wins[WIN_IDX_PAGES]) {
        add_bookmark(get_current_page(), bookmark, NULL);
    } else if (active_win() == wins[WIN_IDX_BOOKMARKS]) {
        Bookmark *bm = get_current_bm();
        if (bm) add_bookmark(bm->page, bookmark, bm);
    }

    set_field_buffer(bookpnl_field_bookmark, 0, "");
}

void input_win_bookpnl(int ch) {
    switch (ch) {
        case '\r':
            _save_bookmark();
            close_panel();
            break;
        case KEY_BACKSPACE:
            form_driver(bookpnl_form, REQ_DEL_PREV);
            break;
        default:
            form_driver(bookpnl_form, ch);
    }
}

void perform_search() {
    char *term;
    form_driver(searchpnl_form, REQ_VALIDATION);
    term = field_buffer(searchpnl_field_search, 0);
    *(strchr(term, ' ')) = '\0';

    if (active_win() == wins[WIN_IDX_PAGES]) {
        search_pagewin(true, term);
    } else if (active_win() == wins[WIN_IDX_BOOKMARKS]) {
        search_bmwin(true, term);
    } else {
        die("Search not defined for active window.");
    }

    set_field_buffer(searchpnl_field_search, 0, "");
}

void input_win_searchpnl(int ch) {
    switch (ch) {
        case '\r':
            perform_search();
            close_panel();
            break;
        case KEY_BACKSPACE:
            form_driver(searchpnl_form, REQ_DEL_PREV);
            break;
        default:
            form_driver(searchpnl_form, ch);
    }
}

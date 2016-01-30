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

#include <string.h>
#include <limits.h>

#include <ncurses.h>
#include <form.h>

#include "pages.h"
#include "../input.h"
#include "../page.h"
#include "../file.h"

FORM* bookpnl_form;
FIELD* bookpnl_field_bookmark;

void pnl_init_all() {
    Win* win;

    win = wins[WIN_IDX_BOOKPNL];
    /* FIXME: Handle field dimensions properly */
    bookpnl_field_bookmark = new_field(1, 10, 2, 2, 0, 0);
    set_field_type(bookpnl_field_bookmark, TYPE_INTEGER, 0, 0, INT_MAX);
    set_field_back(bookpnl_field_bookmark, A_UNDERLINE);
    field_opts_off(bookpnl_field_bookmark, O_AUTOSKIP);
    bookpnl_form = new_form(&bookpnl_field_bookmark);
    set_form_win(bookpnl_form, win->win);
    set_form_sub(bookpnl_form, derwin(win->win, win->r, win->c, 2, 2));
    post_form(bookpnl_form);
}

void draw_win_bookpnl() {
    Win* win = wins[WIN_IDX_BOOKPNL];
    if (panel_hidden(win->pnl))
        return;
    win_draw_border(win);
    wrefresh(win->win);
}

void _save_bookmark() {
    char* bookmark;
    form_driver(bookpnl_form, REQ_VALIDATION);
    bookmark = field_buffer(bookpnl_field_bookmark, 0);
    *(strchr(bookmark, ' ')) = '\0';
    add_bookmark(get_current_page(), bookmark, true);
    set_field_buffer(bookpnl_field_bookmark, 0, "");
    close_panel();
}

void input_win_bookpnl(int ch) {
    switch (ch) {
        case K_RETURN:
            _save_bookmark();
            break;
        case KEY_BACKSPACE:
            form_driver(bookpnl_form, REQ_LEFT_CHAR);
            form_driver(bookpnl_form, REQ_DEL_CHAR);
            break;
        default:
            form_driver(bookpnl_form, ch);
    }
}

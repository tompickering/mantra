#include "win.h"

#include <limits.h>

#include <ncurses.h>
#include <form.h>

#include "../input.h"
#include "../page.h"

FORM* bookpnl_form;
FIELD* bookpnl_field_bookmark;

void pnl_init_all() {
    Win* win;

    win = wins[WIN_IDX_BOOKPNL];
    //FIXME: Handle field dimensions properly.
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
    // TODO: Save bookmark
    close_panel();
}

void input_win_bookpnl(int ch) {
    switch (ch) {
        case K_RETURN:
            _save_bookmark();
            break;
        default:
            form_driver(bookpnl_form, ch);
    }
}

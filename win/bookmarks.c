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

#include <ncurses.h>

#include "../input.h"
#include "../page.h"
#include "../file.h"

int _current_row_bm = 0;
int _prev_row_bm = 0;
Bookmark* _bm_start = NULL;
Bookmark* _current_bm = NULL;
int _MAX_NAME_LEN_BM = 20;

void win_bookmarks_show(Win* win) {
    /* TODO: Deduplicate with pages code */
    int r = 1;
    int c = 2;
    int col_pair;
    Bookmark* bm = _bm_start;
    int max_desc_len;
    char* name;
    char* desc;
    char sect[2];
    Page* page;
    int i;

    sect[1] = '\0';

    _MAX_NAME_LEN_BM = win->c / 3;
    max_desc_len = win->c - _MAX_NAME_LEN_BM - 7;
    name = malloc((_MAX_NAME_LEN_BM + 1) * sizeof(char));
    desc = malloc((max_desc_len + 1) * sizeof(char));

    if (_bm_start == NULL) _bm_start = bookmarks;

    for (; bm != NULL && r < win->r - 1; ++r) {
        page = bm->page;
        sect[0] = '0' + page->sect;
        mvwprintw(win->win, r, c, sect);
        string_clean_buffer(name, page->name, _MAX_NAME_LEN_BM);
        string_clean_buffer(desc, page->desc,  max_desc_len);
        mvwprintw(win->win, r, c + 2, name);
        mvwprintw(win->win, r, c + 3 + _MAX_NAME_LEN_BM, desc);
        bm = bm->next;
    }

    col_pair = WIN_COL_PAIR_NORMAL;
    if (win == wins[win_act_idx])
        col_pair = WIN_COL_PAIR_ACTIVE;
    mvwchgat(win->win, _current_row_bm + 1, 1, win->c - 2, A_REVERSE, col_pair, NULL);
    if (_current_row_bm != _prev_row_bm) {
        mvwchgat(win->win, _prev_row_bm + 1, 1, win->c - 2, A_NORMAL, WIN_COL_PAIR_NORMAL, NULL);
        _prev_row_bm = _current_row_bm;
    }

    /* Recalculate _current_bm */
    _current_bm = _bm_start;
    for (i = 0; _current_bm != NULL && i < _current_row_bm; ++i) {
        _current_bm = _current_bm->next;
        if (_current_bm == NULL) break;
    }

    free(name);
    free(desc);
}

void draw_win_bookmarks() {
    Win* win = wins[WIN_IDX_BOOKMARKS];
    win_bookmarks_show(win);
    win_draw_border(win);
    wrefresh(win->win);
}

/* TODO */
void _page_bm(bool down) {}

void _navigate_bm(bool down) {
    Win* win = wins[WIN_IDX_BOOKMARKS];
    if (down) {
        if (_current_bm && _current_bm->next && _current_row_bm + 3 < win->r) {
            ++_current_row_bm;
        } else if (_bm_start && _bm_start->next && _current_bm->next) {
            _bm_start = _bm_start->next;
        }
    } else {
        if (_current_row_bm == 0) {
            if (_bm_start && _bm_start->prev) {
                _bm_start = _bm_start->prev;
            }
        } else {
            --_current_row_bm;
        }
    }
}

/* TODO */
void _jump_to_end_bm(bool down) {}

void _open_bm() {
    if (_current_bm != NULL) {
        open_page(_current_bm->page->sect,
                  _current_bm->page->name,
                  _current_bm->line);
    }
}

void input_win_bookmarks(int ch) {
    bool down;
    switch (ch) {
        case K_FWD:
        case K_BACK:
            down = (ch == K_BACK) ? false : true;
            _page_bm(down);
            break;
        case K_UP:
        case K_DOWN:
            down = (ch == K_UP) ? false : true;
            _navigate_bm(down);
            break;
        case K_HOME:
        case K_END:
            down = (ch == K_HOME) ? false : true;
            _jump_to_end_bm(down);
            break;
        case K_OPEN:
            _open_bm();
            break;
    }
}

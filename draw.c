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

#include "draw.h"

#include <stdbool.h>

#include <ncurses.h>

#include "win/win.h"

float PNL_SF_W = 0.6;
float PNL_SF_H = 0.5;

void get_dims(int* r, int* c) {
    getmaxyx(stdscr, *r, *c);
}

void draw_windows(int r, int c) {
    int h_bk = r / 3;
    int pnl_w = (int) (c * PNL_SF_W);
    int pnl_h = (int) (r * PNL_SF_H);
    int pnl_xoff = (int) (c - pnl_w) / 2;
    int pnl_yoff = (int) (r - pnl_h) / 2;
    win_update(wins[WIN_IDX_BOOKMARKS], 0, 0, h_bk, c);
    win_update(wins[WIN_IDX_PAGES], 0, h_bk, r - h_bk - 1, c);
    win_update(wins[WIN_IDX_HELPBAR], 0, r - 1, 1, c);
    win_update(wins[WIN_IDX_BOOKPNL], pnl_xoff, pnl_yoff, pnl_h, pnl_w);
    win_draw_all();
}

void draw_screen() {
    int rows, cols;
    get_dims(&rows, &cols);
    draw_windows(rows, cols);
}

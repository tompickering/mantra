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

#include <ncurses.h>
#include <string.h>

const char helpstr[] = "b-Bookmark  /-Search  Space-Switch  o-Open  jk-Navigate  t-Tag  f-Filter";

void draw_win_helpbar() {
    int cols;
    int helplen, xoff;
    Win *win = wins[WIN_IDX_HELPBAR];
    /* Use xoff here to prevent gcc  complaining about unused 'rows' var */
    getmaxyx(win->win, xoff, cols);
    helplen = strlen(helpstr);
    xoff = (cols - helplen) >> 1;
    if (xoff > 0)
        wmove(win->win, 0, xoff);
    wprintw(win->win, "%s", helpstr);
    wrefresh(win->win);

}

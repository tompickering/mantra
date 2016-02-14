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

#include "input.h"

#include <stdbool.h>

#include "win/win.h"

/**
 * First port of call for input characters.
 * Directs input to the appropriate function
 * or takes appropriate action depending on
 * the current context.
 */
bool handle_input(int ch) {
    bool quit = false;
    if (active_pnl() == NULL) {
        switch (ch) {
            case K_CYCLE:
                win_cycle_active();
                break;
            case K_QUIT:
                quit = true;
                break;
            case K_BOOKPNL:
                open_panel(WIN_IDX_BOOKPNL);
                break;
            case K_SEARCH:
                open_panel(WIN_IDX_SEARCHPNL);
                break;
            default:
                active_win()->input(ch);
        }
    } else {
        active_pnl()->input(ch);
    }
    return !quit;
}

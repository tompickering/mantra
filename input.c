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

#include "file.h"
#include "win/win.h"

Key inp2key(int ch) {
    switch (ch) {
        case '0':               return K_0;
        case '1':               return K_1;
        case '2':               return K_2;
        case '3':               return K_3;
        case '4':               return K_4;
        case '5':               return K_5;
        case '6':               return K_6;
        case '7':               return K_7;
        case '8':               return K_8;
        case '9':               return K_9;
        case K_DEFAULT_CYCLE:   return K_CYCLE;
        case K_DEFAULT_QUIT:    return K_QUIT;
        case K_DEFAULT_DOWN:    return K_DOWN;
        case K_DEFAULT_UP:      return K_UP;
        case K_DEFAULT_HOME:    return K_HOME;
        case K_DEFAULT_END:     return K_END;
        case K_DEFAULT_OPEN:    return K_OPEN;
        case K_DEFAULT_FWD:     return K_FWD;
        case K_DEFAULT_BACK:    return K_BACK;
        case K_DEFAULT_NEXT:    return K_NEXT;
        case K_DEFAULT_PREV:    return K_PREV;
        case K_DEFAULT_BOOKPNL: return K_BOOKPNL;
        case K_DEFAULT_SEARCH:  return K_SEARCH;
        case K_DEFAULT_RETURN:  return K_RETURN;
        case K_DEFAULT_BM_DEL:  return K_BM_DEL;
        case K_DEFAULT_RELOAD:  return K_RELOAD;
    }

    return -1;
}

/**
 * First port of call for input characters.
 * Directs input to the appropriate function
 * or takes appropriate action depending on
 * the current context.
 */
bool handle_input(int ch) {
    bool quit = false;
    Key k = inp2key(ch);
    if (active_pnl() == NULL) {
        switch (k) {
            case K_CYCLE:
                win_cycle_active();
                break;
            case K_QUIT:
                quit = true;
                break;
            case K_RELOAD:
                free_bookmarks();
                load_bookmarks();
                reset_win_bookmarks();
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

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

#ifndef GUARD_INPUT_H
#define GUARD_INPUT_H

#define CTRL      - 96

#define K_DEFAULT_CYCLE   ' '
#define K_DEFAULT_QUIT    'q'
#define K_DEFAULT_DOWN    'j'
#define K_DEFAULT_UP      'k'
#define K_DEFAULT_HOME    'g'
#define K_DEFAULT_END     'G'
#define K_DEFAULT_OPEN    'o'
#define K_DEFAULT_FWD     'f' + CTRL
#define K_DEFAULT_BACK    'b' + CTRL
#define K_DEFAULT_NEXT    'n'
#define K_DEFAULT_PREV    'N'
#define K_DEFAULT_BOOKPNL 'b'
#define K_DEFAULT_SEARCH  '/'
#define K_DEFAULT_RETURN  '\r'
#define K_DEFAULT_BM_DEL  'x'
#define K_DEFAULT_RELOAD  'r'
#define K_DEFAULT_BACKSP   KEY_BACKSPACE

#include <stdbool.h>

typedef enum Key {
    K_0,
    K_1,
    K_2,
    K_3,
    K_4,
    K_5,
    K_6,
    K_7,
    K_8,
    K_9,
    K_CYCLE,
    K_QUIT,
    K_DOWN,
    K_UP,
    K_HOME,
    K_END,
    K_OPEN,
    K_FWD,
    K_BACK,
    K_NEXT,
    K_PREV,
    K_BOOKPNL,
    K_SEARCH,
    K_RETURN,
    K_BM_DEL,
    K_RELOAD,
    K_BACKSP
} Key;

Key inp2key(int ch);
bool handle_input(int ch);

#endif

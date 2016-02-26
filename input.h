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
#define K_CYCLE   ' '
#define K_QUIT    'q'
#define K_DOWN    'j'
#define K_UP      'k'
#define K_HOME    'g'
#define K_END     'G'
#define K_OPEN    'o'
#define K_FWD     'f' + CTRL
#define K_BACK    'b' + CTRL
#define K_NEXT    'n'
#define K_PREV    'N'
#define K_BOOKPNL 'b'
#define K_SEARCH  '/'
#define K_RETURN  '\r'
#define K_BM_DEL  'x'

#include <stdbool.h>

bool handle_input(int ch);

#endif

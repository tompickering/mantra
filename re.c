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

#include "re.h"

#include <stdlib.h>
#include <stdbool.h>
#include <regex.h>

#include "error.h"

bool matches_regex(char *str, char *exp) {
    regex_t re;
    int res;

    if (regcomp(&re, exp, REG_EXTENDED|REG_ICASE|REG_NOSUB|REG_NEWLINE))
        die("Could not compile search regex.");

    res = regexec(&re, str, 0, NULL, 0);
    regfree(&re);

    if (!res) return true;
    return false;
}

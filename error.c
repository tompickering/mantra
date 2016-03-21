/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * Mantra: Man page bookmarker                                           *
 * Copyright (C) 2015  Tom Pickering                                     *
 * Code in this file derived from code (C) 2015 Michael Kerrisk          *
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

#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void die(char *msg) {
    char *error_msg;
    char *header = "Error";
    char *separator = " - ";
    int header_len = strlen(header);
    int separator_len = strlen(separator);
    int msg_len = header_len;

    if (msg != NULL) {
        msg_len += strlen(msg) - 1;
        msg_len += separator_len;
    }

    error_msg = malloc((msg_len + 1) * sizeof(char));

    strcpy(error_msg, header);

    if (msg != NULL) {
        strcpy(error_msg + header_len, separator);
        strcpy(error_msg + header_len + separator_len, msg);
    }

    perror(error_msg);
    exit(1);
}

void die_msg(char *msg) {
    fprintf(stderr, "Error: %s.\n", msg);
    exit(1);
}

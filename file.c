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

#include "file.h"

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <gdbm.h>

/**
 * Create and open a database file to store bookmarks.
 */
void db_init(char* dir) {
    char* db_path = (char*) malloc(strlen(dir) + strlen(MANTRA_DB) + 1);
    strcpy(db_path, dir);
    strcpy(db_path + strlen(dir), MANTRA_DB);
    db_path[strlen(dir) + strlen(MANTRA_DB)] = '\0';
    db = gdbm_open(db_path, 0, GDBM_WRCREAT, 0644, NULL);
}

/**
 * Create ~/.mantra if it doesn't already exist,
 * and open a gdbm database for bookmarks.
 */
void file_init() {
    char* usr_home = NULL;
    char* mantra_home;

    usr_home = getenv("HOME");

    if (usr_home == NULL)
        usr_home = getpwuid(getuid())->pw_dir;

    if (usr_home == NULL) {
        fprintf(stderr, "Error: Unable to determine user home directory.\n");
        exit(1);
    } else if (chdir(usr_home) != 0) {
        fprintf(stderr, "Error: Unable to change to user home directory.\n");
        exit(1);
    }

    mantra_home = (char*) malloc(strlen(usr_home) + strlen(MANTRA_HOME) + 2);
    strcpy(mantra_home, usr_home);
    mantra_home[strlen(usr_home)] = '/';
    strcpy(mantra_home + strlen(usr_home) + 1, MANTRA_HOME);
    mantra_home[strlen(usr_home) + strlen(MANTRA_HOME) + 1] = '\0';

    errno = 0;
    mkdir(MANTRA_HOME, 0700);

    if (errno != 0 && errno != EEXIST) {
        fprintf(stderr, "Error: Unable to create directory %s.\n", MANTRA_HOME);
        exit(1);
    }

    db_init(mantra_home);
}

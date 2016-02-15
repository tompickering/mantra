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

#ifndef GUARD_FILE_H
#define GUARD_FILE_H

#define MANTRA_HOME ".mantra/"
#define MANTRA_DB "bookmarks.db"

#include <stdbool.h>
#include <lmdb.h>

#include "page.h"

typedef struct _Bookmark Bookmark;

struct _Bookmark {
    Page* page;
    char* line;
    Bookmark* prev;
    Bookmark* next;
};

extern Bookmark* bookmarks;

void insert_bookmark(Page*, char*);
void update_bookmark_for_page(Page*, char*);
void rm_bookmark(Bookmark*);
void rm_bookmark_for_page(Page*);
int delete_bookmark_for_page(Page*);
int erase_bookmark(Bookmark*);
int erase_bookmark_for_page(Page*);
int add_bookmark(Page*, char*, bool);
void db_init(char*);
void file_init();
void file_close();

#endif

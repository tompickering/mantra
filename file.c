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

Bookmark* bookmarks = NULL;

/**
 * Load a value into a datum struct.
 */
void _datum(datum* d, char* value) {
    d->dptr = value;
    d->dsize = strlen(value);
}

/**
 * Load bookmarks from DB.
 */
void bookmarks_init() {
    Bookmark* current_bm = bookmarks;
    Bookmark* prev_bm = NULL;
    Page* page;
    datum key;
    datum val;

    key = gdbm_firstkey(db);

    while (key.dptr != NULL) {
        page = search_page(key.dptr[0] - '0', key.dptr + 2);

        if (page != NULL) {
            val = gdbm_fetch(db, key);
            current_bm = (Bookmark*) malloc(sizeof(Bookmark));
            if (bookmarks == NULL) bookmarks = current_bm;
            current_bm->page = page;
            current_bm->line = val.dptr;
            if (prev_bm != NULL) {
                current_bm->prev = prev_bm;
                prev_bm->next = current_bm;
            }
            prev_bm = current_bm;
        } else {
            /* TODO: Flag to clear up all missing marks? */
            fprintf(stderr, "Warning: Bookmarked page '%s' not found.\n", key.dptr + 2);
        }

        key = gdbm_nextkey(db, key);
    }

    if (bookmarks != NULL) bookmarks->prev = NULL;
    if (current_bm != NULL) current_bm->next = NULL;
}

/**
 * Remove a bookmark from the list.
 */
void rm_bookmark(Bookmark* bm) {
    if (bm != NULL) {
        if (bm->prev != NULL) bm->prev->next = bm->next;
        if (bm->next != NULL) bm->next->prev = bm->prev;
        if (bm == bookmarks) bookmarks = bm->next;
        free(bm->line);
        free(bm);
    }
}

/**
 * Remove a bookmark from the list, based on a page.
 */
void rm_bookmark_for_page(Page* page) {
    Bookmark* bm = bookmarks;

    if (bm != NULL)
        while (bm->next && bm->page != page)
            bm = bm->next;

    if (bm != NULL)
        rm_bookmark(bm);
}

/**
 * Insert a bookmark at the end of the list.
 */
void insert_bookmark(Page* page, char* line) {
    Bookmark* bm = (Bookmark*) malloc(sizeof(Bookmark));
    Bookmark* last_bm = bookmarks;

    if (last_bm != NULL)
        while (last_bm->next)
            last_bm = last_bm->next;

    bm->page = page;
    bm->line = (char*) malloc(strlen(line) + 1);
    strcpy(bm->line, line);
    bm->prev = last_bm;
    bm->next = NULL;
    if (last_bm != NULL) last_bm->next = bm;
}

/**
 * Update a bookmark in the list, based on a page.
 */
void update_bookmark_for_page(Page* page, char* line) {
    Bookmark* bm = bookmarks;

    if (bm != NULL)
        while (bm->page != page && bm->next)
            bm = bm->next;

    bm->page = page;
    bm->line = (char*) realloc(bm->line, strlen(line) + 1);
    strcpy(bm->line, line);
}

/**
 * Remove a bookmark from the DB, based on the page.
 */
int delete_bookmark_for_page(Page* page) {
    datum key;
    char* sectpage = (char*) malloc(strlen(page->name) + 3);
    sectpage[0] = '0' + page->sect;
    sectpage[1] = ':';
    strcpy(sectpage + 2, page->name);
    sectpage[strlen(page->name) + 3] = '\0';
    _datum(&key, sectpage);

    if (gdbm_delete(db, key))
        return -1;

    return 0;
}

/**
 * Remove a bookmark from the list and DB.
 */
int erase_bookmark(Bookmark* bm) {
    if (delete_bookmark_for_page(bm->page))
        return -1;
    rm_bookmark(bm);
    return 0;
}

/**
 * Remove a bookmark from the list and DB, based on a page.
 */
int erase_bookmark_for_page(Page* page) {
    if (delete_bookmark_for_page(page))
        return -1;
    rm_bookmark_for_page(page);
    return 0;
}

/**
 * Create a bookmark entry in the DB.
 */
int add_bookmark(Page* page, char* line, bool update) {
    datum key;
    datum val;
    char* sectpage = (char*) malloc(strlen(page->name) + 3);
    sectpage[0] = '0' + page->sect;
    sectpage[1] = ':';
    strcpy(sectpage + 2, page->name);
    sectpage[strlen(page->name) + 3] = '\0';
    _datum(&key, sectpage);
    _datum(&val, line);

    if (gdbm_store(db, key, val, GDBM_INSERT)) {
        /* If we couldn't add a record, this may be
         * because the key exists. If update requested,
         * remove the record and try again.
         */
        if (update) {
            if (gdbm_store(db, key, val, GDBM_REPLACE))
                return -1;
            update_bookmark_for_page(page, line);
        }
        return -1;
    }

    insert_bookmark(page, line);

    return 0;
}

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
    bookmarks_init();
}

/**
 * Cleanup.
 */
void file_close() {
    Bookmark* head = bookmarks;

    while (bookmarks != NULL) {
        bookmarks = bookmarks->next;
        free(head->line);
        free(head);
        head = bookmarks;
    }

    gdbm_close(db);
}

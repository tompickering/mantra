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
#include "error.h"

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

/**
 * Check s returns MDB_SUCCESS, otherwise print error and die.
 */
#define DIE_UNLESS(s) do {\
    int rc = (s);\
    if (rc != MDB_SUCCESS) {\
        die_msg(mdb_strerror(rc));\
    }\
} while (0)

/**
 * Load a NUL-terminated string into an MDB_val struct.
 */
#define STR2VAL(d, v) do {\
    (d)->mv_data = v;\
    /* FIXME: we need to store the trailing \0 at the moment */\
    (d)->mv_size = strlen(v) + 1;\
} while(0)

Bookmark* bookmarks = NULL;
MDB_env *env;
MDB_dbi dbi;

/**
 * Load bookmarks from DB.
 */
void bookmarks_init() {
    Bookmark* current_bm = bookmarks;
    Bookmark* prev_bm = NULL;
    Page* page;
    MDB_txn *txn;
    MDB_cursor *cursor;
    MDB_val key, val;
    int rc;

    DIE_UNLESS(mdb_txn_begin(env, NULL, MDB_RDONLY, &txn));
    DIE_UNLESS(mdb_cursor_open(txn, dbi, &cursor));

    rc = mdb_cursor_get(cursor, &key, &val, MDB_FIRST);
    while (rc != MDB_NOTFOUND) {
        if (rc != MDB_SUCCESS) {
            die(mdb_strerror(rc));
        }
        int section = *((uint8_t*)key.mv_data) - '0';
        char *name = ((char*)key.mv_data) + 2;
        page = search_page(section, name);

        if (page != NULL) {
            char *line = calloc(1, val.mv_size + 1);
            memcpy(line, val.mv_data, val.mv_size);

            current_bm = (Bookmark*) malloc(sizeof(Bookmark));
            if (bookmarks == NULL) bookmarks = current_bm;
            current_bm->page = page;
            current_bm->line = line;
            if (prev_bm != NULL) {
                current_bm->prev = prev_bm;
                prev_bm->next = current_bm;
            }
            prev_bm = current_bm;
        } else {
            /* TODO: Flag to clear up all missing marks? */
            fprintf(stderr, "Warning: Bookmarked page '%s' not found.\n", name);
        }

        rc = mdb_cursor_get(cursor, &key, &val, MDB_NEXT);
    }
    mdb_cursor_close(cursor);
    mdb_txn_commit(txn);

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

    if (!bookmarks) bookmarks = bm;
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
    char *p, *sectpage = (char*) malloc(strlen(page->name) + 3);
    MDB_txn *txn;
    MDB_val key;
    int rc;

    p = sectpage;
    *(p++) = '0' + page->sect;
    *(p++) = ':';
    p = strcpy(p, page->name);

    STR2VAL(&key, sectpage);

    DIE_UNLESS(mdb_txn_begin(env, NULL, 0, &txn));

    rc = mdb_del(txn, dbi, &key, NULL);
    if (rc == MDB_SUCCESS) {
        mdb_txn_commit(txn);
    } else {
        mdb_txn_abort(txn);
    }

    return rc;
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
    MDB_val key, val;
    MDB_txn *txn;
    MDB_cursor *cursor;
    char *p, *sectpage = malloc(strlen(page->name) + 3);
    int rc, updating = 0;

    p = sectpage;
    *(p++) = '0' + page->sect;
    *(p++) = ':';
    p = strcpy(p, page->name);

    STR2VAL(&key, sectpage);
    STR2VAL(&val, line);

    DIE_UNLESS(mdb_txn_begin(env, NULL, 0, &txn));
    DIE_UNLESS(mdb_cursor_open(txn, dbi, &cursor));

    rc = mdb_cursor_get(cursor, &key, &val, MDB_SET);
    if (rc == MDB_SUCCESS) {
        if (!update) {
            mdb_cursor_close(cursor);
            mdb_txn_abort(txn);
            return -1;
        }
        updating = 1;
    } else if (rc != MDB_NOTFOUND) {
        die(mdb_strerror(rc));
    }

    DIE_UNLESS(mdb_put(txn, dbi, &key, &val, 0));

    if (updating) {
        update_bookmark_for_page(page, line);
    } else {
        insert_bookmark(page, line);
    }
    mdb_cursor_close(cursor);
    mdb_txn_commit(txn);

    return 0;
}

/**
 * Create and open a database file to store bookmarks.
 */
void db_init(char* dir) {
    char* db_path = (char*) malloc(strlen(dir) + strlen(MANTRA_DB) + 1);
	MDB_txn *txn;

    strcpy(db_path, dir);
    strcpy(db_path + strlen(dir), MANTRA_DB);
    db_path[strlen(dir) + strlen(MANTRA_DB)] = '\0';

    DIE_UNLESS(mdb_env_create(&env));
    DIE_UNLESS(mdb_env_set_mapsize(env, 10485760));
    DIE_UNLESS(mdb_env_open(env, db_path, MDB_NOSUBDIR, 0600));

    DIE_UNLESS(mdb_txn_begin(env, NULL, 0, &txn));
    DIE_UNLESS(mdb_dbi_open(txn, NULL, MDB_DUPSORT, &dbi));
    DIE_UNLESS(mdb_txn_commit(txn));

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
    mkdir(mantra_home, 0700);

    if (errno != 0 && errno != EEXIST) {
        fprintf(stderr, "Error: Unable to create directory %s.\n", mantra_home);
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

    mdb_dbi_close(env, dbi);
    mdb_env_close(env);
}

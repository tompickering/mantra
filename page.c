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

#include "page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Page *pages = NULL;
Page **SECT;
unsigned int NPAGES;
const unsigned char NSECTS = 9;

/* FIXME: Try to get this data from the mandb rather than a command */
/**
 * Source a list of man pages in the system and store
 * the information in a data construct ('pages').
 */
void pages_init() {
    FILE *fp;
    int npages;
    char *sect = NULL;
    int page_idx;
    char tok_delim[2] = " ";
    char cmd[] = "man -k . -s    2>/dev/null | sort";
    char *line, *tok = NULL;
    size_t len = 0;
    size_t toklen;
    Page *page = NULL;

    fp = popen("man -k . | wc -l", "r");
    getline(&line, &len, fp);
    npages = atoi(line);
    pages = (Page *)calloc(npages, sizeof(Page));
    NPAGES = npages;
    pclose(fp);

    /* Allocate one additional element - the overhead
     * is worth the convenience of being able to address
     * by actual section number! */
    SECT = (Page **)calloc(NSECTS + 1, sizeof(Page*));

    sect = (char *)calloc(2, sizeof(char));

    page_idx = 0;
    for (sect[0] = '1'; sect[0] <= '0' + NSECTS; ++sect[0]) {
        SECT[sect[0] - '0'] = &pages[page_idx];
        cmd[12] = sect[0];
        fp = popen(cmd, "r");
        if (fp == NULL) {
            fprintf(stderr, "Error; Could not load man pages.\n");
            exit(1);
        }
        while ((getline(&line, &len, fp)) != -1) {
            page = &pages[page_idx++];
            tok = strtok(line, tok_delim);
            toklen = strlen(tok);
            page->name = (char*) malloc((toklen+1) * sizeof(char));
            strcpy(page->name, tok);
            page->sect = strdup(sect);
            strtok(NULL, tok_delim);
            strtok(NULL, tok_delim);
            tok = strtok(NULL, "");
            toklen = strlen(tok);
            page->desc = (char*) malloc((toklen+1) * sizeof(char));
            strcpy(page->desc, tok);
        }
        pclose(fp);
        free(line);
        line = NULL;
    }
}

Page *search_page(char *sect, char *name) {
    Page *inspect = SECT[sect[0] - '0'];
    while (inspect < pages + NPAGES && !strncmp(inspect->sect, sect, 1)) {
        if (!strcmp(name, inspect->name))
            return inspect;
        inspect++;
    }
    return NULL;
}

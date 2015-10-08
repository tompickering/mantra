#include "page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Page* pages = NULL;

// FIXME: Try to get this data using libmandb rather than a command
// FIXME: Store the data in a hash table, not a linked list
void pages_init() {
    FILE* fp;
    char sect;
    char tok_delim[2] = " ";
    char cmd[] = "man -k . -s   ";
    char* line = NULL;
    char* tok = NULL;
    size_t len;
    size_t toklen;
    Page* page = NULL;
    Page* prevpage = NULL;

    for (sect = 1; sect < 10; ++sect) {
        cmd[12] = ('0' + sect);
        fp = popen(cmd, "r");
        if (fp == NULL) {
            fprintf(stderr, "Error; Could not load man pages.\n");
            exit(1);
        }
        while ((getline(&line, &len, fp)) != -1) {
            page = (Page*) malloc(sizeof(Page));
            if (pages == NULL)
                pages = page;
            if (prevpage != NULL)
                prevpage->next = page;
            tok = strtok(line, tok_delim);
            toklen = strlen(tok);
            page->name = (char*) malloc((toklen+1) * sizeof(char));
            strcpy(page->name, tok);
            page->sect = sect;
            prevpage = page;
        }
    }

    pclose(fp);
}

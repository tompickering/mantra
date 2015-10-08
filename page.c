#include "page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Page* pages = NULL;
unsigned int NPAGES;

// FIXME: Try to get this data using libmandb rather than a command
void pages_init() {
    FILE* fp;
    int npages;
    char sect;
    int page_idx = 0;
    char tok_delim[2] = " ";
    char cmd[] = "man -k . -s   ";
    char* line = NULL;
    char* tok = NULL;
    size_t len;
    size_t toklen;
    Page* page = NULL;

    fp = popen("man -k . | wc -l", "r");
    getline(&line, &len, fp);
    npages = atoi(line);
    pages = malloc(npages * sizeof(Page));
    NPAGES = npages;

    for (sect = 1; sect < 10; ++sect) {
        cmd[12] = ('0' + sect);
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
            page->sect = sect;
        }
    }

    pclose(fp);
}

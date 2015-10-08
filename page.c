#include "page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Page* pages = NULL;
Page** SECT;
unsigned int NPAGES;
const unsigned char NSECTS = 9;

// FIXME: Try to get this data using libmandb rather than a command
void pages_init() {
    FILE* fp;
    int npages;
    char sect;
    int page_idx;
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

    // Allocate one additional element - the overhead
    // is worth the convenience of being able to address
    // by actual section number!
    SECT = malloc((NSECTS + 1) * sizeof(Page*));

    page_idx = 0;
    for (sect = 1; sect <= NSECTS; ++sect) {
        SECT[sect] = &pages[page_idx];
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

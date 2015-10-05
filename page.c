#include "page.h"

#include <stdio.h>
#include <stdlib.h>

void pages_init() {
    char sect;
    FILE* fp;
    size_t len;
    char* line = NULL;
    char cmd[] = "man -k . -s  ";
    for (sect = 1; sect < 10; ++sect) {
        cmd[12] = ('0' + sect);
        fp = popen(cmd, "r");
        if (fp == NULL) {
            fprintf(stderr, "Error; Could not load man pages.\n");
            exit(1);
        }
        while ((getline(&line, &len, fp)) != -1) {
            // TODO: Something useful with the output!
        }
    }
    pclose(fp);
}

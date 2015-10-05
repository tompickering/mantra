#ifndef GUARD_MAN_H
#define GUARD_MAN_H

typedef struct _Page {
    char* name;
} Page;

extern Page** pages;

void pages_init();

#endif

#ifndef GUARD_MAN_H
#define GUARD_MAN_H

typedef struct _Page Page;

struct _Page {
    char sect;
    char* name;
    Page* next;
};

extern Page* pages;

void pages_init();

#endif

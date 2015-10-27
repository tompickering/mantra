#ifndef GUARD_MAN_H
#define GUARD_MAN_H

typedef struct _Page Page;

struct _Page {
    char sect;
    char* name;
    char* desc;
};

extern Page* pages;
extern Page** SECT;
extern unsigned int NPAGES;
extern const unsigned char NSECTS;

void pages_init();

#endif

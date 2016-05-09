#!/usr/bin/make -f

.PHONY: clean all install

all:

OBJS := $(patsubst %.c,%.o,$(wildcard *.c **/*.c))
DEPS = $(OBJS:%.o=%.d)
CLEAN = $(NAME) $(OBJS) $(DEPS)

LDLIBS=-lncursesw -lmenu -lform -llmdb -lm
NAME=mantra
CFLAGS=-Wall -pedantic -g

#CC=gcc -pg

PREFIX = /usr/local

%.d: %.c
	$(CC) -MM -MF $@ -MT $@ -MT $*.o $<

all: $(NAME)

$(NAME):

$(NAME): $(OBJS)

clean:
	rm -rf $(CLEAN) core

install: all
	install $(NAME) $(DESTDIR)$(PREFIX)/bin

ifneq ($(findstring clean,$(MAKECMDGOALS)),clean)
-include $(DEPS)
endif

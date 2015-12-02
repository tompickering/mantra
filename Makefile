#!/usr/bin/make -f

.PHONY: clean all

all:

OBJS := $(patsubst %.c,%.o,$(wildcard *.c **/*.c))
DEPS = $(OBJS:%.o=%.d)
CLEAN = $(PROGRAM) $(OBJS) $(DEPS)

LDLIBS=-lncursesw -lpanel -lmenu -lform
NAME=mantra
CFLAGS=-Wall -pedantic -g

%.d: %.c
	$(CC) -MM -MF $@ -MT $@ -MT $*.o $<

all: $(NAME)

$(NAME):

$(NAME): $(OBJS)

clean:
	rm -rf $(CLEAN) core

install:
	cp mantra /usr/bin/

ifneq ($(findstring clean,$(MAKECMDGOALS)),clean)
-include $(DEPS)
endif

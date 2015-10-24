CC=gcc
LDLIBS=-lncursesw -lpanel -lmenu -lform
NAME=mantra
CFLAGS=-g
OBJS=mantra.o file.o page.o input.o draw.o win/win.o win/bookmarks.o win/pages.o win/helpbar.o win/pnl.o

all: $(NAME)

$(NAME):

$(NAME): $(OBJS)

clean:
	rm *[.]o

install:
	cp mantra /usr/bin/

#include <ncurses.h>
#include <signal.h>

#include "draw.h"

void ncurses_init() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);
    curs_set(0);
    start_color();
    refresh();
}

void ncurses_close() {
    endwin();
}

int main(int argc, char** argv) {
    int ch = 0;

    ncurses_init();
    do {
        draw_screen();
    } while((ch = getch()) != 'q');
    ncurses_close();
}

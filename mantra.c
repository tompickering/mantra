#include <ncurses.h>
#include <signal.h>

#include "draw.h"
#include "win/win.h"

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
    int win_act;

    ncurses_init();
    win_init_all();
    do {
        switch (ch) {
            case -1:
                break;
            case ' ':
                win_act = (win_active() == WIN_IDX_PAGES) ? WIN_IDX_BOOKMARKS : WIN_IDX_PAGES;
                win_set_active(win_act);
        }
        draw_screen();
    } while((ch = getch()) != 'q');
    ncurses_close();
}

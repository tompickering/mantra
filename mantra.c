#include <ncurses.h>
#include <signal.h>

#include "page.h"
#include "draw.h"
#include "win/win.h"

volatile bool flag_sigwinch = false;

void handle_sigwinch(int sig) {
    flag_sigwinch = true;
}

void ncurses_init() {
    initscr();
    cbreak();
    noecho();
    halfdelay(1);
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

    signal(SIGWINCH, handle_sigwinch);

    ncurses_init();
    win_init_all();
    pages_init();
    do {
        switch (ch) {
            case -1:
                if (flag_sigwinch) {
                    flag_sigwinch = false;
                    endwin();
                    win_clear_all();
                    refresh();
                }
                break;
            case ' ':
                win_cycle_active();
        }
        draw_screen();
    } while((ch = getch()) != 'q');
    ncurses_close();
}

#include <signal.h>
#include <stdbool.h>

#include <ncurses.h>

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
    bool running = true;

    signal(SIGWINCH, handle_sigwinch);

    file_init();
    printf("Loading pages...\n");
    pages_init();
    ncurses_init();
    win_init_all();
    do {
        ch = getch();
        if (ch == -1) {
            if (flag_sigwinch) {
                flag_sigwinch = false;
                endwin();
                refresh();
                win_clear_all();
            }
        } else {
            running = handle_input(ch);
        }
        draw_screen();
    } while(running);
    ncurses_close();
}

#include <ncurses.h>
#include <signal.h>

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
                win_act = (win_active() == WIN_IDX_PAGES) ? WIN_IDX_BOOKMARKS : WIN_IDX_PAGES;
                win_set_active(win_act);
        }
        draw_screen();
    } while((ch = getch()) != 'q');
    ncurses_close();
}

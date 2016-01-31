/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * Mantra: Man page bookmarker                                           *
 * Copyright (C) 2015  Tom Pickering                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <signal.h>
#include <stdbool.h>

#include <ncurses.h>

#include "file.h"
#include "page.h"
#include "input.h"
#include "draw.h"
#include "win/win.h"

volatile bool flag_sigwinch = false;

/**
 * Registered as SIGWINCH handler
 * Set a flag to indicate that windows need
 * a complete redraw.
 */
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
    bool running = true;

    signal(SIGWINCH, handle_sigwinch);

    printf("Loading pages...\n");
    pages_init();

    printf("Loading bookmarks...\n");
    file_init();

    printf("Initialising...\n");
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

    file_close();

    return 0;
}

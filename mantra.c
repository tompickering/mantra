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

void ncurses_init() {
    initscr();
    cbreak();
    noecho();
    halfdelay(1);
    nonl();
    intrflush(stdscr, false);
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

    printf("Loading pages...\n");
    pages_init();

    printf("Loading bookmarks...\n");
    file_init();

    printf("Initialising...\n");
    ncurses_init();
    win_init_all();

    draw_screen();

    do {
        ch = getch();
        switch (ch) {
            case KEY_RESIZE:
                endwin();
                refresh();
                win_clear_all();
                draw_screen();
                break;
            case ERR:
                break;
            default:
                running = handle_input(ch);
                draw_screen();
                break;
        }
    } while(running);
    ncurses_close();

    file_close();

    return 0;
}

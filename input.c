#include "input.h"

#include <stdbool.h>

#include "win/win.h"

bool handle_input(int ch) {
    bool quit = false;
    switch (ch) {
        case K_CYCLE:
            win_cycle_active();
            break;
        case K_QUIT:
            quit = true;
            break;
        case K_BOOKPNL:
            open_panel(WIN_IDX_BOOKPNL);
            break;
        default:
            active_win()->input(ch);
    }
    return !quit;
}

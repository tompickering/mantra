#include "input.h"

#include <stdbool.h>

#include "win/win.h"

/**
 * First port of call for input characters.
 * Directs input to the appropriate function
 * or takes appropriate action depending on
 * the current context.
 */
bool handle_input(int ch) {
    bool quit = false;
    if (active_pnl() == NULL) {
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
    } else {
        active_pnl()->input(ch);
    }
    return !quit;
}

#ifndef GUARD_INPUT_H
#define GUARD_INPUT_H

#define CTRL      - 96
#define K_CYCLE   ' '
#define K_QUIT    'q'
#define K_DOWN    'j'
#define K_UP      'k'
#define K_HOME    'g'
#define K_END     'G'
#define K_OPEN    'o'
#define K_FWD     'f' + CTRL
#define K_BACK    'b' + CTRL
#define K_BOOKPNL 'b'
#define K_RETURN  '\n'

#include <stdbool.h>

bool handle_input(int ch);

#endif

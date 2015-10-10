#ifndef GUARD_INPUT_H
#define GUARD_INPUT_H

#define K_CYCLE ' '
#define K_QUIT  'q'
#define K_DOWN  'j'
#define K_UP    'k'
#define K_HOME  'g'
#define K_END   'G'

#include <stdbool.h>

bool handle_input(int ch);

#endif

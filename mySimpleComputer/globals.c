#include "mySimpleComputer.h"

int current_address = 0; 

// Define the window variables
WINDOW *term_win = NULL;
WINDOW *cache_win = NULL;
WINDOW *flag_win = NULL;
WINDOW *accumulator_win = NULL;
WINDOW *counter_win = NULL; 
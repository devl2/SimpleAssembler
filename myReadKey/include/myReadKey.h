#pragma once

#include "mySimpleComputer.h"

enum keys{
    N_KEY_UP,
    N_KEY_DOWN,
    N_KEY_LEFT,
    N_KEY_RIGHT,
    N_KEY_F5,
    N_KEY_F6,
    N_KEY_ENTER,
    N_KEY_ESC,
    N_KEY_OTHER,
    N_KEY_L,
    N_KEY_S,
    N_KEY_I,
    N_KEY_0,
    N_KEY_1,
    N_KEY_2,
    N_KEY_3,
    N_KEY_4,
    N_KEY_5,
    N_KEY_6,
    N_KEY_7,
    N_KEY_8,
    N_KEY_9,
    N_KEY_PLUS,
    N_KEY_MINUS,
    N_KEY_BACKSPACE,
    N_KEY_Q,
    N_KEY_W,
    N_KEY_R,
    N_KEY_T,
    N_KEY_A,
    N_KEY_B,
    N_KEY_C,
    N_KEY_D,
    N_KEY_E,
    N_KEY_F,
    N_KEY_X
};

int rk_readkey(enum keys *);
int rk_mytermsave (void);
int rk_mytermrestore (void);
int rk_mytermregime (int regime, int vtime, int vmin, int echo, int sigint);
int rk_readvalue (int *value, int timeout);

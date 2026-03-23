#pragma once
#include <mySimpleComputer.h>

int mt_clrscr(void);
int mt_gotoXY(int, int);
int mt_getscreensize(int * rows, int * cols);
int mt_setfgcolor(enum my_colors);
int mt_setbgcolor(enum my_colors);
int mt_setdefaultcolor(void);
int mt_setcursorvisible(int value);
int mt_delline(void);
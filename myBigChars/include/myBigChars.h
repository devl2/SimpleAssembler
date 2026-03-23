#pragma once
#include "mySimpleComputer.h"

int bc_strlen(char *str);
int bc_printA(char *str);
int bc_box(int x1, int y1, int x2, int y2, enum my_colors box_fg, enum my_colors box_bg, 
    char *header, enum my_colors header_fg, enum my_colors header_bg);
int bc_setbigchatpos(int *big, int x, int y, int value);
int bc_printbigchar(int [2], int x, int y, enum my_colors fg, enum my_colors bg, WINDOW * charwin);
int bc_bigcharwrite(int fd, int *big, int count);
int bc_bigcharread(int fd, int *big, int need_count, int *count);

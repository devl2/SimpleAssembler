#include "myTerm.h"

int mt_clrscr(void){
    printf("\033[2J\033[H");
    return 0;
}

int mt_gotoXY(int row, int col){
    printf("\033[%d;%dH", row, col);
    return 0;
}

int mt_getscreensize(int *rows, int *cols) {
    struct winsize{
        unsigned short row;
        unsigned short col;
        unsigned short xpixel;
        unsigned short ypixel;
    };

    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        return -1;
    }
    *rows = ws.row;
    *cols = ws.col;
    return 0;
}

int mt_setfgcolor(enum my_colors color) {
    if (color >= MY_COLOR_BLACK && color <= MY_COLOR_WHITE) {
        printf("\033[3%dm", color);
    } else if (color == MY_COLOR_DEFAULT) {
        printf("\033[39m");
    } else {
        return -1; 
    }
    return 0;
}

int mt_setbgcolor(enum my_colors color) {
    if (color >= MY_COLOR_BLACK && color <= MY_COLOR_WHITE) {
        printf("\033[4%dm", color);
    } else if (color == MY_COLOR_DEFAULT) {
        printf("\033[49m");
    } else {
        return -1;
    }
    return 0;
}

int mt_setdefaultcolor(void) {
    printf("\033[0m");
    return 0;
}

int mt_setcursorvisible(int value) {
    if (value) {
        printf("\033[?25h");
    } else {
        printf("\033[?25l");
    }
    return 0;
}

int mt_delline(void) {
    printf("\033[2K");
    return 0;
}


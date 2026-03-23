#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <wchar.h>
#include <locale.h>
#include <termios.h>
#include <ctype.h>
#include <signal.h>
#define COMMAND_MASK 0X7F
#define OPERAND_MASK  0x7F
#define SIGN_MASK     0x4000
#define COMMAND_SIGN_MASK 16384 
#define COMMAND_SHIFT 7 
#define MEMORY_SIZE 128
#define MAX_LINES 4
#define FLAG_OVERFLOW  0x01 
#define FLAG_DIVISION  0x02  
#define FLAG_OUTOFMEM  0x04  
#define FLAG_INVALID   0x08
#define FLAG_STOP      0x10

#define MEMORY_HEIGH 13
#define MEMORY_WIDTH (10*8 + 5)
#define ACCUMULATOR_HEIGH 5
#define ACCUMULATOR_WIDTH (10*3 + 5)

#define DECODED_HEIGH 5
#define DECODED_WIDTH (10*7 + 5)

#define FLAG_HEIGH 5
#define FLAG_WIDTH (10*3 + 5)

#define COUNTER_HEIGH 5
#define COUNTER_WIDTH (10*3 + 5)

#define COMMAND_HEIGH 5
#define COMMAND_WIDTH (10*3 + 5)

#define TERM_HEIGH 7
#define TERM_WIDTH 17

#define BIG_HEIGH 13
#define BIG_WIDTH  2*(10*3 + 5)
#define BIG_SIZE 18

#define KEY_WIDTH (10*3 + 5)

#define CACHE_HEIGH 7
#define CACHE_WIDTH (10*8 + 8)

#define TERMINAL_HEIGH 5
#define TERMINAL_WIDTH (10*7 + 8)

extern int data[MEMORY_SIZE];
extern int FLAG;
extern int acum;
extern int schet;
extern int takt;
extern int stop;
extern int cache_check;
extern int cache_check2;
extern int cache_jump;
extern int cache_print;
extern int cache_print_addr;
extern int current_address;

extern WINDOW *term_win;
extern WINDOW *cache_win;
extern WINDOW *flag_win;
extern WINDOW *accumulator_win;
extern WINDOW *counter_win;

enum my_colors {
    MY_COLOR_BLACK = 0,
    MY_COLOR_RED,
    MY_COLOR_GREEN,
    MY_COLOR_YELLOW,
    MY_COLOR_BLUE,
    MY_COLOR_MAGENTA,
    MY_COLOR_CYAN,
    MY_COLOR_WHITE,
    MY_COLOR_DEFAULT
};

//model op
int sc_memoryInit (void);
int sc_memorySet (int address, int value);
int sc_memoryGet (int address, int* value);
int sc_memorySave (char * filename);
int sc_memoryLoad(char * filename);

//model reg
int sc_regInit(void);
int sc_regSet(int Register,int value);
int sc_regGet(int Register, int * value);
int sc_accumulatorInit(void);
int sc_accumulatorSet(int value);
int sc_accumulatorGet(int* value);
int sc_icounterInit(void);
int sc_icounterSet(int value);
int sc_icounterGet(int* value);

// model YY
int sc_commandValidate(int command);
int sc_commandEncode(int sign, int command, int operand, int* value);
int sc_commandDecode(int value, int *sign, int *command, int *operand);

// model print
void printCell(int address,enum my_colors fg, enum my_colors bg,WINDOW *memory_win);
void printFlags(WINDOW *flag_win);
void printDecidedCommand(int value, WINDOW *decoded_win);
void printAccumulator(WINDOW *accumulator_win);
void printCounters(WINDOW *counter_win);
void printCommand(int address,WINDOW *command_win);
void printTerm(int address, int input,WINDOW *term_win);
void initPrintWindows(void);

// Add missing function declarations
void printCache(int address, int input, WINDOW *cache_win);
void edit_cell(int address, WINDOW *memory_win);
void ALU(int command, int operand);
void init(void);
int update_cache(int address);
void edit_accum(WINDOW *accumulator_win);
void edit_icounter(WINDOW *counter_win);
void resetSystem(void);
void printHexBigChar(int address, WINDOW *char_win);
void printKeys(WINDOW *key_win);
void printname(WINDOW *terminal_win);
void printcell_cache(int address, WINDOW *terminal_win);
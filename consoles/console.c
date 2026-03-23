#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdio.h>   
#include <stdlib.h>  


#include "mySimpleComputer.h"
#include "myBigChars.h"
#include "myReadKey.h"
#include "myTerm.h"

volatile sig_atomic_t running_mode = 0;
volatile sig_atomic_t need_step = 0;
extern int current_address;

#define MIN_ROWS 30
#define MIN_COLS 80
#define MEMORY_Y 10
#define MEMORY_X 20
#define INOUT_Y 10
#define INOUT_X 40
#define MEMORY_CELLS 10
#define INOUT_CELLS 7
#define STDIN_FILENO 0 
#define STDOUT_FILENO 1 
#define STDERR_FILENO 2 

// These windows are defined in globals.c
extern WINDOW *term_win;
extern WINDOW *cache_win;
extern WINDOW *flag_win;
extern WINDOW *accumulator_win;
extern WINDOW *counter_win;

// These windows are local to console.c
WINDOW *memory_win;
WINDOW *decoded_win;
WINDOW *command_win;
WINDOW *key_win;
WINDOW *load_win;
WINDOW *terminal_win;

int is_terminal() { 
    return isatty(STDOUT_FILENO); 
}

int check_screen_size() {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    printf("rows = %d cols = %d",rows , cols );
    if (rows < MIN_ROWS || cols < MIN_COLS) {
        return 0; 
    }
    return 1;
}

void autoSaveAndDisplayCache() {
    printCache(current_address, 0, cache_win);
}

void CU(void) {
    int value, sign, command, operand;
    
    sc_memoryGet(schet, &value);
    if (sc_commandDecode(value, &sign, &command, &operand)) {
        sc_regSet(FLAG_INVALID, 1);
        return;
    }

    switch (command) {
        case 0x10: // READ
            printTerm(schet, 1, term_win);
            edit_cell(operand, memory_win);
            printTerm(schet, 0, term_win);
            schet++;
            break;
            
        case 0x11: // WRITE
            printTerm(schet, 1, term_win);
            printCell(operand, MY_COLOR_YELLOW, MY_COLOR_WHITE, memory_win);
            printTerm(schet, 0, term_win);
            schet++;
            break;

        case 0x43: // HALT
            sc_regSet(FLAG_STOP, 1);
            stop = 1;
            break;

        case 0x20: // LOAD
        case 0x21: // STORE
        case 0x30: // ADD
        case 0x31: // SUB
        case 0x32: // DIVIDE
        case 0x33: // MUL
        case 0x40: // JUMP
        case 0x41: // JNEG
        case 0x42: // JZ
            ALU(command, operand);
            printAccumulator(accumulator_win);
            printFlags(flag_win);
            schet++;
            break;
    }
}

void IRC(int signum) {
    if (signum == SIGALRM && running_mode) {
        int t_flag;
        sc_regGet(FLAG_STOP, &t_flag);
        if (!t_flag) CU();
    }
    else if (signum == SIGUSR1) {
        init();
        running_mode = 0;
    }
}

static void signal_handler(int signum) {
    IRC(signum);
}

void init_signals(void) {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    
    sigaction(SIGALRM, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
}

int main() {
    current_address = 0;
    enum keys key;
    sc_memoryInit();
    sc_regInit();
    sc_accumulatorInit();
    sc_icounterInit();

    for (int i = 0; i < MEMORY_SIZE; i++) {
        sc_memorySet(i, i + 1);    
    }

    sc_memorySet(0, -1);
    sc_memorySet(1, -2);
    sc_memorySet(2, -3);
    initscr();
    start_color();
    noecho();
    if (!is_terminal()) {
        fprintf(stderr, "Ошибка: вывод должен быть направлен в терминал.\n");
        return 1;
    }
    if (!check_screen_size()){ fprintf(stderr, "ERROR SIZE\n"); return 1; }

    refresh();
    memory_win = newwin(MEMORY_HEIGH, MEMORY_WIDTH, 1, 1);
    accumulator_win = newwin(ACCUMULATOR_HEIGH, ACCUMULATOR_WIDTH, 1, MEMORY_WIDTH + 5);
    decoded_win = newwin(DECODED_HEIGH, DECODED_WIDTH + 10, MEMORY_HEIGH + 1, 1 );
    flag_win = newwin(FLAG_HEIGH, FLAG_WIDTH, 1, MEMORY_WIDTH + ACCUMULATOR_WIDTH + 5 );
    counter_win = newwin(COUNTER_HEIGH, COUNTER_WIDTH, ACCUMULATOR_HEIGH + 1, MEMORY_WIDTH + 5 );
    command_win = newwin(COMMAND_HEIGH, COMMAND_WIDTH, ACCUMULATOR_HEIGH + 1, MEMORY_WIDTH + ACCUMULATOR_WIDTH + 5 );
    term_win = newwin(TERM_HEIGH, TERM_WIDTH, BIG_HEIGH + ACCUMULATOR_HEIGH + COMMAND_HEIGH + 1, MEMORY_WIDTH + ACCUMULATOR_WIDTH - 17 );
    key_win = newwin(TERM_HEIGH, TERM_WIDTH + 23, BIG_HEIGH + ACCUMULATOR_HEIGH + COMMAND_HEIGH + 1, MEMORY_WIDTH + ACCUMULATOR_WIDTH);
    load_win = newwin(DECODED_HEIGH, DECODED_WIDTH + 20, MEMORY_HEIGH + 25, 1);
    cache_win = newwin(CACHE_HEIGH, CACHE_WIDTH + 5,BIG_HEIGH + ACCUMULATOR_HEIGH + COMMAND_HEIGH + 1, 1);
    terminal_win = newwin(TERMINAL_HEIGH, TERMINAL_WIDTH, CACHE_HEIGH + BIG_HEIGH + ACCUMULATOR_HEIGH + COMMAND_HEIGH + 3, 1);

    box(memory_win, 0, 0); 
    box(accumulator_win, 0, 0); 
    box(decoded_win, 0, 0); 
    box(flag_win, 0, 0);  
    box(counter_win, 0, 0);
    box(command_win, 0, 0);
    box(term_win, 0, 0);
    box(key_win, 0, 0);
    box(cache_win, 0, 0);
    box(terminal_win, 0, 0);

    mvwprintw(memory_win, 0, MEMORY_WIDTH / 2, "MEMORY");
    wrefresh(memory_win);
    mvwprintw(accumulator_win, 0, ACCUMULATOR_WIDTH / 2 - 4, "ACCUMULATOR");
    wrefresh(accumulator_win);
    mvwprintw(decoded_win, 0, DECODED_WIDTH / 2 - 10, "REDACTED CELL (FORMAT)");
    wrefresh(decoded_win);
    mvwprintw(flag_win, 0, FLAG_WIDTH / 2 - 2, "FLAGS");
    wrefresh(flag_win);
    mvwprintw(counter_win, 0, COUNTER_WIDTH / 2 - 3, "COUNTER");
    wrefresh(counter_win);
    mvwprintw(command_win, 0, COMMAND_WIDTH / 2 - 3, "COMMAND");
    wrefresh(command_win);
    mvwprintw(term_win, 0, TERM_WIDTH / 2 - 3, "IN--OUT");
    wrefresh(term_win);
    mvwprintw(key_win, 0, TERM_WIDTH / 2 + 7, "KEYS");
    wrefresh(key_win);
    mvwprintw(cache_win, 0, CACHE_WIDTH / 2 - 5, "PROCCESSOR CACHE");
    wrefresh(cache_win);

    mvwprintw(terminal_win, 0, CACHE_WIDTH / 2 - 5, "TERMINAL");
    wrefresh(terminal_win);
    
    sc_accumulatorSet(100);

    printAccumulator(accumulator_win);
    setlocale(LC_ALL, "");
    WINDOW *char_win = newwin(BIG_HEIGH, BIG_WIDTH, ACCUMULATOR_HEIGH + COMMAND_HEIGH + 1, MEMORY_WIDTH + 5);
    box(char_win, 0, 0);
    mvwprintw(char_win, 0, BIG_WIDTH / 2 - 5, "Big Size of cell");
    wrefresh(char_win);
    
    sc_regSet (FLAG_OVERFLOW, 0);
    sc_regSet (FLAG_DIVISION, 0);
    sc_regSet (FLAG_OUTOFMEM, 0);
    sc_regSet (FLAG_STOP, 1);
    sc_regSet (FLAG_INVALID, 0);
    printFlags(flag_win);
    
    sc_icounterSet(0);
    printCounters (counter_win);

    printCommand(data[0],command_win);

    initPrintWindows();  // Initialize print windows

    for (int i = 0; i < MEMORY_SIZE; i++) 
    {
        if (i == current_address) { printCell(i, MY_COLOR_YELLOW, MY_COLOR_WHITE, memory_win);}
        else {printCell(i, MY_COLOR_YELLOW, MY_COLOR_BLACK, memory_win);}
    }
    // for (int i = 0; i < 7; ++i)
    // {
    //     printTerm (i, 1, term_win);
    // }

    bool ff = true;
    char * filename;
    rk_mytermregime(1, 0, 1, 0, 1);
    int wh = 1; int left =0, right = 0,time = 0;
    while (wh) 
    { 
        sc_regSet (FLAG_INVALID, 0);
        for( int i = 0; i < MEMORY_SIZE; i++)
        {
            if (data[i] < 0 && ff){sc_regSet (FLAG_INVALID, 1); ff = false;}
        }
        ff = true;
        enum keys key;
        if (rk_readkey(&key) == -1 && !FLAG_STOP ) continue;
        switch (key) 
        {
            case N_KEY_UP:
                current_address = (current_address - 12 + MEMORY_SIZE) % MEMORY_SIZE;
                break;
            case N_KEY_DOWN:
                current_address = (current_address + 12) % MEMORY_SIZE; 
                break;
            case N_KEY_LEFT:
                current_address = (current_address - 1 + MEMORY_SIZE) % MEMORY_SIZE;
                break;
            case N_KEY_RIGHT:
                current_address = (current_address + 1) % MEMORY_SIZE;
                break;
            case N_KEY_ENTER:
                printTerm(current_address, 0, term_win);
                edit_cell(current_address, memory_win);
                update_cache(current_address);
                break;
            case N_KEY_9:
                edit_accum(accumulator_win);
                break;
            case N_KEY_0:
                edit_icounter(counter_win);
                printCommand(data[schet],command_win);
                break;
            case N_KEY_L: {
                char filename[256];
                echo();
                curs_set(1);
                mvwprintw(load_win, 1, 0, "Load name: ");
                wrefresh(load_win);
                wgetnstr(load_win, filename, sizeof(filename) - 1);
                noecho();
                curs_set(0);
                    
                if (sc_memoryLoad(filename) == 0) {
                    mvwprintw(load_win, 1, 0, "Load success '%s'", filename);
                    acum = 0;  // Сброс аккумулятора
                    printAccumulator(accumulator_win);  // Обновление отображения
                }
                wrefresh(load_win);
                wclear(load_win);
                napms(2000);
                break;
            }
            case N_KEY_S: {
                char filename[256]; 
                echo();
                curs_set(1);
                mvwprintw(load_win, 0, 0, "Save name: ");
                wrefresh(load_win);
                wgetnstr(load_win, filename, sizeof(filename) - 1);
                noecho();
                curs_set(0);
                
                
                if (sc_memorySave(filename) == 0) {
                    mvwprintw(load_win, 1, 0, "Save success in '%s'", filename);
                }
                wrefresh(load_win);
                wclear(load_win);
                napms(2000);
                break;
            }
            case N_KEY_I:
                resetSystem();
                break;
            case N_KEY_T:
                left  = (data[current_address] >> COMMAND_SHIFT) & OPERAND_MASK;
                right = data[current_address] & OPERAND_MASK;
                ALU(left, current_address);
                int is_cached = update_cache(current_address); 

                if (is_cached == 1) {
                    time = 2;
                } else {
                    time = 8;
                }
                for (int i =0; i < time; i++)
                {
                    if (takt <= 0){takt  = 8;}
                    napms(1000);
                    takt--;
                    printCounters (counter_win);
                    
                }
                time = 0;
                
                if (cache_check == 1){printCache(right, 1, cache_win); cache_check = 0;}
                
                break;  
            case N_KEY_R:{
                    rk_mytermregime(0, 0, 0, 0, 0); 
                    sc_regSet (FLAG_STOP, 0);
                    for(int i = 0; i < MEMORY_SIZE; i++ ) {
                        i = current_address;
                        left  = (data[current_address] >> COMMAND_SHIFT) & OPERAND_MASK;
                        right = data[current_address] & OPERAND_MASK;
                        ALU(left, current_address);
                        printTerm(current_address, 1, term_win);
                        printCell(i, MY_COLOR_YELLOW, MY_COLOR_WHITE, memory_win);
                        printDecidedCommand(data[i],decoded_win);
                        printHexBigChar(i, char_win);
                        printFlags(flag_win);
                        printAccumulator(accumulator_win);
                        printKeys(key_win);
                        printCounters (counter_win);
                        if (cache_print == 1){printname(terminal_win);}
                        if (cache_print_addr == 1) { printcell_cache(right, terminal_win); cache_print_addr = 0;}
                       
                        int is_cached = 0; 
                        if(cache_check == 1){is_cached = update_cache(right);} 
                        else {is_cached = update_cache(current_address);}
                        if (cache_check2== 1)
                        {
                            printTerm(right, 0, term_win);
                            edit_cell(right, memory_win);
        
                            cache_check2=0;
                        }
                        time = (is_cached == 1) ? 2 : 8;
                        takt = time;
                        for (int i = 0; i < time; i++)
                        {
                            printCounters (counter_win);
                            napms(1000);
                            takt--;
                            
                            
                        }
                        time = 0;
                        if (stop == 1) { break;}
                        if (cache_check == 1){printCache(right, 1, cache_win); cache_check = 0;}
                        else {printCache(current_address, 1, cache_win); cache_check = 0;}
                        printCell(i, MY_COLOR_YELLOW, MY_COLOR_BLACK, memory_win);
                        if (cache_jump != 1){current_address++;}
                        else {cache_jump = 0;}
                        
                    }
                    sc_regSet (FLAG_STOP, 1);     
                    current_address = 0;  
                    cache_print = 0;
                    stop = 0;
                    rk_mytermregime(1, 0, 1, 0, 1);      
                    break;
            }
            case N_KEY_ESC:
                wh = 0;
                break;
            default:
                key = N_KEY_OTHER;
                break;
        }
      werase(char_win);
      box(char_win, 0, 0);
      mvwprintw(char_win, 0, BIG_WIDTH / 2 - 5, "Big Size of cell");
      for (int i = 0; i < MEMORY_SIZE; i++) {
          if (i == current_address) 
          {
            printTerm(current_address, 1, term_win);
            printCell(i, MY_COLOR_YELLOW, MY_COLOR_WHITE, memory_win);
            printDecidedCommand(data[i],decoded_win);
            printHexBigChar(i, char_win);
            printFlags(flag_win);
            
            printAccumulator(accumulator_win);
            printKeys(key_win);
          } else {
              printCell(i, MY_COLOR_YELLOW, MY_COLOR_BLACK, memory_win);   
          }
        }
        printCommand(data[schet],command_win);
        printCounters (counter_win);
        autoSaveAndDisplayCache();
        
      wrefresh(memory_win);
    }
    delwin(memory_win);
    delwin(accumulator_win);
    delwin(decoded_win);
    delwin(flag_win);
    delwin(term_win);
    endwin();

    return 0;
}

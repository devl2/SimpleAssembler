    #include "mySimpleComputer.h"
    #include "myBigChars.h"
    #include "myReadKey.h"
    #include "myTerm.h"
    #define TERM_SIZE 4

  #define CACHE_SIZE 5
#define CACHE_LINE_SIZE 12

static int cache_addresses[CACHE_SIZE] = {-1, -1, -1, -1, -1};
static int cache_last_used[CACHE_SIZE] = {-1,-1,-1,-1,-1}; 

int update_cache(int address) {
    int line_start = (address / CACHE_LINE_SIZE) * CACHE_LINE_SIZE;
    int per = 0;

    
    int found_index = -1;
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache_addresses[i] == line_start) {
            found_index = i;
            per = 1;
            break;
        }
    }

    if (found_index == -1) {       
        for (int i = CACHE_SIZE - 1; i > 0; i--) {
            cache_addresses[i] = cache_addresses[i-1];
            cache_last_used[i] = cache_last_used[i-1];
        }
        cache_addresses[0] = line_start;
        cache_last_used[0] = 0;
    } else {
       
        int temp_addr = cache_addresses[found_index];
        for (int i = found_index; i > 0; i--) {
            cache_addresses[i] = cache_addresses[i-1];
            cache_last_used[i] = cache_last_used[i-1];
        }

        cache_addresses[0] = temp_addr;
        cache_last_used[0] = CACHE_SIZE;
    }

    for (int i = 1; i < CACHE_SIZE; i++) {
        if (cache_last_used[i] > 0) {
            cache_last_used[i]--;
        }
    }

    return per;
}
    
    static int big_chars[BIG_SIZE][2] = {
        {0b1111110001100011000111111, 0},
        {0b1110000100001000010011111, 1},
        {0b0111010001001100100011111, 2},
        {0b1111000001111100000111110 , 3},
        {0b1000110001111110000100001, 4},
        {0b1111110000111100000111110, 5},
        {0b1111110000111111000111111, 6},
        {0b1111000010001000100001000, 7},
        {0b0111010001011101000101110, 8},
        {0b1111110001111110000111110, 9},
        {0b1111110001111111000110001, 10},
        {0b1111010001111101000111110, 11},
        {0b0111110000100001000001111 , 12},
        {0b1110010010100011001011100, 13},
        {0b1111110000111111000011111, 14},
        {0b1111110000111111000010000 , 15},
        {0b0010000100111110010000100, 16},
        {0b0000000000111110000000000, 17}
    };

    void printCell(int address, enum my_colors header_fg, enum my_colors bg, WINDOW *memory_win)
    {
        
        if (bg != MY_COLOR_BLACK){init_pair(2, header_fg, bg); wattron(memory_win, COLOR_PAIR(2));}
        else {init_pair(1, header_fg, bg);wattron(memory_win, COLOR_PAIR(1));}
        int left  = (data[address] >> COMMAND_SHIFT) & COMMAND_MASK;
        int right = data[address] & OPERAND_MASK;
        
        int col = address % 12;
        int row = address / 12;
        mvwprintw(memory_win, row + 1 , col*7 + 1, " %c%02X%02X",(data[address] < 0) ? '-': '+',left,right );
            
        wattroff(memory_win, COLOR_PAIR(1));
        wattroff(memory_win, COLOR_PAIR(2));
        wrefresh(memory_win);
    }
    void printname(WINDOW *terminal_win)
    {
        
        mvwprintw(terminal_win, 1, 1, "Martsenovich Danila Sergeevich IP314");
        mvwprintw(terminal_win, 2, 1, "Bokiy Vladislav Olegovich IP314");
        wrefresh(terminal_win);
    }
    void printcell_cache(int address,WINDOW *terminal_win)
    {
        int left = (data[address] >> COMMAND_SHIFT) & COMMAND_MASK;
        int right = data[address] & OPERAND_MASK;
         mvwprintw(terminal_win, 3, 1, "%02d :  %c%02X%02X",address,  (data[address] < 0) ? '-' : '+', left, right);
        
        wrefresh(terminal_win);
    }

    void printFlags(WINDOW *flag_win)
    {
        if (sc_regGet (0, &FLAG) == 0)
        {
            mvwprintw(flag_win,FLAG_HEIGH / 2, 10, "%c  %c  %c  %c  %c", 
                (FLAG & FLAG_OVERFLOW) ? 'P' : '_',
                (FLAG & FLAG_DIVISION) ? '0' : '_',
                (FLAG & FLAG_OUTOFMEM) ? 'M' : '_',
                (FLAG & FLAG_STOP) ? 'T' : '_',
                (FLAG & FLAG_INVALID) ? 'E' : '_');
        }
        wrefresh(flag_win);
    }

    void printDecidedCommand(int value, WINDOW *decoded_win)
    {
        mvwprintw(decoded_win,DECODED_HEIGH / 2, 1, "dec: %04d | oct: %04o | hex: %04X | bin: ", value, value, value & 0x7FFF);
        for (int i = 14; i >= 0; i--) {
            mvwprintw(decoded_win,DECODED_HEIGH / 2, 65-i, "%d", (value  >> i) & 1);
        }
        wrefresh(decoded_win);
        
    }

    void printAccumulator(WINDOW *accumulator_win)
    {
        int left  = (acum >> COMMAND_SHIFT) & COMMAND_MASK;
        int right = acum & OPERAND_MASK;
        mvwprintw(accumulator_win, ACCUMULATOR_HEIGH / 2 , 5 , "sc: %c%02X%02X hex:  %c%04X ",(acum >= 0) ?'+' : '-',left,right,(acum >= 0) ?'+' : '-', acum & 0x7FFF );
            
        wrefresh(accumulator_win);
    }

    void printCounters(WINDOW *counter_win)
    {
        sc_icounterGet(&schet);
        mvwprintw(counter_win, COUNTER_HEIGH / 2 , 5 , "T: %04d IC:  %04X", takt, current_address);
        wrefresh(counter_win);
    }


    char lines[MAX_LINES][64];
    int count = 0;
    int term_history[TERM_SIZE] = { -1, -1, -1, -1 };
    int input_history[TERM_SIZE] = { -1, -1, -1, -1 };
    int data_history[TERM_SIZE] = { -1, -1, -1, -1 };
    
    void initPrintWindows(void) {
        werase(term_win);
        box(term_win, 0, 0);
        mvwprintw(term_win, 0, TERM_WIDTH / 2 - 3, "IN--OUT");
        wrefresh(term_win);
    }

    void printTerm(int address, int input, WINDOW *term_win)
    {
        for (int i = TERM_SIZE - 1; i > 0; i--) 
        {
            term_history[i] = term_history[i-1];
            input_history[i] = input_history[i-1];
            data_history[i] = data_history[i-1];
        }
        term_history[0] = address;
        input_history[0] = input;
        data_history[0] = data[address];

        for (int i = 0; i < TERM_SIZE; i++)
        {
            if (term_history[i] == -1){ continue;}
            else 
            {
                mvwprintw(term_win, i + 1, 3, "%02d %c ", term_history[i], input_history[i] ? '>' : '<' );
                int left  = (data_history[i] >> COMMAND_SHIFT) & COMMAND_MASK;
                int right = data_history[i] & OPERAND_MASK;
                mvwprintw(term_win, i + 1, 8 , " %c%02X%02X", data_history[i] < 0 ? '-' : '+', left,right );
            }
            
        }
        wrefresh(term_win);
    }

    void printCache(int address, int input, WINDOW *cache_win) {
    if (input) {
        update_cache(address);
    }
    
    werase(cache_win);
    box(cache_win, 0, 0);
    mvwprintw(cache_win, 0, CACHE_WIDTH / 2 - 7, "PROCESSOR CACHE");
    
    
    for (int i = 0; i < CACHE_SIZE; i++) {
        int addr = cache_addresses[i];
        if (addr == -1) {
            mvwprintw(cache_win, i + 1, 1, "-1");
            continue;
        }
        
        mvwprintw(cache_win, i + 1, 1, "%02d:", addr);
        
        for (int j = 0; j < CACHE_LINE_SIZE; j++) {
            int cell_addr = addr + j;
            if (cell_addr >= MEMORY_SIZE) continue;
            
            int left = (data[cell_addr] >> COMMAND_SHIFT) & COMMAND_MASK;
            int right = data[cell_addr] & OPERAND_MASK;
            
            if (cell_addr == address && input) {
                wattron(cache_win, A_REVERSE);
                mvwprintw(cache_win, i + 1, 5 + j * 6, " %c%02X%02X", 
                         (data[cell_addr] < 0) ? '-' : '+', left, right);
                wattroff(cache_win, A_REVERSE);
            } else {
                mvwprintw(cache_win, i + 1, 5 + j * 6, " %c%02X%02X", 
                         (data[cell_addr] < 0) ? '-' : '+', left, right);
            }
        }
    }
    
    wrefresh(cache_win);
}

    void printCommand(int address,WINDOW *command_win)
    {
        int value = address, sign,command,operand;
        sc_icounterGet(&value);
        if (sc_commandDecode (value, &sign, &command, &operand) == 0)
        {
            mvwprintw(command_win, COMMAND_HEIGH / 2 , 12 , "%c", sign ? '!' : ' ' );
            mvwprintw(command_win, COMMAND_HEIGH / 2 , 13 , "%c%02X : %02X", sign ? '-' : '+', command, operand );
        }
        
        wrefresh(command_win);
    }
    
void printHexBigChar(int address, WINDOW *char_win) {
    int value = data[address];
    
    int sign_bit = (value & SIGN_MASK) ? 1 : 0;
    int command = (value >> COMMAND_SHIFT) & COMMAND_MASK;
    int operand = value & OPERAND_MASK;

    int x_pos = 5;
    
    int sign_char = sign_bit ? 17 : 16; 
    bc_printbigchar(big_chars[sign_char], x_pos, 3,  MY_COLOR_WHITE, MY_COLOR_BLACK, char_win); 
    x_pos += 12;

    int cmd_high = (command >> 4) & 0xF;
    int cmd_low = command & 0xF;
    bc_printbigchar(big_chars[cmd_high], x_pos, 3, MY_COLOR_WHITE, MY_COLOR_BLACK, char_win);
    x_pos += 12;
    bc_printbigchar(big_chars[cmd_low], x_pos, 3,  MY_COLOR_WHITE, MY_COLOR_BLACK, char_win);
    x_pos += 12;
    
    int op_high = (operand >> 4) & 0xF;
    int op_low = operand & 0xF;
    bc_printbigchar(big_chars[op_high], x_pos, 3, MY_COLOR_WHITE, MY_COLOR_BLACK, char_win);
    x_pos += 12;
    bc_printbigchar(big_chars[op_low], x_pos, 3,  MY_COLOR_WHITE, MY_COLOR_BLACK, char_win);

    mvwprintw(char_win, BIG_HEIGH - 2, 1 , "cell #%d", address );
    
    wrefresh(char_win);
}

int bc_printbigchar(int big[2], int x, int y, enum my_colors fg, enum my_colors bg, WINDOW *char_win) {
    if (!big || !char_win) return -1;
    
    init_pair(3, fg, bg);
    wattron(char_win, COLOR_PAIR(3));
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int pos = i * 5 + j;
            int mask = (pos < 25) ? (1 << (24 - pos)) : 0;
            
            if (big[0] & mask) {
                mvwaddch(char_win, y + i, x + j * 2, ACS_CKBOARD);
                mvwaddch(char_win, y + i, x + j * 2 + 1, ACS_CKBOARD);
            } else {
                mvwaddch(char_win, y + i, x + j * 2, ' ');
                mvwaddch(char_win, y + i, x + j * 2 + 1, ' ');
            }
        }
    }
    
    wattroff(char_win, COLOR_PAIR(1));
    wrefresh(char_win);
    return 0;
}

void printKeys(WINDOW *key_win) {
    werase(key_win);
    box(key_win, 0, 0);
    
    mvwprintw(key_win, 0, TERM_WIDTH / 2 - 2, "KEYS");
    
    mvwprintw(key_win, 1, 1, "l - load  s - save  i - reset");
    mvwprintw(key_win, 2, 1, "r - run  t - step");
    mvwprintw(key_win, 3, 1, "ESC - exit");
    mvwprintw(key_win, 4, 1, "F5(9) - accumulator");
    mvwprintw(key_win, 5, 1, "F6(0) - instruction counter");
    
    wrefresh(key_win);
}

void resetSystem(void) {
    // Сброс памяти
    sc_memoryInit();
    
    // Сброс регистров
    sc_regInit();
    sc_regSet(FLAG_OVERFLOW, 0);
    sc_regSet(FLAG_DIVISION, 0);
    sc_regSet(FLAG_OUTOFMEM, 0);
    sc_regSet(FLAG_STOP, 1);
    sc_regSet(FLAG_INVALID, 0);
    
    // Сброс аккумулятора и счетчика
    acum = 0;
    schet = 0;
    takt = 8;
    stop = 0;
    
    // Сброс кэша
    for (int i = 0; i < CACHE_SIZE; i++) {
        cache_addresses[i] = -1;
        cache_last_used[i] = -1;
    }
    
    // Сброс истории терминала
    for (int i = 0; i < TERM_SIZE; i++) {
        term_history[i] = -1;
        input_history[i] = -1;
        data_history[i] = -1;
    }
    
    // Очистка окон
    werase(term_win);
    box(term_win, 0, 0);
    mvwprintw(term_win, 0, TERM_WIDTH / 2 - 3, "IN--OUT");
    wrefresh(term_win);
    
    werase(cache_win);
    box(cache_win, 0, 0);
    mvwprintw(cache_win, 0, CACHE_WIDTH / 2 - 7, "PROCESSOR CACHE");
    wrefresh(cache_win);
    
    // Обновление отображения
    printFlags(flag_win);
    printAccumulator(accumulator_win);
    printCounters(counter_win);
    printCache(0, 0, cache_win);
}
#include "mySimpleComputer.h"
#include "myReadKey.h"
#include <signal.h>

//volatile sig_atomic_t running_mode = 0; // 0 - интерактивный, 1 - режим работы модели
//volatile sig_atomic_t need_step = 0;    // Флаг для шага выполнения

int data[MEMORY_SIZE]; 
int FLAG = 0;      
int acum = 0;     
int schet = 0;
int takt = 8;
int stop =0;
int cache_check = 0;
int cache_check2 = 0;
int cache_jump = 0;
int cache_print = 0;
int cache_print_addr = 0;
extern int current_address;


int sc_memoryInit (void)
{
    for(int i = 0; i < MEMORY_SIZE; i++){ data[i] = 0;}
    return 0;
}
int sc_memorySet (int address, int value)
{
    data[address] = value;
    return 0;
}
int sc_memoryGet (int address, int* value)
{
    if (address < 0 || address >= MEMORY_SIZE || !value)
    {
      return -1;
    }
	*value = data[address];
	return 0;
}
int sc_memorySave (char * filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file) {
		fprintf(stderr, "File not open! \n");
		return -1;
	}
	
	fwrite(data, MEMORY_SIZE, sizeof(int), file);
	fclose(file);

	return 0;
}
int sc_memoryLoad(char * filename){
    FILE *file = fopen(filename, "rb");
    if (!file) {
		fprintf(stderr, "File not open! \n");
		return -1;
	}
	
	fread(data, MEMORY_SIZE, sizeof(int), file);
	fclose(file);
	return 0;
};

void edit_cell(int address, WINDOW *win) {
    char input[10] = {0}; 
    int pos = 0;
    int value;
    enum keys key;
    
    sc_memoryGet(address, &value);
    
    rk_mytermregime(1, 0, 1, 0, 1);
    curs_set(1);

    int row = address / 12;
    int col = address % 12;
    mvwprintw(win, row + 1, col * 7 + 2, "        ");
    wmove(win, row + 1, col * 7 + 2);
    wrefresh(win);

    while (1) {
        if (rk_readkey(&key)) continue;

        if (key == N_KEY_ENTER) {
            break;
        }
        
        if (key == N_KEY_ESC) {
            pos = 0;
            break;  
        }

        if (key == N_KEY_BACKSPACE) {
            if (pos > 0) {
                pos--;
                input[pos] = '\0';
                mvwaddch(win, row + 1, col * 7 + 2 + pos, ' ');
                wmove(win, row + 1, col * 7 + 2 + pos);
                wrefresh(win);
            }
            continue;
        }

        if ((key >= N_KEY_0 && key <= N_KEY_9) || 
            (key >= N_KEY_A && key <= N_KEY_F) ||
            (pos == 0 && (key == N_KEY_PLUS || key == N_KEY_MINUS)) ||
            (pos == 1 && key == N_KEY_X)) {
            
            if (pos < 9) { 
                char c;
                if (key >= N_KEY_0 && key <= N_KEY_9) {
                    c = '0' + (key - N_KEY_0);
                } else if (key >= N_KEY_A && key <= N_KEY_F) {
                    c = 'a' + (key - N_KEY_A);
                } else if (key == N_KEY_X) {
                    c = 'x';
                } else {
                    c = (key == N_KEY_PLUS) ? '+' : '-';
                }
                
                input[pos++] = c;
                waddch(win, c);
                wrefresh(win);
            }
        }
    }

    curs_set(0);
    rk_mytermregime(1, 0, 1, 0, 1);

    if (pos > 0) {
        if (pos > 2 && input[0] == '0' && input[1] == 'x') {
            value = (int)strtol(input, NULL, 16);
        } else {
            value = atoi(input);
        }
        
        if (value > 16383 || value < -16383) { 
            value = 16383;
            sc_regSet(FLAG_OVERFLOW, 1); 
        } else {
            sc_regSet(FLAG_OVERFLOW, 0);
        }
        sc_memorySet(address, value);
    }
    
    printCell(address, MY_COLOR_YELLOW, MY_COLOR_WHITE, win);
    wrefresh(win);
    printCell(address, MY_COLOR_YELLOW, MY_COLOR_BLACK, win);
}

void edit_reg(int reg_num, WINDOW *win) {
    char input[10] = {0};
    int pos = 0;
    int value;
    sc_regGet(reg_num, &value);
    
    echo();
    curs_set(1); 
    
    mvwprintw(win, 1, 1, "     ");
    wmove(win, 1, 1);
    wrefresh(win);
    
    int ch;
    while ((ch = getch()) != '\n') {
        if (ch == KEY_BACKSPACE || ch == 127) {
            if (pos > 0) {
                pos--;
                input[pos] = '\0';
                mvwaddch(win, 1, 1 + pos, ' ');
                wmove(win, 1, 1 + pos);
                wrefresh(win);
            }
        }
        else if (isdigit(ch) && pos < 7) {
            input[pos++] = ch;
            waddch(win, ch);
            wrefresh(win);
        }
    }
    
    noecho();
    curs_set(0); 
    
    if (pos > 0) {
        value = atoi(input);
        sc_memorySet(reg_num, value);
    }
    
    printCell(reg_num, MY_COLOR_YELLOW, MY_COLOR_WHITE, win);
    wrefresh(win);
}

void edit_accum(WINDOW *win) {
    char input[10] = {0};
    int pos = 0;
    sc_accumulatorGet(&acum);
    
    rk_mytermregime(1, 0, 1, 0, 1);
    curs_set(1); 
    
    mvwprintw(win, 2, 9, "     ");
    wmove(win, 2, 9);
    wrefresh(win);
    
    enum keys key;
    while (1) {
        if (rk_readkey(&key)) continue;

        if (key == N_KEY_ENTER) {
            break;
        }
        
        if (key == N_KEY_ESC) {
            pos = 0;
            break;  
        }

        if (key == N_KEY_BACKSPACE) {
            if (pos > 0) {
                pos--;
                input[pos] = '\0';
                mvwaddch(win, 2, 9 + pos, ' ');
                wmove(win, 2, 9 + pos);
                wrefresh(win);
            }
            continue;
        }

        if ((key >= N_KEY_0 && key <= N_KEY_9) || 
            (key >= N_KEY_A && key <= N_KEY_F) ||
            (pos == 0 && (key == N_KEY_PLUS || key == N_KEY_MINUS)) ||
            (pos == 1 && key == N_KEY_X)) {
            
            if (pos < 9) { 
                char c;
                if (key >= N_KEY_0 && key <= N_KEY_9) {
                    c = '0' + (key - N_KEY_0);
                } else if (key >= N_KEY_A && key <= N_KEY_F) {
                    c = 'a' + (key - N_KEY_A);
                } else if (key == N_KEY_X) {
                    c = 'x';
                } else {
                    c = (key == N_KEY_PLUS) ? '+' : '-';
                }
                
                input[pos++] = c;
                waddch(win, c);
                wrefresh(win);
            }
        }
    }
    
    curs_set(0);
    rk_mytermregime(1, 0, 1, 0, 1);
    
    if (pos > 0) {
        if (pos > 2 && input[0] == '0' && input[1] == 'x') {
            acum = (int)strtol(input, NULL, 16);
        } else {
            acum = atoi(input);
        }
        sc_accumulatorSet(acum);
    }
    if (acum > 0x7FFF || acum < -0x7FFF) { 
        acum = 16383;
        sc_regSet(FLAG_OVERFLOW, 1); 
    } else {
        sc_regSet(FLAG_OVERFLOW, 0);
    }
    printAccumulator(win);
    wrefresh(win);
}

void edit_icounter(WINDOW *win) {
    char input[10] = {0};
    int pos = 0; 
    sc_icounterGet(&schet);
    
    echo();
    curs_set(1); 
    
    mvwprintw(win, 2, 19, "     ");
    wmove(win, 2, 19);
    wrefresh(win);
    
    int ch;
    while ((ch = getch()) != '\n') {
        if (ch == KEY_BACKSPACE || ch == 127) {
            if (pos > 0) {
                pos--;
                input[pos] = '\0';
                mvwaddch(win, 2, 19 + pos, ' ');
                wmove(win, 2, 19 + pos);
                wrefresh(win);
            }
        }
        else if (isdigit(ch) && pos < 5) {
            input[pos++] = ch;
            waddch(win, ch);
            wrefresh(win);
        }
    }
    
    noecho();
    curs_set(0); 
    
    if (pos > 0) {
        schet = atoi(input);
        sc_icounterSet(schet);
    }
    if (schet > 128 || schet < 0){schet = 0 ; sc_regSet (FLAG_OVERFLOW, 1); } else {sc_regSet (FLAG_OVERFLOW, 0);}
    printCounters(win);
    wrefresh(win);
}

int ALU(int command, int operand) {
    int value =0;
    int right =0;
    
    switch (command) {  

        case 0x01: // CPUINFO
           cache_print = 1;
           cache_check = 1;
           break;

        case 0x0A: // READ
            if (operand < 0 || operand >= MEMORY_SIZE) {
                sc_regSet(FLAG_OUTOFMEM, 1);
                return -1;
            } else {sc_regSet(FLAG_OUTOFMEM, 0);}
            right = data[operand] & OPERAND_MASK;
            sc_memoryGet(right, &value);
            if (value > 0x7FFF || value < -0x7FFF) {
                value = 16383;
                sc_regSet(FLAG_OVERFLOW, 1);
                return -1;
            } else {sc_regSet(FLAG_OVERFLOW, 0);}
            cache_check = 1;
            cache_check2 = 1;
            break;
            
        case 0x0B: // WRITE
            cache_print_addr = 1;
            cache_check = 1;
            break;

        case 0x14: // LOAD
            if (operand < 0 || operand >= MEMORY_SIZE) {
                sc_regSet(FLAG_OUTOFMEM, 1);
                return -1;
            } else {sc_regSet(FLAG_OUTOFMEM, 0);}
            right = data[operand] & OPERAND_MASK;
            sc_memoryGet(right, &value);
            acum = value;
            if (value > 0x7FFF || value < -0x7FFF) {
                acum = 16383;
                sc_regSet(FLAG_OVERFLOW, 1);
                return -1;
            } else {sc_regSet(FLAG_OVERFLOW, 0);}
            cache_check = 1;
            break;

        case 0x15: // STORE  
            if (operand < 0 || operand >= MEMORY_SIZE) {
                sc_regSet(FLAG_OUTOFMEM, 1);
                return -1;
            } else {sc_regSet(FLAG_OUTOFMEM, 0);}
            right = data[operand] & OPERAND_MASK;
            value = acum;
            sc_memorySet(right, value);
            if (acum > 0x7FFF || acum < -0x7FFF) {
                sc_regSet(FLAG_OVERFLOW, 1);
                return -1;
            } else {sc_regSet(FLAG_OVERFLOW, 0);}
            cache_check = 1;
            break;

        case 0x1E: // ADD 
            if (operand < 0 || operand >= MEMORY_SIZE) {
                sc_regSet(FLAG_OUTOFMEM, 1);
                return -1;
            } else {sc_regSet(FLAG_OUTOFMEM, 0);}
            right = data[operand] & OPERAND_MASK;
            sc_memoryGet(right, &value);
            acum += value;
            if (acum > 0x7FFF || acum < -0x7FFF) {
                sc_regSet(FLAG_OVERFLOW, 1);
                return -1;
            } else {sc_regSet(FLAG_OVERFLOW, 0);}
            cache_check = 1;
            break;
            
        case 0x1F: // SUB 
            if (operand < 0 || operand >= MEMORY_SIZE) {
                sc_regSet(FLAG_OUTOFMEM, 1);
                return -1;
            } else {sc_regSet(FLAG_OUTOFMEM, 0);}
            right = data[operand] & OPERAND_MASK;
            sc_memoryGet(right, &value);
            acum -= value;
            if (acum > 0x7FFF || acum < -0x7FFF) {
                sc_regSet(FLAG_OVERFLOW, 1);
                return -1;
            } else {sc_regSet(FLAG_OVERFLOW, 0);}
            cache_check = 1;
            break;
            
        case 0x20: // DIVIDE 
            if (operand < 0 || operand >= MEMORY_SIZE) {
                sc_regSet(FLAG_OUTOFMEM, 1);
                return -1;
            } else {sc_regSet(FLAG_OUTOFMEM, 0);}
            right = data[operand] & OPERAND_MASK;
            sc_memoryGet(right, &value);
            if (value == 0) {
                sc_regSet(FLAG_DIVISION, 1);
                return -1;
            } else {sc_regSet(FLAG_DIVISION, 0);}
            acum = acum / value;
            if (acum > 0x7FFF || acum < -0x7FFF) {
                sc_regSet(FLAG_OVERFLOW, 1);
                return -1;
            } else {sc_regSet(FLAG_OVERFLOW, 0);}
            cache_check = 1;
            break;
            
        case 0x21: // MUL
            if (operand < 0 || operand >= MEMORY_SIZE) {
                sc_regSet(FLAG_OUTOFMEM, 1);
                return -1;
            } else {sc_regSet(FLAG_OUTOFMEM, 0);}
            right = data[operand] & OPERAND_MASK;
            sc_memoryGet(right, &value);
            acum *= value;
            if (acum > 0x7FFF || acum < -0x7FFF) {
                sc_regSet(FLAG_OVERFLOW, 1);
                return -1;
            } else {sc_regSet(FLAG_OVERFLOW, 0);}
            cache_check = 1;
            break;

        case 0x28: // JUMP 
            if (operand < 0 || operand >= MEMORY_SIZE) {
                sc_regSet(FLAG_OUTOFMEM, 1);
                return -1;
            } else {sc_regSet(FLAG_OUTOFMEM, 0);}
            right = data[operand] & OPERAND_MASK;
            current_address = right;
            cache_check = 1;
            cache_jump = 1;
            break;

        case 0x29: // JNEG
            if (acum < 0) {
                if (operand < 0 || operand >= MEMORY_SIZE) {
                    sc_regSet(FLAG_OUTOFMEM, 1);
                    return -1;
                }
                right = data[operand] & OPERAND_MASK;
                current_address = right;
                cache_check = 1;
                cache_jump = 1;  
            }
            break;

        case 0x2A: // JZ
            if (acum == 0) {
                if (operand < 0 || operand >= MEMORY_SIZE) {
                    sc_regSet(FLAG_OUTOFMEM, 1);
                    return -1;
                }
                right = data[operand] & OPERAND_MASK;
                current_address = right;
                cache_check = 1;
                cache_jump = 1;
            }
            break;

        case 0x2B: // HALT
            stop = 1;
            break;

        case 0x44: // INC
            if (++acum > 0x7FFF) {
                sc_regSet(FLAG_OVERFLOW, 1);
                return -1;
            }
            break;
            
        case 0x45: // DEC
            if (--acum < -0x7FFF) {
                sc_regSet(FLAG_OVERFLOW, 1);
                return -1;
            }
            break;
         
        default:
            sc_regSet(FLAG_INVALID, 1);
            return -1;
    }

    sc_regSet(FLAG_OVERFLOW, 0);
    sc_regSet(FLAG_DIVISION, 0);
    
    return 0;
}

void init(){
	sc_memoryInit();

	sc_regInit();
	sc_accumulatorInit();
	sc_icounterInit();
	refresh();
}


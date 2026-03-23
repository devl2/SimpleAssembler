#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 128
#define MAX_CONST 32

int var_addr[26] = {0};
int var_next_addr = 0; 
int const_values[MAX_CONST] = {0};
int const_addrs[MAX_CONST] = {0};
int const_count = 0;
int line_to_addr[100] = {0};
int total_commands = 0;

int get_var_addr(char var) {
    if (var >= '0' && var <= '9') {
        return atoi(&var);
    }
    if (var < 'A' || var > 'Z') return -1;
    if (var_addr[var - 'A'] == 0) {
        if (var_next_addr >= 100) {
            fprintf(stderr, "Error: No more variable addresses available\n");
            exit(1);
        }
        var_addr[var - 'A'] = var_next_addr++;
    }
    return var_addr[var - 'A'];
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s file.sb file.sa\n", argv[0]);
        return 1;
    }
    FILE *in = fopen(argv[1], "r");
    FILE *out = fopen(argv[2], "w");
    if (!in || !out) {
        perror("File");
        return 1;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), in)) {
        if (line[0] == '\n' || line[0] == 0)
            continue;
            
        int lineno;
        char cmd[16], arg1[16], arg2[16], arg3[16];
        memset(cmd, 0, sizeof(cmd));
        
        if (sscanf(line, "%d %15s %15s %15s %15s", &lineno, cmd, arg1, arg2, arg3) < 2)
            continue;

        if (strcmp(cmd, "REM") == 0) total_commands++;
        else if (strcmp(cmd, "INPUT") == 0) total_commands++;
        else if (strcmp(cmd, "PRINT") == 0 || strcmp(cmd, "OUTPUT") == 0) total_commands++;
        else if (strcmp(cmd, "LET") == 0) {
            char expr[32];
            strncpy(expr, arg3, sizeof(expr)-1);
            expr[sizeof(expr)-1] = '\0';

            char clean_expr[32];
            int clean_pos = 0;
            for (int i = 0; expr[i]; i++) {
                if (!isspace(expr[i])) {
                    clean_expr[clean_pos++] = expr[i];
                }
            }
            clean_expr[clean_pos] = '\0';
        
            int num;
            if (sscanf(clean_expr, "%d", &num) == 1) {
                total_commands += 2;
            }
            else {
                char *op_ptr = strpbrk(clean_expr, "+-*/");
                if (op_ptr != NULL) {
                    total_commands += 2;
                }
                else if (strlen(clean_expr) == 1) {
                    total_commands += 2;
                }
            }
        }
        else if (strcmp(cmd, "GOTO") == 0) total_commands++;
        else if (strcmp(cmd, "IF") == 0) {
            total_commands += 3;
        }
        else if (strcmp(cmd, "END") == 0) total_commands++;
    }
    rewind(in);

    var_next_addr = total_commands;

    int curr_addr = 0;
    while (fgets(line, sizeof(line), in)) {
        if (line[0] == '\n' || line[0] == 0)
            continue;
            
        int lineno;
        char cmd[16], arg1[16], arg2[16], arg3[16];
        memset(cmd, 0, sizeof(cmd));
        
        if (sscanf(line, "%d %15s %15s %15s %15s", &lineno, cmd, arg1, arg2, arg3) < 2)
            continue;

        line_to_addr[lineno] = curr_addr;

        if (strcmp(cmd, "REM") == 0) curr_addr++;
        else if (strcmp(cmd, "INPUT") == 0) curr_addr++;
        else if (strcmp(cmd, "PRINT") == 0 || strcmp(cmd, "OUTPUT") == 0) curr_addr++;
        else if (strcmp(cmd, "LET") == 0) {
            char expr[32];
            strncpy(expr, arg3, sizeof(expr)-1);
            expr[sizeof(expr)-1] = '\0';

            char clean_expr[32];
            int clean_pos = 0;
            for (int i = 0; expr[i]; i++) {
                if (!isspace(expr[i])) {
                    clean_expr[clean_pos++] = expr[i];
                }
            }
            clean_expr[clean_pos] = '\0';
        
            int num;
            if (sscanf(clean_expr, "%d", &num) == 1) {
                curr_addr += 2;
            }
            else {
                char *op_ptr = strpbrk(clean_expr, "+-*/");
                if (op_ptr != NULL) {
                    curr_addr += 4;
                }
                else if (strlen(clean_expr) == 1) {
                    curr_addr += 2;
                }
            }
        }
        else if (strcmp(cmd, "GOTO") == 0) curr_addr++;
        else if (strcmp(cmd, "IF") == 0) {
            curr_addr += 3;
        }
        else if (strcmp(cmd, "END") == 0) curr_addr++;
    }
    rewind(in);

    curr_addr = 0;
    
    while (fgets(line, sizeof(line), in)) {
        if (line[0] == '\n' || line[0] == 0)
            continue;
            
        int lineno;
        char cmd[16], arg1[16], arg2[16], arg3[16];
        memset(cmd, 0, sizeof(cmd));
        
        if (sscanf(line, "%d %15s %15s %15s %15s", &lineno, cmd, arg1, arg2, arg3) < 2)
            continue;

        if (strcmp(cmd, "REM") == 0) {
            fprintf(out, "%02d CPUINFO 00 ; (Комментарий)\n", curr_addr++);
        } 
        else if (strcmp(cmd, "INPUT") == 0) {
            int vaddr = get_var_addr(arg1[0]);
            fprintf(out, "%02d READ %02d ; (Ввод %c)\n", curr_addr++, vaddr, arg1[0]);
        } 
        else if (strcmp(cmd, "PRINT") == 0 || strcmp(cmd, "OUTPUT") == 0) {
            int vaddr = get_var_addr(arg1[0]);
            fprintf(out, "%02d WRITE %02d ; (Вывод %c)\n", curr_addr++, vaddr, arg1[0]);
        } 
        else if (strcmp(cmd, "LET") == 0) {
            char lhs = arg1[0];
            int addr_lhs = get_var_addr(lhs);
            
            char *equals = strstr(line, "=");
            if (equals) {
                equals++;
                while (*equals && isspace(*equals)) equals++;
                
                char expr[32] = {0};
                strncpy(expr, equals, sizeof(expr)-1);

                char *end = expr + strlen(expr) - 1;
                while (end > expr && isspace(*end)) {
                    *end = '\0';
                    end--;
                }
            
                char clean_expr[32];
                int clean_pos = 0;
                for (int i = 0; expr[i]; i++) {
                    if (!isspace(expr[i])) {
                        clean_expr[clean_pos++] = expr[i];
                    }
                }
                clean_expr[clean_pos] = '\0';
            
                int num;
                if (sscanf(clean_expr, "%d", &num) == 1) {
                    fprintf(out, "%02d LOAD #%d ; (Загрузка константы %d)\n", curr_addr++, num, num);
                    fprintf(out, "%02d STORE %02d ; (Сохранение в %c)\n", curr_addr++, addr_lhs, lhs);
                }
                else {
                    char *op_ptr = strpbrk(clean_expr, "+-*/");
                    if (op_ptr != NULL) {
                        char op = *op_ptr;
                        *op_ptr = '\0';
                        char *left = clean_expr;
                        char *right = op_ptr + 1;
            
                        int addr_left = get_var_addr(left[0]);
                        int addr_right = get_var_addr(right[0]);
            
                        fprintf(out, "%02d LOAD %02d ; (Загрузка %c)\n", curr_addr++, addr_left, left[0]);
                        
                        switch(op) {
                            case '+': fprintf(out, "%02d ADD %02d ; (Сложение с %c)\n", curr_addr++, addr_right, right[0]); break;
                            case '-': fprintf(out, "%02d SUB %02d ; (Вычитание %c)\n", curr_addr++, addr_right, right[0]); break;
                            case '*': fprintf(out, "%02d MUL %02d ; (Умножение на %c)\n", curr_addr++, addr_right, right[0]); break;
                            case '/': fprintf(out, "%02d DIVIDE %02d ; (Деление на %c)\n", curr_addr++, addr_right, right[0]); break;
                        }
                        
                        fprintf(out, "%02d STORE %02d ; (Сохранение в %c)\n", curr_addr++, addr_lhs, lhs);
                    }
                    else if (strlen(clean_expr) == 1) {
                        fprintf(out, "%02d LOAD %02d ; (Загрузка %c)\n", curr_addr++, get_var_addr(clean_expr[0]), clean_expr[0]);
                        fprintf(out, "%02d STORE %02d ; (Сохранение в %c)\n", curr_addr++, addr_lhs, lhs);
                    }
                }
            }
        }
        else if (strcmp(cmd, "GOTO") == 0) {
            int target_line = atoi(arg1);
            int target_addr = line_to_addr[target_line];
            fprintf(out, "%02d JUMP %02d ; (Переход на строку %d)\n", curr_addr++, target_addr, target_line);
        }
        else if (strcmp(cmd, "IF") == 0) {
            char var = arg1[0];
            char cmp[3];
            strncpy(cmp, arg2, sizeof(cmp));
            cmp[sizeof(cmp)-1] = '\0';

            int compare_value = 0;
            if (isdigit(arg2[0])) {
                compare_value = atoi(arg2);
            } else if (isdigit(arg3[0])) {
                compare_value = atoi(arg3);
            } else {
                char *p = arg2;
                while (*p && !isdigit(*p)) p++;
                if (*p) compare_value = atoi(p);
                else {
                    p = arg3;
                    while (*p && !isdigit(*p)) p++;
                    if (*p) compare_value = atoi(p);
                }
            }

            char *target_str = strstr(line, "GOTO");
            if (target_str) {
                target_str += 4;
                while (*target_str && !isdigit(*target_str)) target_str++;
            }
            int target_line = atoi(target_str);
            int target_addr = line_to_addr[target_line];

            int vaddr = get_var_addr(var);

            fprintf(out, "%02d LOAD %02d ; (Загрузка %c)\n", curr_addr++, vaddr, var);

            if (strcmp(cmp, "<") == 0) {
                fprintf(out, "%02d JNEG %02d ; (Переход если меньше на строку %d)\n", curr_addr++, target_addr, target_line);
            }
            else if (strcmp(cmp, ">") == 0) {
                int after_if = curr_addr + 2;
                fprintf(out, "%02d JUMP %02d ; (Переход на строку %d)\n", curr_addr++, target_addr, target_line);
            }
            else if (strcmp(cmp, "==") == 0) {
                fprintf(out, "%02d JZ %02d ; (Переход если равно на строку %d)\n", curr_addr++, target_addr, target_line);
            }
        }
        else if (strcmp(cmd, "END") == 0) {
            fprintf(out, "%02d HALT 00 ; (Останов)\n", curr_addr++);
        }
    }

    for (int i = 0; i < 26; ++i) {
        if (var_addr[i])
            fprintf(out, "%02d = +0000 ; (Переменная %c)\n", var_addr[i], 'A' + i);
    }

    fclose(in);
    fclose(out);
    printf("Translation completed: %s\n", argv[2]);
    return 0;
}
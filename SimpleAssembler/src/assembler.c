#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mySimpleComputer.h"
#include "assembler.h"

#define MAX_LINE 128

typedef struct {
    char *name;
    int code;
} Command;

Command commands[] = {
    {"NOP", 0x00},
    {"CPUINFO", 0x01},
    {"READ", 0x0A},
    {"WRITE", 0x0B},
    {"LOAD", 0x14},
    {"STORE", 0x15},
    {"ADD", 0x1E},
    {"SUB", 0x1F},
    {"DIVIDE", 0x20},
    {"MUL", 0x21},
    {"JUMP", 0x28},
    {"JNEG", 0x29},
    {"JZ", 0x2A},
    {"HALT", 0x2B},
    {NULL, 0}
};

int find_command_code(const char *name) {
    for (int i = 0; commands[i].name != NULL; ++i) {
        if (strcmp(commands[i].name, name) == 0)
            return commands[i].code;
    }
    return -1;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s file.sa file.o\n", argv[0]);
        return 1;
    }
    FILE *in = fopen(argv[1], "r");
    if (!in) {
        perror("Input file");
        return 1;
    }
    sc_memoryInit();

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), in)) {
        int addr, operand;
        char cmd[16] = {0};
        if (line[0] == ';' || line[0] == '\n' || line[0] == 0)
            continue;
        if (strstr(line, "=")) {
            char value_str[32] = {0};
            if (sscanf(line, "%d = %s", &addr, value_str) == 2) {
                if (value_str[0] == '+' || value_str[0] == '-') {
                    if (value_str[1] == '0' && (value_str[2] == 'x' || value_str[2] == 'X')) {
                        operand = (int)strtol(value_str + 1, NULL, 16);
                    } else {
                        operand = atoi(value_str);
                    }
                } else {
                    if (value_str[0] == '0' && (value_str[1] == 'x' || value_str[1] == 'X')) {
                        operand = (int)strtol(value_str, NULL, 16);
                    } else {
                        operand = atoi(value_str);
                    }
                }
                sc_memorySet(addr, operand);
            }
        } else {
            if (sscanf(line, "%d %15s %d", &addr, cmd, &operand) == 3) {
                int code = find_command_code(cmd);
                if (code < 0) {
                    fprintf(stderr, "Unknown command: %s\n", cmd);
                    continue;
                }
                int value = 0;
                sc_commandEncode(0, code, operand, &value);
                sc_memorySet(addr, value);
            }
        }
    }
    fclose(in);
    sc_memorySave(argv[2]);
    printf("Compiled successfully to %s\n", argv[2]);
    return 0;
}
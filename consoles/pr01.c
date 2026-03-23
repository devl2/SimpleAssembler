#include "mySimpleComputer.h"
#include <stdio.h>

/*int main() {
    int value, sign, command, operand;
    int *idvalue = &value;
    
    sc_memoryInit();
    sc_regInit();
    sc_accumulatorInit();
    sc_icounterInit();
    
    sc_memorySet(1, 12);
    sc_memorySet(2, 34);
   
    sc_memoryGet(1, idvalue);
    //printf("Memory[1] = %d\n", *idvalue);
    printCell(1,COLOR_RED,COLOR_BLUE);
   
    sc_regSet(FLAG_OVERFLOW, 1);
    sc_regSet(FLAG_DIVISION, 0);

    sc_regGet(FLAG_OVERFLOW, &value);
    printf("FLAG_OVERFLOW = %d\n", value);
    
    sc_accumulatorSet(100);
    sc_accumulatorGet(&value);
    printf("Accumulator = %d\n", value);
    
    sc_icounterSet(10);
    sc_icounterGet(&value);
    printf("Instruction Counter = %d\n", value);

    sc_commandEncode(0, 0x21, 0x7F, &value);
    printf("decimal: %d\n", value);
    printf("hexadecimal: 0x%X\n", value);
    printf("binary: ");
    for (int i = 15; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
    }
    printf("\n");
    
    sc_commandDecode(value, &sign, &command, &operand);
    printf("Decoded command: sign=%d, command=%02X, operand=%02X\n", sign, command, operand);

    return 0;
}*/
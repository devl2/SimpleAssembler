#include "./mySimpleComputer.h"
int sc_commandValidate(int command) {
    if ((command & ~COMMAND_MASK) != 0) {
        return -1;
    }
    return 0;
}

int sc_commandEncode(int sign, int command, int operand, int* value)
{
    if (value == NULL || 
        (sign != 0 && sign != 1) || 
        command < 0 || command > COMMAND_MASK || 
        operand < 0 || operand > OPERAND_MASK) {
        return -1;
    }

    *value = (sign ? SIGN_MASK : 0) | (command << COMMAND_SHIFT) | operand;
    return 0;
}

int sc_commandDecode(int value, int *sign, int *command, int *operand) {
    if (value < -32768 || value > 0x7FFF)
    {
      return -1;
    }
    
    
    *sign = (value & SIGN_MASK) ? 1 : 0;
    *command = (value >> COMMAND_SHIFT) & COMMAND_MASK;
    *operand = value & OPERAND_MASK;

    return 0;
}
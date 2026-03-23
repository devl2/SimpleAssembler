#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "mySimpleComputer.h"

int translate_assembler(const char* input_file, const char* output_file);

int get_opcode(const char* command);

#endif
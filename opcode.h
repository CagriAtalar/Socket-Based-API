// opcode.h
#ifndef OPCODE_H
#define OPCODE_H

typedef enum {
    OPCODE_PUSH,
    OPCODE_POP,
    OPCODE_UNKNOWN
} OPCODE;

typedef enum {
    TRAP_OK,
    TRAP_FILE_ERROR,
    TRAP_DECODE_ERROR,
    TRAP_UNKNOWN_OPCODE
} TRAP_TABLE;

#endif

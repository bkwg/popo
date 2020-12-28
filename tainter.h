#ifndef POPO_TAINTER_H
#define POPO_TAINTER_H

#include "popo_types.h"

typedef struct virtual_cpu {
    U64 rax;
    U64 rcx;
    U64 rdx;
    U64 rbx;
    U64 rsp;
    U64 rbp;
    U64 rsi;
    U64 rdi;
    U64 r8;
    U64 r9;
    U64 r10;
    U64 r11;
    U64 r12;
    U64 r13;
    U64 r14;
    U64 r15;
} PopoVCPU;

PopoStatus popo_disas_and_taint(struct user_regs_struct* regs, char* buffer);

#endif /* POPO_TAINTER_H */

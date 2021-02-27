#ifndef DISAS_H
#define DISAS_H

#include "bk_types.h"

// TODO: depends on architecture
int popo_disassemble(U64 pc, char buffer[], void* insn);

#endif /* ! DISAS_H */

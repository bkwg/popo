#ifndef POPO_TRACEE_H
#define POPO_TRACEE_H

#include "bk_types.h"
#include "dbg_types.h"

typedef struct PopoTracee {
    I32 pid;
    const char* image_path;
    BkStatus (*run)(const char*);
} PopoTracee;

void popo_init_tracee(PopoTracee* self);

#endif /* POPO_TRACEE_H */

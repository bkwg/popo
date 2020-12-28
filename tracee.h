#ifndef POPO_TRACEE_H
#define POPO_TRACEE_H

#include "popo_types.h"

typedef struct PopoTracee {
    I32 pid;
    const char* image_path;
    PopoStatus (*run)(const char*);
} PopoTracee;

void popo_init_tracee(PopoTracee* self);

#endif /* POPO_TRACEE_H */

#ifndef DEBUG_H
#define DEBUG_H

#include "bk_types.h"
#include "tracee.h"

typedef struct tracer {
    PopoTracee tracee;
} PopoTracer;

BkStatus popo_launch(PopoArgs args);

#endif /* ! DEBUG_H */

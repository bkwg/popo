#ifndef DEBUG_H
#define DEBUG_H

#include "popo_types.h"
#include "tracee.h"

typedef struct tracer {
    PopoTracee tracee;
} PopoTracer;

PopoStatus popo_launch(PopoArgs args);

#endif /* ! DEBUG_H */

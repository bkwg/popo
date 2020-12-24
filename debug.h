#ifndef DEBUG_H
#define DEBUG_H

#define FAILED -1
#define SUCCEED 0

#include <sys/types.h>

int trace_child(pid_t child);
void run_target(const char* target_name);

#endif /* ! DEBUG_H */

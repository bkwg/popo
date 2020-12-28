#include <sys/ptrace.h>
#include <unistd.h>
#include <stdio.h>
#include "tracee.h"

/*

    Private

*/

static PopoStatus run(const char* target_name)
{
    // all other flags are ignored
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) == -1)
    {
        perror("ptrace TRACEME");
        return POPO_STATUS_FAILURE;
    }

    // replace the current process image with a new process image (relies on
    // execve).
    if (execl(target_name, target_name, (char*) NULL) == -1)
    {
        perror("execl");
        return POPO_STATUS_FAILURE;
    }

    return POPO_STATUS_SUCCESS;
}

/*

    Public

*/

void popo_init_tracee(PopoTracee* self)
{
    self->pid = 0;
    self->image_path = NULL;
    self->run = &run;
}

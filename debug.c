#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>

#include "debug.h"
#include "disas.h"


static unsigned int g_insn_counter = 0;

void run_target(const char* target_name)
{
    // all other flags are ignored
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0)
    {
        perror("ptrace");
        return;
    }

    // replace the current process image with a new process image (relies on
    // execve).
    if (execl(target_name, target_name, (char*) NULL) == -1)
    {
        perror("execl");
        return;
    }
}

void increment_insn_count()
{
    ++g_insn_counter;
}

int handle_child_signal(pid_t child, siginfo_t *infop)
{
    struct user_regs_struct regs = { 0 };

    switch (infop->si_code)
    {
        //TODO: differenciate between x86 and x86_64
        //      maybe by reading elf header...
        case CLD_TRAPPED:
            // TODO: check that appropriately (cf man RETURN VALUE)
            //printf("child %d trapped\n", infop->si_pid);
            if (ptrace(PTRACE_GETREGS, child, 0, &regs) == -1)
            {
                perror("ptrace GETREGS");
                return FAILED;
            }

            long insn = 0;

            if ((insn = ptrace(PTRACE_PEEKTEXT, child, regs.rip, 0)) == -1)
            {
                perror("ptrace PEEKTEXT");
                return FAILED;
            }

            /* ACTIONS : here we perform actions the instructions */
            disassemble(regs.rip, (char*)&insn);
            increment_insn_count();

            if (ptrace(PTRACE_SINGLESTEP, child, 0, 0) == -1)
            {
                perror("ptrace");
                return FAILED;
            }
            break;
        case CLD_CONTINUED:
            printf("child %d continued by signal %d (SIGCOUNT)\n", infop->si_pid, infop->si_status);
            break;
        case CLD_STOPPED:
            printf("child %d stopped by signal %d\n", infop->si_pid, infop->si_status);
            break;
        case CLD_EXITED:
            printf("child %d exited with status %d\n", infop->si_pid, infop->si_status);
            break;
        case CLD_KILLED:
            printf("child %d killed by signal %d\n", infop->si_pid, infop->si_status);
            break;
        case CLD_DUMPED:
            printf("child %d dumped by signal %d\n", infop->si_pid, infop->si_status);
            break;
        default:
            printf("something else happened to the child %d\n", infop->si_pid);
    }

    return SUCCEED;
}

int trace_child(pid_t child)
{
    siginfo_t infop = { 0 };

    do
    {
        if (waitid(P_PID, child, &infop, WSTOPPED | WCONTINUED | WEXITED) != 0)
        {
            perror("waitid");
            return FAILED;
        }
        
        handle_child_signal(child, &infop);
    }
    while ((infop.si_code != CLD_KILLED)
        && (infop.si_code != CLD_EXITED)
        && (infop.si_code != CLD_DUMPED));

    printf(">>> insn_count %d\n", g_insn_counter);
    return SUCCEED;
}

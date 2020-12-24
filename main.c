#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <unistd.h>

#include "disas.h"

#define FAILED -1
#define CLD_TERMINATED CLD_EXITED | CLD_KILLED | CLD_DUMPED

void print_usage(const char* program_name)
{
    printf("usage: %s image\n", program_name);
}

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

int main(int argc, char *argv[]) {

    if (argc != 2)
    {
        print_usage(argv[0]);
        return FAILED;
    }

    
    pid_t child;
    
    child = fork();

    if (child == -1)
    {
        perror("fork");
        return FAILED;
    }
    else if (child == 0)
    {
        // in the child process
        puts("in the child");
        run_target(argv[1]);
        return 111;
    }
    else if (child > 0)
    {
        printf("my child is %d\n", child);
        siginfo_t infop;

        
        unsigned int insn_counter = 0;
        struct user_regs_struct regs = { 0 };
        do
        {
            if (waitid(P_PID, child, &infop, WSTOPPED | WCONTINUED | WEXITED) != 0)
            {
                perror("waitid");
                return -1;
            }
            switch (infop.si_code)
            {
                //TODO: differenciate between x86 and x86_64
                //      maybe by reading elf header...
                case CLD_TRAPPED:
                    // TODO: check that appropriately (cf man RETURN VALUE)
                    if (ptrace(PTRACE_GETREGS, child, 0, &regs) == -1) {
                        perror("ptrace GETREGS");
                        return FAILED;
                    }

                    long insn = 0;

                    if ((insn = ptrace(PTRACE_PEEKTEXT, child, regs.rip, 0)) == -1)
                    {
                        perror("ptrace PEEKTEXT");
                    }

                    printf("insn: %lx %llx\n", insn, regs.rip);
                    disassemble(regs.rip, (char*)&insn);

                    //printf("child %d trapped\n", infop.si_pid);
                    ++insn_counter;
                    if (ptrace(PTRACE_SINGLESTEP, child, 0, 0) == -1)
                    {
                        perror("ptrace");
                    }
                    break;
                case CLD_CONTINUED:
                    printf("child %d continued by signal %d (SIGCOUNT)\n", infop.si_pid, infop.si_status);
                    break;
                case CLD_STOPPED:
                    printf("child %d stopped by signal %d\n", infop.si_pid, infop.si_status);
                    break;
                case CLD_EXITED:
                    printf("child %d exited with status %d\n", infop.si_pid, infop.si_status);
                    break;
                case CLD_KILLED:
                    printf("child %d killed by signal %d\n", infop.si_pid, infop.si_status);
                    break;
                case CLD_DUMPED:
                    printf("child %d dumped by signal %d\n", infop.si_pid, infop.si_status);
                    break;
                default:
                    printf("something else happened to the child %d\n", infop.si_pid);
            }
        } while ((infop.si_code != CLD_KILLED)
                && (infop.si_code != CLD_EXITED)
                && (infop.si_code != CLD_DUMPED));

        printf(">>> insn_count %d\n", insn_counter);
    }
    
    return 0;
}

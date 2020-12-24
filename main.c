#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define FAILED -1

int main(int argc, char *argv[]) {

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
        return 111;
    }
    else if (child > 0)
    {
        printf("my child is %d\n", child);
        siginfo_t infop;
        if (waitid(P_PID, child, &infop, WEXITED) != 0)
        {
            perror("waitid");
            return -1;
        }
        
        switch (infop.si_code)
        {
            case CLD_EXITED:
                printf("child %d exited with status %d\n", infop.si_pid, infop.si_status);
                break;
            case CLD_TRAPPED:
                printf("child %d trapped\n", infop.si_pid);
                break;
            case CLD_KILLED:
                printf("child %d killed by signal %d\n", infop.si_pid, infop.si_status);
                break;
            default:
                printf("something else happened to the child %d\n", infop.si_pid);
        }
    }
    
    return 0;
}

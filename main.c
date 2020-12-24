#include <stdio.h>
#include <unistd.h>
#include "disas.h"
#include "debug.h"

void print_usage(const char* program_name)
{
    printf("usage: %s image\n", program_name);
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
        puts("in the child");
        run_target(argv[1]);
        return 111;
    }
    else if (child > 0)
    {
        printf("my child is %d\n", child);
        trace_child(child);
    }
    
    return 0;
}

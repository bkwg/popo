#include <stdio.h>
#include <unistd.h>

#include "disas.h"
#include "debug.h"
#include "argparse.h"

static const char *const usages[] = {
    "program [options] [[--] args]",
    NULL,
};


// See void
void print_popo(char *file)
{
    FILE* f = fopen(file, "r");
    if (!f)
        return;

    char line[100] = { 0 };
    while (fgets(line, 100, f))
    {
        printf("%s", line);
    }

    fclose(f);
}

void print_banner(void)
{
    print_popo("popo_ascii_image.txt");
    print_popo("popo_ascii_text.txt");
    puts("\n");
}

int launch_popo_spawn(char* image_path)
{
    printf("[+] Spawning a process from '%s'\n", image_path);
    pid_t child;
    
    child = fork();

    if (child == -1)
    {
        perror("fork");
        return FAILED;
    }
    else if (child == 0)
    {
        puts("[+] Process started successfully\n");
        run_target(image_path);
        return 111;
    }
    else if (child > 0)
    {
        printf("[+] Starting tracing process '%d'\n", child);
        trace_child(child);
    }

    return SUCCEED;
}

void launch_popo_attach(pid_t pid)
{
    printf("[+] Attaching to process '%d'\n", pid);
    attach_target(pid);
    printf("[+] Starting tracing process '%d'\n", pid);
    trace_child(pid);
}

int main(int argc, char *argv[])
{
    pid_t pid = 0;
    char* image_path = NULL;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("Main options"),
        OPT_STRING('s', "path", &image_path, "spawn the process given its image path", NULL, 0, 0),
        OPT_INTEGER('a', "pid", &pid, "attach to the process given its pid", NULL, 0, 0),
        OPT_END(),
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\npopo", "\npopo");
    argc = argparse_parse(&argparse, argc, (const char**)argv);
    print_banner();
    if (image_path != NULL)
        launch_popo_spawn(image_path);
    else if (pid != 0)
        launch_popo_attach(pid);   

    return 0;
}

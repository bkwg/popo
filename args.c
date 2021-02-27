#include "argparse.h"
#include "bk_types.h"
#include "args.h"

static struct argparse argparse = { 0 };

static const char* usages[] = {
    "program [options] [[--] args]",
    NULL,
};

static BkStatus popo_check_args(PopoArgs* args)
{
    if ((args->pid == 0)
        && (args->image_path == NULL))
        return BK_STATUS_BADARGS;
    return BK_STATUS_SUCCESS;
}

static void popo_usage(void)
{
    argparse_usage(&argparse);
}

BkStatus popo_parse_and_check_args(int argc, char** argv, PopoArgs* args)
{
    // Init parser
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("Main options"),
        OPT_STRING('s',
            "path",
            &args->image_path,
            "spawn the process given its image path",
            NULL,
            0,
            0),
        OPT_INTEGER('a',
            "pid",
            &args->pid,
            "attach to the process given its pid",
            NULL,
            0,
            0),
        OPT_END(),
    };

    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\npopo arguments:", "\n");

    // Check args. If wrong format, exit
    argc = argparse_parse(&argparse, argc, (const char**)argv);

    if (BK_FAILURE(popo_check_args(args)))
    {
        popo_usage();
        return BK_STATUS_BADARGS;
    }

    return BK_STATUS_SUCCESS;
}

#include "banner.h"
#include "args.h"
#include "tracer.h"

int main(int argc, char* argv[])
{
    popo_print_banner();

    PopoArgs args = { 0 };

    if (POPO_FAILURE(popo_parse_and_check_args(argc, argv, &args)))
        return POPO_STATUS_FAILURE;

    return popo_launch(args);
}

#include "banner.h"
#include "args.h"
#include "tracer.h"
#include "bk_types.h"

int main(int argc, char* argv[])
{
    popo_print_banner();

    PopoArgs args = { 0 };

    if (BK_FAILURE(popo_parse_and_check_args(argc, argv, &args)))
        return BK_STATUS_BADARGS;

    return popo_launch(args);
}

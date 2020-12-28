#include <stdio.h>
#include "banner.h"

#define POPO_ASCII_IMAGE_NAME "popo_ascii_image.txt"
#define POPO_ASCII_TEXT_NAME "popo_ascii_text.txt"

void popo_open_file_print_content(char* filename)
{
    FILE* f = fopen(filename, "r");
    if (!f)
        return;

    char line[100] = { 0 };
    while (fgets(line, 100, f))
    {
        printf("%s", line);
    }

    fclose(f);
}

void popo_print_banner(void)
{
    popo_open_file_print_content(POPO_ASCII_IMAGE_NAME);
    popo_open_file_print_content(POPO_ASCII_TEXT_NAME);

    puts("\n");
}

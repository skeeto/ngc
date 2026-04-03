#define _POSIX_C_SOURCE 200809L

#include "assemble.h"
#include "parse.h"
#include "dynarr.h"
#include "ngc.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FILE *fp = fmemopen((void *)data, size, "r");
    if (!fp)
        return 0;

    struct error err = {0};
    struct parsed_file file = {0};
    parsed_file_alloc(&file);

    size_t parse_result = parse_file(&err, &file, fp, LANG_FEAT_ALL);
    fclose(fp);

    if (parse_result == 0) {
        struct dynarr instructions = {0};
        dynarr_alloc(&instructions, 0x20, sizeof(ngc_word_t));
        assemble_file(&err, &instructions, file);
        dynarr_empty(&instructions);
    }

    parsed_file_empty(&file);
    return 0;
}

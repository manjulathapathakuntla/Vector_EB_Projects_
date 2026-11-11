/*
 * main.c
 *
 * A simple C preprocessor.
 *   Step 1: remove comments          (remove_comments.c)
 *   Step 2: file inclusion           (file_inclusion.c)
 *   Step 3: macro substitution       (macro.c)
 *
 * Usage:  ./my_preprocessor file.c
 * Output: file.i
 */
#include "preprocessor.h"

int main(int argc, char *argv[])
{
    char out_name[512];
    size_t len;
    FILE *out;

    if (argc != 2) {
        printf("Usage: %s <file.c>\n", argv[0]);
        return 1;
    }

    /* input must be a .c file */
    len = strlen(argv[1]);
    if (len < 3 || strcmp(argv[1] + len - 2, ".c") != 0) {
        printf("Error: input must be a .c file\n");
        return 1;
    }
    if (len >= sizeof(out_name)) {
        printf("Error: file name too long\n");
        return 1;
    }

    /* file.c  ->  file.i */
    strcpy(out_name, argv[1]);
    out_name[len - 1] = 'i';

    out = fopen(out_name, "w");
    if (out == NULL) {
        printf("Error: cannot create %s\n", out_name);
        return 1;
    }

    if (process_file(argv[1], out, 0) != 0) {
        fclose(out);
        remove(out_name);
        return 1;
    }

    fclose(out);
    printf("Done: %s -> %s\n", argv[1], out_name);
    return 0;
}

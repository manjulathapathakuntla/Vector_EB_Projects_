/*
 * file_inclusion.c
 *
 * Step 2: file inclusion, and the main loop over lines
 *   #include "file.h"  -> the header is processed and copied here
 *   #include <stdio.h> -> system header, line is kept as it is
 *   #define / #undef   -> stored in / removed from the macro table
 *   #ifdef / #ifndef / #if / #else / #endif -> keep or skip lines
 *   normal line        -> macros replaced, written to the .i file
 */
#include "preprocessor.h"

static int cond_stack[MAX_IF];          /* 1 = keep lines, 0 = skip   */
static int cond_top = 0;

/* lines are kept only if every open #if level is true */
static int is_active(void)
{
    int i;
    for (i = 0; i < cond_top; i++)
        if (cond_stack[i] == 0)
            return 0;
    return 1;
}

static void push_condition(int value)
{
    if (cond_top < MAX_IF)
        cond_stack[cond_top++] = value;
    else
        printf("Error: too many nested #if\n");
}

/* read the whole file into memory ("\r" of Windows files removed) */
char *read_file(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    char *buf;
    long size;
    size_t n, i, j;

    if (fp == NULL)
        return NULL;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buf = malloc((size_t)size + 1);
    if (buf == NULL) {
        fclose(fp);
        return NULL;
    }
    n = fread(buf, 1, (size_t)size, fp);
    fclose(fp);

    for (i = 0, j = 0; i < n; i++)
        if (buf[i] != '\r')
            buf[j++] = buf[i];
    buf[j] = '\0';
    return buf;
}

/* #include "name"  ->  look for name next to the current file */
static void do_include(const char *p, const char *filename, FILE *out,
                       int depth)
{
    char name[256], path[512];
    const char *slash1, *slash2, *slash;
    int k = 0, dir_len = 0;

    if (*p == '<') {                     /* system header: keep line */
        fprintf(out, "#include %s\n", p);
        return;
    }
    if (*p != '"') {
        printf("%s: Error: #include expects \"file\" or <file>\n", filename);
        return;
    }

    p++;
    while (*p && *p != '"' && k < (int)sizeof(name) - 1)
        name[k++] = *p++;
    name[k] = '\0';

    /* folder of the current file, e.g. "src/" from "src/a.c" */
    slash1 = strrchr(filename, '/');
    slash2 = strrchr(filename, '\\');
    slash = slash1;
    if (slash2 != NULL && (slash == NULL || slash2 > slash))
        slash = slash2;
    if (slash != NULL)
        dir_len = (int)(slash - filename) + 1;

    snprintf(path, sizeof(path), "%.*s%s", dir_len, filename, name);
    process_file(path, out, depth + 1);
}

static void handle_directive(const char *p, const char *filename, FILE *out,
                             int depth)
{
    char word[32], name[MAX_NAME];
    int k = 0;

    /* directive word: include, define, ifdef ... */
    while (*p == ' ' || *p == '\t')
        p++;
    while (isalpha((unsigned char)*p) && k < 31)
        word[k++] = *p++;
    word[k] = '\0';
    while (*p == ' ' || *p == '\t')
        p++;
    if (word[0] == '\0')
        return;                          /* a line with only "#" */

    /* conditions are checked even inside skipped parts */
    if (strcmp(word, "ifdef") == 0 || strcmp(word, "ifndef") == 0) {
        int defined;
        read_identifier(&p, name);
        defined = is_macro_defined(name);
        push_condition(strcmp(word, "ifdef") == 0 ? defined : !defined);
        return;
    }
    if (strcmp(word, "if") == 0) {
        char value[MAX_LINE];
        expand_macros(p, value, 0);      /* #if DEBUG  ->  #if 1 */
        push_condition(atoi(value) != 0);
        return;
    }
    if (strcmp(word, "else") == 0) {
        if (cond_top > 0)
            cond_stack[cond_top - 1] = !cond_stack[cond_top - 1];
        else
            printf("%s: Error: #else without #if\n", filename);
        return;
    }
    if (strcmp(word, "endif") == 0) {
        if (cond_top > 0)
            cond_top--;
        else
            printf("%s: Error: #endif without #if\n", filename);
        return;
    }

    if (!is_active())
        return;

    if (strcmp(word, "include") == 0) {
        do_include(p, filename, out, depth);
    } else if (strcmp(word, "define") == 0) {
        add_macro(p);
    } else if (strcmp(word, "undef") == 0) {
        read_identifier(&p, name);
        remove_macro(name);
    } else {
        fprintf(out, "#%s %s\n", word, p);   /* e.g. #pragma: keep it */
    }
}

int process_file(const char *filename, FILE *out, int depth)
{
    char *text, *code, *line;
    char logical[MAX_LINE];
    char expanded[MAX_LINE];

    if (depth > MAX_DEPTH) {
        printf("Error: #include nested too deeply (%s)\n", filename);
        return -1;
    }

    text = read_file(filename);
    if (text == NULL) {
        printf("Error: cannot open file %s\n", filename);
        return -1;
    }

    /* Step 1: remove comments */
    code = remove_comments(text);
    free(text);
    if (code == NULL)
        return -1;

    line = code;
    while (*line) {
        char *p;

        /* join lines ending with '\' into one logical line */
        logical[0] = '\0';
        for (;;) {
            char *nl = strchr(line, '\n');
            size_t n;

            if (nl != NULL)
                *nl = '\0';
            strncat(logical, line, sizeof(logical) - strlen(logical) - 1);
            line = (nl != NULL) ? nl + 1 : line + strlen(line);

            n = strlen(logical);
            if (n > 0 && logical[n - 1] == '\\' && nl != NULL) {
                logical[n - 1] = ' ';
                continue;
            }
            break;
        }

        p = logical;
        while (*p == ' ' || *p == '\t')
            p++;

        if (*p == '#') {
            handle_directive(p + 1, filename, out, depth);
        } else if (is_active()) {
            /* Step 3: replace macros */
            expand_macros(logical, expanded, 0);
            fprintf(out, "%s\n", expanded);
        }
    }

    free(code);
    return 0;
}

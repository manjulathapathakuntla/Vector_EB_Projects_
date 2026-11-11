/*
 * preprocessor.h
 *
 * Common header for my_preprocessor.
 *   remove_comments.c  Step 1: remove comments
 *   file_inclusion.c   Step 2: file inclusion and directives
 *   macro.c            Step 3: macro substitution
 *   main.c             main()
 */
#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE    4096    /* longest line we can handle        */
#define MAX_MACROS  500     /* how many #define macros we store  */
#define MAX_PARAMS  10      /* parameters of one macro           */
#define MAX_NAME    64      /* length of a macro / param name    */
#define MAX_BODY    1024    /* length of a macro body            */
#define MAX_ARG     256     /* length of one macro argument      */
#define MAX_DEPTH   20      /* nested #include / macro limit     */
#define MAX_IF      50      /* nested #if limit                  */

typedef struct {
    char name[MAX_NAME];
    int  is_function;                   /* 1 for  SQUARE(x)  type      */
    int  param_count;
    char params[MAX_PARAMS][MAX_NAME];
    char body[MAX_BODY];
    int  busy;                          /* 1 while being expanded      */
} Macro;

/* remove_comments.c */
char *remove_comments(const char *src);

/* macro.c */
int  read_identifier(const char **p, char *name);
int  is_macro_defined(const char *name);
void add_macro(const char *text);
void remove_macro(const char *name);
void expand_macros(const char *line, char *out, int depth);

/* file_inclusion.c */
char *read_file(const char *filename);
int   process_file(const char *filename, FILE *out, int depth);

#endif

/*
 * macro.c
 *
 * Step 3: macro substitution
 *   #define PI 3.14                 object-like macro
 *   #define SQUARE(x) ((x) * (x))   function-like macro
 *   #define STR(x) #x               # makes a string   ->  "x"
 *   #define CAT(a, b) a ## b        ## joins two words ->  ab
 */
#include "preprocessor.h"

static Macro macros[MAX_MACROS];
static int   macro_count = 0;

/* append n characters of s to out (out can hold MAX_LINE chars) */
static void append(char *out, size_t *j, const char *s, size_t n)
{
    while (n-- > 0 && *j < MAX_LINE - 1)
        out[(*j)++] = *s++;
    out[*j] = '\0';
}

/* copy a "string" or 'c' constant unchanged; returns pointer after it */
static const char *copy_literal(const char *p, char *out, size_t *j)
{
    char quote = *p;

    append(out, j, p++, 1);
    while (*p && *p != quote) {
        if (*p == '\\' && p[1])
            append(out, j, p++, 1);
        append(out, j, p++, 1);
    }
    if (*p)
        append(out, j, p++, 1);
    return p;
}

static void trim(char *s)
{
    char *start = s;
    size_t n;

    while (*start == ' ' || *start == '\t')
        start++;
    memmove(s, start, strlen(start) + 1);
    n = strlen(s);
    while (n > 0 && (s[n - 1] == ' ' || s[n - 1] == '\t'))
        s[--n] = '\0';
}

/* read a C identifier (letters, digits, _) into name; moves *p after it */
int read_identifier(const char **p, char *name)
{
    const char *s = *p;
    int k = 0;

    if (!(isalpha((unsigned char)*s) || *s == '_')) {
        name[0] = '\0';
        return 0;
    }
    while (isalnum((unsigned char)*s) || *s == '_') {
        if (k < MAX_NAME - 1)
            name[k++] = *s;
        s++;
    }
    name[k] = '\0';
    *p = s;
    return k;
}

static int find_macro(const char *name)
{
    int i;
    for (i = 0; i < macro_count; i++)
        if (strcmp(macros[i].name, name) == 0)
            return i;
    return -1;
}

int is_macro_defined(const char *name)
{
    return find_macro(name) >= 0;
}

/* text is everything after "#define" */
void add_macro(const char *text)
{
    Macro m;
    const char *p = text;
    int index;

    memset(&m, 0, sizeof(m));
    while (*p == ' ' || *p == '\t')
        p++;
    if (read_identifier(&p, m.name) == 0) {
        printf("Error: #define needs a macro name\n");
        return;
    }

    /* '(' right after the name -> function-like macro */
    if (*p == '(') {
        m.is_function = 1;
        p++;
        while (*p && *p != ')') {
            char param[MAX_NAME];
            if (read_identifier(&p, param) > 0) {
                if (m.param_count < MAX_PARAMS)
                    strcpy(m.params[m.param_count++], param);
            } else {
                p++;                     /* skip ',' and spaces */
            }
        }
        if (*p == ')')
            p++;
    }

    strncpy(m.body, p, MAX_BODY - 1);
    trim(m.body);

    index = find_macro(m.name);
    if (index >= 0)
        macros[index] = m;               /* redefine */
    else if (macro_count < MAX_MACROS)
        macros[macro_count++] = m;
    else
        printf("Error: too many macros\n");
}

void remove_macro(const char *name)
{
    int index = find_macro(name);
    if (index >= 0)
        macros[index] = macros[--macro_count];
}

/*
 * Read the arguments of a macro call.  p points at '('.
 * SQUARE(a + 1, f(x, y))  ->  "a + 1" and "f(x, y)"
 * Returns pointer after ')', or NULL if ')' is missing.
 */
static const char *read_arguments(const char *p, char args[][MAX_ARG],
                                  int *argc)
{
    int level = 0, k = 0;
    char quote = 0;

    *argc = 0;
    p++;                                 /* skip '(' */
    while (*p) {
        char c = *p;

        if (quote) {                     /* inside "..." */
            if (c == '\\' && p[1]) {
                if (k < MAX_ARG - 1 && *argc < MAX_PARAMS)
                    args[*argc][k++] = c;
                p++;
                c = *p;
            } else if (c == quote) {
                quote = 0;
            }
        } else if (c == '"' || c == '\'') {
            quote = c;
        } else if (c == '(') {
            level++;
        } else if (c == ')' && level > 0) {
            level--;
        } else if (c == ')' || c == ',') {
            /* end of one argument */
            if (*argc < MAX_PARAMS) {
                args[*argc][k] = '\0';
                trim(args[*argc]);
                (*argc)++;
            }
            k = 0;
            p++;
            if (c == ')')
                return p;
            continue;
        }

        if (k < MAX_ARG - 1 && *argc < MAX_PARAMS)
            args[*argc][k++] = c;
        p++;
    }
    return NULL;
}

static int param_index(const Macro *m, const char *name)
{
    int i;
    for (i = 0; i < m->param_count; i++)
        if (strcmp(m->params[i], name) == 0)
            return i;
    return -1;
}

/* put the arguments in place of the parameters in the macro body */
static void substitute(const Macro *m, char args[][MAX_ARG], char *result,
                       int depth)
{
    const char *p = m->body;
    char name[MAX_NAME];
    char temp[MAX_LINE];
    size_t j = 0;
    int i;

    result[0] = '\0';
    while (*p) {
        if (*p == '"' || *p == '\'') {
            p = copy_literal(p, result, &j);
        } else if (p[0] == '#' && p[1] == '#') {
            /* a ## b : remove the spaces around ## */
            while (j > 0 && (result[j - 1] == ' ' || result[j - 1] == '\t'))
                j--;
            result[j] = '\0';
            p += 2;
            while (*p == ' ' || *p == '\t')
                p++;
        } else if (*p == '#') {
            /* #x : put the argument inside quotes */
            p++;
            while (*p == ' ')
                p++;
            read_identifier(&p, name);
            i = param_index(m, name);
            append(result, &j, "\"", 1);
            append(result, &j, i >= 0 ? args[i] : name,
                   strlen(i >= 0 ? args[i] : name));
            append(result, &j, "\"", 1);
        } else if (isalpha((unsigned char)*p) || *p == '_') {
            read_identifier(&p, name);
            i = param_index(m, name);
            if (i >= 0) {
                /* argument is expanded first: SQUARE(SQUARE(2)) */
                expand_macros(args[i], temp, depth + 1);
                append(result, &j, temp, strlen(temp));
            } else {
                append(result, &j, name, strlen(name));
            }
        } else {
            append(result, &j, p++, 1);
        }
    }
}

/*
 * Replace every macro in line. Result goes to out (size MAX_LINE).
 * The result of a macro is checked again for more macros
 * (e.g. AREA(r) uses PI and SQUARE).
 */
void expand_macros(const char *line, char *out, int depth)
{
    const char *p = line;
    size_t j = 0;

    out[0] = '\0';
    while (*p) {
        if (*p == '"' || *p == '\'') {
            p = copy_literal(p, out, &j);
        } else if (isdigit((unsigned char)*p)) {
            /* number like 10UL or 0x1F: copy whole */
            while (isalnum((unsigned char)*p) || *p == '_' || *p == '.')
                append(out, &j, p++, 1);
        } else if (isalpha((unsigned char)*p) || *p == '_') {
            const char *start = p;
            char name[MAX_NAME];
            char result[MAX_LINE];
            char temp[MAX_LINE];
            Macro *m;
            int index;

            read_identifier(&p, name);
            index = find_macro(name);
            if (index < 0 || macros[index].busy || depth >= MAX_DEPTH) {
                append(out, &j, start, (size_t)(p - start));
                continue;
            }
            m = &macros[index];

            if (!m->is_function) {
                strcpy(result, m->body);
            } else {
                char args[MAX_PARAMS][MAX_ARG];
                const char *q = p;
                const char *after;
                int argc;

                while (*q == ' ' || *q == '\t')
                    q++;
                if (*q != '(') {         /* name used without (...) */
                    append(out, &j, start, (size_t)(p - start));
                    continue;
                }
                after = read_arguments(q, args, &argc);
                if (after == NULL) {
                    printf("Error: missing ')' in call of %s\n", m->name);
                    append(out, &j, start, strlen(start));
                    break;
                }
                if (m->param_count == 0 && argc == 1 && args[0][0] == '\0')
                    argc = 0;            /* F() with no parameters */
                if (argc != m->param_count) {
                    printf("Error: macro %s needs %d argument(s), %d given\n",
                           m->name, m->param_count, argc);
                    append(out, &j, start, (size_t)(after - start));
                    p = after;
                    continue;
                }
                substitute(m, args, result, depth);
                p = after;
            }

            /* check the result again; m is busy so  #define X X+1  stops */
            m->busy = 1;
            expand_macros(result, temp, depth + 1);
            m->busy = 0;
            append(out, &j, temp, strlen(temp));
        } else {
            append(out, &j, p++, 1);
        }
    }
}

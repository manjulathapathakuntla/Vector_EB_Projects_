/*
 * remove_comments.c
 *
 * Step 1: remove comments
 */
#include "preprocessor.h"

/*
 * Removes  // line comments  and  / * block comments * /.
 * Anything inside "strings" or 'c'har constants is not touched,
 * so  "http://abc"  stays as it is.
 */
char *remove_comments(const char *src)
{
    size_t len = strlen(src);
    size_t i = 0, j = 0;
    char *out = malloc(len + 1);

    if (out == NULL)
        return NULL;

    while (i < len) {
        if (src[i] == '"' || src[i] == '\'') {
            /* string or char constant: copy it unchanged */
            char quote = src[i];
            out[j++] = src[i++];
            while (i < len && src[i] != quote && src[i] != '\n') {
                if (src[i] == '\\' && i + 1 < len)
                    out[j++] = src[i++];        /* escape like \" */
                out[j++] = src[i++];
            }
            if (i < len && src[i] == quote)
                out[j++] = src[i++];
        } else if (src[i] == '/' && src[i + 1] == '/') {
            /* line comment: skip till end of line */
            while (i < len && src[i] != '\n')
                i++;
        } else if (src[i] == '/' && src[i + 1] == '*') {
            /* block comment: replace with one space,
               keep the newlines so line count stays same */
            out[j++] = ' ';
            i += 2;
            while (i < len && !(src[i] == '*' && src[i + 1] == '/')) {
                if (src[i] == '\n')
                    out[j++] = '\n';
                i++;
            }
            i += 2;
        } else {
            out[j++] = src[i++];
        }
    }
    out[j] = '\0';
    return out;
}

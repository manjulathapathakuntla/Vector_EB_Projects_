/*
 * stud_util.c
 * Input helpers, validation, searching and list cleanup shared by all modules.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "student.h"

/* Reads one line from stdin (newline removed). Returns 0 on EOF, 1 otherwise.
 * Any characters beyond the buffer size are discarded. */
int read_line(const char *prompt, char *buf, size_t size)
{
    size_t len;
    int c;

    if (prompt != NULL)
    {
        printf("%s", prompt);
        fflush(stdout);
    }

    if (fgets(buf, (int)size, stdin) == NULL)
    {
        buf[0] = '\0';
        return 0;
    }

    len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
        buf[--len] = '\0';
    else
        while ((c = getchar()) != '\n' && c != EOF)
            ;   /* discard the rest of an over-long line */

    if (len > 0 && buf[len - 1] == '\r')   /* Windows line endings */
        buf[--len] = '\0';

    return 1;
}

/* Removes leading and trailing white space in place. */
void trim(char *s)
{
    char *start = s;
    size_t len;

    while (*start != '\0' && isspace((unsigned char)*start))
        start++;
    if (start != s)
        memmove(s, start, strlen(start) + 1);

    len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1]))
        s[--len] = '\0';
}

/* Reads a single-character choice. Returns the lowercase character,
 * 0 for invalid input (empty / more than one character), EOF on end of input. */
int read_choice(const char *prompt)
{
    char buf[INPUT_LEN];

    if (!read_line(prompt, buf, sizeof buf))
        return EOF;
    trim(buf);
    if (buf[0] == '\0' || buf[1] != '\0')
        return 0;
    return tolower((unsigned char)buf[0]);
}

/* Reads an integer. Returns 1 on success, 0 on invalid input, -1 on EOF. */
int read_int(const char *prompt, int *out)
{
    char buf[INPUT_LEN];
    char *end;
    long value;

    if (!read_line(prompt, buf, sizeof buf))
        return -1;
    trim(buf);
    if (buf[0] == '\0')
        return 0;

    value = strtol(buf, &end, 10);
    if (*end != '\0' || value < -2147483647L || value > 2147483647L)
        return 0;

    *out = (int)value;
    return 1;
}

/* Parses a percentage in the range 0.00 - 100.00. Returns 1 if valid. */
int parse_percentage(const char *text, float *out)
{
    char *end;
    double value;

    if (text[0] == '\0')
        return 0;
    value = strtod(text, &end);
    if (*end != '\0')
        return 0;
    if (!(value >= 0.0 && value <= 100.0))   /* also rejects NaN */
        return 0;

    *out = (float)value;
    return 1;
}

/* Keeps asking until a valid percentage is entered. Returns 0 on EOF. */
int read_percentage(const char *prompt, float *out)
{
    char buf[INPUT_LEN];

    for (;;)
    {
        if (!read_line(prompt, buf, sizeof buf))
            return 0;
        trim(buf);
        if (parse_percentage(buf, out))
            return 1;
        printf("Invalid percentage. Enter a value between 0.00 and 100.00.\n");
    }
}

/* Checks a (trimmed) name. Prints the reason and returns 0 if invalid. */
int validate_name(const char *name)
{
    if (name[0] == '\0')
    {
        printf("Name should not be empty.\n");
        return 0;
    }
    if (strlen(name) >= NAME_LEN)
    {
        printf("Name is too long (maximum %d characters).\n", NAME_LEN - 1);
        return 0;
    }
    return 1;
}

/* Keeps asking until a valid name is entered. buf must hold NAME_LEN bytes.
 * Returns 0 on EOF. */
int read_name(const char *prompt, char *buf)
{
    char tmp[INPUT_LEN];

    for (;;)
    {
        if (!read_line(prompt, tmp, sizeof tmp))
            return 0;
        trim(tmp);
        if (validate_name(tmp))
        {
            strcpy(buf, tmp);
            return 1;
        }
    }
}

/* Case-insensitive string comparison (portable replacement for strcasecmp). */
int str_icmp(const char *a, const char *b)
{
    int ca, cb;

    do
    {
        ca = tolower((unsigned char)*a++);
        cb = tolower((unsigned char)*b++);
    } while (ca == cb && ca != '\0');

    return ca - cb;
}

STUDENT *find_by_rollno(STUDENT *head, int rollno)
{
    while (head != NULL && head->rollno != rollno)
        head = head->next;
    return head;
}

int match_name(const STUDENT *node, const void *key)
{
    return str_icmp(node->name, (const char *)key) == 0;
}

int match_percentage(const STUDENT *node, const void *key)
{
    float diff = node->percentage - *(const float *)key;
    if (diff < 0)
        diff = -diff;
    return diff < 0.005f;   /* equal when rounded to 2 decimal places */
}

/*
 * Shows every record that satisfies match(). If there is exactly one match it is
 * returned directly; if there are several, the user is asked for the roll number
 * of the intended record. Returns NULL when nothing suitable is selected.
 */
STUDENT *pick_from_matches(STUDENT *head, match_fn match, const void *key,
                           const char *action)
{
    STUDENT *p, *first = NULL, *chosen;
    int count = 0, roll, r;

    for (p = head; p != NULL; p = p->next)
        if (match(p, key))
        {
            if (count == 0)
            {
                printf("\nMatching record(s):\n");
                print_table_header();
                first = p;
            }
            print_table_row(p);
            count++;
        }

    if (count == 0)
    {
        printf("No matching record found.\n");
        return NULL;
    }
    print_table_footer();

    if (count == 1)
        return first;

    printf("%d records match. ", count);
    printf("Enter the roll number of the record to %s: ", action);
    r = read_int(NULL, &roll);
    if (r != 1)
    {
        printf("Invalid roll number.\n");
        return NULL;
    }

    chosen = find_by_rollno(head, roll);
    if (chosen == NULL || !match(chosen, key))
    {
        printf("Roll number %d is not one of the matching records.\n", roll);
        return NULL;
    }
    return chosen;
}

/* Frees every node, sets *head to NULL and returns how many nodes were freed. */
int free_list(STUDENT **head)
{
    STUDENT *cur = *head, *next;
    int count = 0;

    while (cur != NULL)
    {
        next = cur->next;
        free(cur);
        cur = next;
        count++;
    }
    *head = NULL;
    return count;
}

/*
 * student.h
 * Structure definition, shared declarations and function prototypes
 * for the Student Record Management System (Mini Project - II).
 */
#ifndef STUDENT_H
#define STUDENT_H

#include <stddef.h>

#define DATA_FILE   "student.dat"
#define TEMP_FILE   "student.tmp"
#define NAME_LEN    50
#define INPUT_LEN   256

struct student
{
    int rollno;
    char name[NAME_LEN];
    float percentage;
    struct student *next;
};

typedef struct student STUDENT;

/* Match callback used when searching by name / percentage */
typedef int (*match_fn)(const STUDENT *node, const void *key);

/* ---------- stud_add.c ---------- */
void add_record(STUDENT **head);
int  next_free_rollno(const STUDENT *head);
void append_node(STUDENT **head, STUDENT *node);

/* ---------- stud_del.c ---------- */
void delete_record(STUDENT **head);
int  delete_by_rollno(STUDENT **head, int rollno);
void delete_all_records(STUDENT **head);

/* ---------- stud_show.c ---------- */
void show_list(const STUDENT *head);
void print_table_header(void);
void print_table_row(const STUDENT *node);
void print_table_footer(void);

/* ---------- stud_mod.c ---------- */
void modify_record(STUDENT *head);

/* ---------- stud_save.c ---------- */
int  save_records(const STUDENT *head);
int  load_records(STUDENT **head);
int  exit_menu(STUDENT **head);

/* ---------- stud_sort.c ---------- */
void sort_menu(STUDENT **head);
void reverse_list(STUDENT **head);

/* ---------- stud_util.c ---------- */
int      read_line(const char *prompt, char *buf, size_t size);
void     trim(char *s);
int      read_choice(const char *prompt);
int      read_int(const char *prompt, int *out);
int      parse_percentage(const char *text, float *out);
int      read_percentage(const char *prompt, float *out);
int      validate_name(const char *name);
int      read_name(const char *prompt, char *buf);
int      str_icmp(const char *a, const char *b);
STUDENT *find_by_rollno(STUDENT *head, int rollno);
int      match_name(const STUDENT *node, const void *key);
int      match_percentage(const STUDENT *node, const void *key);
STUDENT *pick_from_matches(STUDENT *head, match_fn match, const void *key,
                           const char *action);
int      free_list(STUDENT **head);

#endif /* STUDENT_H */

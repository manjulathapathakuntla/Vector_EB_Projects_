/*
 * stud_show.c
 * Displays the student list in tabular form.
 */
#include <stdio.h>
#include "student.h"

#define LINE "------------------------------------------------------------"

void print_table_header(void)
{
    printf("%s\n", LINE);
    printf("%-10s %-35s %12s\n", "Roll No.", "Name", "Percentage");
    printf("%s\n", LINE);
}

void print_table_row(const STUDENT *node)
{
    printf("%-10d %-35s %12.2f\n", node->rollno, node->name, node->percentage);
}

void print_table_footer(void)
{
    printf("%s\n", LINE);
}

void show_list(const STUDENT *head)
{
    int count = 0;

    if (head == NULL)
    {
        printf("\nNo student records available.\n");
        return;
    }

    printf("\n");
    print_table_header();
    for (; head != NULL; head = head->next)
    {
        print_table_row(head);
        count++;
    }
    print_table_footer();
    printf("Total records: %d\n", count);
}

/*
 * stud_mod.c
 * Modifies an existing record found by roll number, name or percentage.
 * The roll number of a record never changes.
 */
#include <stdio.h>
#include <string.h>
#include "student.h"

/* Asks for the new name and percentage. Pressing Enter keeps the old value. */
static void edit_record(STUDENT *node)
{
    char buf[INPUT_LEN];
    float perc;

    printf("\nCurrent details:\n");
    print_table_header();
    print_table_row(node);
    print_table_footer();

    for (;;)
    {
        printf("Enter new name (press Enter to keep \"%s\"): ", node->name);
        if (!read_line(NULL, buf, sizeof buf))
            return;
        trim(buf);
        if (buf[0] == '\0')
            break;                          /* keep current name */
        if (validate_name(buf))
        {
            strcpy(node->name, buf);
            break;
        }
    }

    for (;;)
    {
        printf("Enter new percentage (press Enter to keep %.2f): ", node->percentage);
        if (!read_line(NULL, buf, sizeof buf))
            return;
        trim(buf);
        if (buf[0] == '\0')
            break;                          /* keep current percentage */
        if (parse_percentage(buf, &perc))
        {
            node->percentage = perc;
            break;
        }
        printf("Invalid percentage. Enter a value between 0.00 and 100.00.\n");
    }

    printf("Record with roll number %d modified successfully.\n", node->rollno);
}

void modify_record(STUDENT *head)
{
    int choice, roll, r;
    char name[NAME_LEN];
    float perc;
    STUDENT *target = NULL;

    if (head == NULL)
    {
        printf("\nNo student records available.\n");
        return;
    }

    printf("\nEnter which record to search for modification\n\n");
    printf("R/r : Search by roll number\n");
    printf("N/n : Search by name\n");
    printf("P/p : Search by percentage\n");
    choice = read_choice("Enter your choice: ");

    switch (choice)
    {
    case 'r':
        r = read_int("Enter roll number: ", &roll);
        if (r == -1)
            return;
        if (r == 0 || roll <= 0)
        {
            printf("Invalid roll number. It must be a positive integer.\n");
            return;
        }
        target = find_by_rollno(head, roll);
        if (target == NULL)
            printf("No record found with roll number %d.\n", roll);
        break;

    case 'n':
        if (!read_name("Enter name: ", name))
            return;
        target = pick_from_matches(head, match_name, name, "modify");
        break;

    case 'p':
        if (!read_percentage("Enter percentage: ", &perc))
            return;
        target = pick_from_matches(head, match_percentage, &perc, "modify");
        break;

    case EOF:
        return;

    default:
        printf("Invalid choice.\n");
    }

    if (target != NULL)
        edit_record(target);
}

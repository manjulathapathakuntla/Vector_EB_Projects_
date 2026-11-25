/*
 * stud_del.c
 * Deletes one record (by roll number or name) or all records.
 */
#include <stdio.h>
#include <stdlib.h>
#include "student.h"

/* Unlinks and frees the node with the given roll number. Returns 1 if deleted. */
int delete_by_rollno(STUDENT **head, int rollno)
{
    STUDENT *cur = *head, *prev = NULL;

    while (cur != NULL && cur->rollno != rollno)
    {
        prev = cur;
        cur = cur->next;
    }
    if (cur == NULL)
        return 0;

    if (prev == NULL)
        *head = cur->next;      /* deleting the first node */
    else
        prev->next = cur->next;

    free(cur);
    return 1;
}

void delete_record(STUDENT **head)
{
    int choice, roll, r;
    char name[NAME_LEN];
    STUDENT *target;

    if (*head == NULL)
    {
        printf("\nNo student records available.\n");
        return;
    }

    printf("\nR/r : Enter roll number to delete\n");
    printf("N/n : Enter name to delete\n");
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
        if (delete_by_rollno(head, roll))
            printf("Record with roll number %d deleted successfully.\n", roll);
        else
            printf("No record found with roll number %d.\n", roll);
        break;

    case 'n':
        if (!read_name("Enter name: ", name))
            return;
        target = pick_from_matches(*head, match_name, name, "delete");
        if (target == NULL)
            return;
        roll = target->rollno;  /* final deletion is done by roll number */
        if (delete_by_rollno(head, roll))
            printf("Record with roll number %d deleted successfully.\n", roll);
        break;

    case EOF:
        return;

    default:
        printf("Invalid choice.\n");
    }
}

/* Frees every node. student.dat is not touched until the user saves. */
void delete_all_records(STUDENT **head)
{
    int count;

    if (*head == NULL)
    {
        printf("\nNo student records available.\n");
        return;
    }
    count = free_list(head);
    printf("\nAll %d record(s) deleted from memory.\n", count);
    printf("(%s is unchanged until you save.)\n", DATA_FILE);
}

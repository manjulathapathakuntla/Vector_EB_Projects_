/*
 * stud_add.c
 * Adds a new dynamically allocated student record.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"

/* Smallest positive integer not already used as a roll number.
 * Example: if 1, 2, 4 and 5 exist, the result is 3. */
int next_free_rollno(const STUDENT *head)
{
    int candidate = 1;
    const STUDENT *p = head;

    while (p != NULL)
    {
        if (p->rollno == candidate)
        {
            candidate++;
            p = head;       /* restart the scan with the new candidate */
        }
        else
            p = p->next;
    }
    return candidate;
}

/* Links a node at the end of the list. */
void append_node(STUDENT **head, STUDENT *node)
{
    STUDENT *p;

    node->next = NULL;
    if (*head == NULL)
    {
        *head = node;
        return;
    }
    for (p = *head; p->next != NULL; p = p->next)
        ;
    p->next = node;
}

void add_record(STUDENT **head)
{
    STUDENT *node;

    node = (STUDENT *)malloc(sizeof(STUDENT));
    if (node == NULL)
    {
        printf("Memory allocation failed. Record not added.\n");
        return;
    }

    node->rollno = next_free_rollno(*head);
    node->next = NULL;
    printf("\nRoll number assigned: %d\n", node->rollno);

    if (!read_name("Enter student name: ", node->name) ||
        !read_percentage("Enter percentage (0.00 - 100.00): ", &node->percentage))
    {
        printf("\nInput ended. Record not added.\n");
        free(node);
        return;
    }

    append_node(head, node);
    printf("Record added successfully (Roll No. %d).\n", node->rollno);
}

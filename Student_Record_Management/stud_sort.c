/*
 * stud_sort.c
 * Sorts the list (by name or by percentage) and reverses it.
 * Both operations re-link the existing nodes; no node is copied or reallocated.
 */
#include <stdio.h>
#include <string.h>
#include "student.h"

typedef int (*compare_fn)(const STUDENT *a, const STUDENT *b);

/* Alphabetical, ignoring case; ties broken by roll number. */
static int compare_name(const STUDENT *a, const STUDENT *b)
{
    int c = str_icmp(a->name, b->name);
    if (c == 0)
        c = strcmp(a->name, b->name);
    if (c == 0)
        c = a->rollno - b->rollno;
    return c;
}

/* Highest percentage first; ties broken by roll number. */
static int compare_percentage(const STUDENT *a, const STUDENT *b)
{
    if (a->percentage > b->percentage)
        return -1;
    if (a->percentage < b->percentage)
        return 1;
    return a->rollno - b->rollno;
}

/* Merges two sorted lists by re-linking their nodes. */
static STUDENT *merge(STUDENT *a, STUDENT *b, compare_fn cmp)
{
    STUDENT dummy;
    STUDENT *tail = &dummy;

    dummy.next = NULL;
    while (a != NULL && b != NULL)
    {
        if (cmp(a, b) <= 0)
        {
            tail->next = a;
            a = a->next;
        }
        else
        {
            tail->next = b;
            b = b->next;
        }
        tail = tail->next;
    }
    tail->next = (a != NULL) ? a : b;
    return dummy.next;
}

/* Merge sort on a singly linked list: O(n log n), changes links only. */
static STUDENT *merge_sort(STUDENT *head, compare_fn cmp)
{
    STUDENT *slow, *fast, *second;

    if (head == NULL || head->next == NULL)
        return head;

    /* split the list into two halves */
    slow = head;
    fast = head->next;
    while (fast != NULL && fast->next != NULL)
    {
        slow = slow->next;
        fast = fast->next->next;
    }
    second = slow->next;
    slow->next = NULL;

    return merge(merge_sort(head, cmp), merge_sort(second, cmp), cmp);
}

void sort_menu(STUDENT **head)
{
    int choice;

    if (*head == NULL)
    {
        printf("\nNo student records available.\n");
        return;
    }

    printf("\nN/n : Sort with name\n");
    printf("P/p : Sort with percentage\n");
    choice = read_choice("Enter your choice: ");

    switch (choice)
    {
    case 'n':
        *head = merge_sort(*head, compare_name);
        printf("List sorted by name.\n");
        show_list(*head);
        break;

    case 'p':
        *head = merge_sort(*head, compare_percentage);
        printf("List sorted by percentage (highest first).\n");
        show_list(*head);
        break;

    case EOF:
        break;

    default:
        printf("Invalid choice.\n");
    }
}

/* Reverses the list in place by turning every next pointer around. */
void reverse_list(STUDENT **head)
{
    STUDENT *prev = NULL, *cur = *head, *next;

    if (cur == NULL)
    {
        printf("\nNo student records available.\n");
        return;
    }

    while (cur != NULL)
    {
        next = cur->next;
        cur->next = prev;
        prev = cur;
        cur = next;
    }
    *head = prev;

    printf("\nList reversed.\n");
    show_list(*head);
}

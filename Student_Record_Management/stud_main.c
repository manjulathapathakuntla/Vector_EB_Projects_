/*
 * stud_main.c
 * Entry point: loads student.dat, shows the menu and dispatches each option.
 */
#include <stdio.h>
#include "student.h"

static void show_menu(void)
{
    printf("\n******** STUDENT RECORD MENU ********\n\n");
    printf("a/A : Add new record\n");
    printf("d/D : Delete a record\n");
    printf("s/S : Show the list\n");
    printf("m/M : Modify a record\n");
    printf("v/V : Save records\n");
    printf("e/E : Exit\n");
    printf("t/T : Sort the list\n");
    printf("l/L : Delete all the records\n");
    printf("r/R : Reverse the list\n\n");
}

int main(void)
{
    STUDENT *head = NULL;
    int choice, count, running = 1;

    count = load_records(&head);
    if (count > 0)
        printf("%d record(s) loaded from %s.\n", count, DATA_FILE);
    else
        printf("Starting with an empty student list.\n");

    while (running)
    {
        show_menu();
        choice = read_choice("Enter your choice: ");

        switch (choice)
        {
        case 'a':
            add_record(&head);
            break;
        case 'd':
            delete_record(&head);
            break;
        case 's':
            show_list(head);
            break;
        case 'm':
            modify_record(head);
            break;
        case 'v':
            count = save_records(head);
            if (count >= 0)
                printf("\n%d record(s) saved to %s.\n", count, DATA_FILE);
            break;
        case 'e':
            if (exit_menu(&head))
                running = 0;
            break;
        case 't':
            sort_menu(&head);
            break;
        case 'l':
            delete_all_records(&head);
            break;
        case 'r':
            reverse_list(&head);
            break;
        case EOF:
            /* input closed: never leak memory, exit without saving */
            count = free_list(&head);
            printf("\nInput ended. Exiting without saving (%d node(s) freed).\n", count);
            running = 0;
            break;
        default:
            printf("\nInvalid choice. Please try again.\n");
        }
    }

    return 0;
}

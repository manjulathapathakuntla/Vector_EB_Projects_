/*
 * stud_save.c
 * Saves records to / loads records from student.dat and handles the exit menu.
 *
 * File format (binary):
 *   4-byte signature "SRM1"
 *   then for every record: int rollno, char name[NAME_LEN], float percentage
 * The next pointer is never written, because addresses are meaningless on disk.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"

static const char SIGNATURE[4] = { 'S', 'R', 'M', '1' };

/* Writes all records. Returns the number saved, or -1 on error.
 * Data goes to a temporary file first so a failed save cannot corrupt
 * the previous student.dat. */
int save_records(const STUDENT *head)
{
    FILE *fp;
    int count = 0, ok = 1;

    fp = fopen(TEMP_FILE, "wb");
    if (fp == NULL)
    {
        printf("Error: cannot open %s for writing.\n", TEMP_FILE);
        return -1;
    }

    if (fwrite(SIGNATURE, sizeof SIGNATURE, 1, fp) != 1)
        ok = 0;

    for (; ok && head != NULL; head = head->next)
    {
        if (fwrite(&head->rollno, sizeof head->rollno, 1, fp) != 1 ||
            fwrite(head->name, sizeof head->name, 1, fp) != 1 ||
            fwrite(&head->percentage, sizeof head->percentage, 1, fp) != 1)
            ok = 0;
        else
            count++;
    }

    if (fclose(fp) != 0)
        ok = 0;

    if (!ok)
    {
        remove(TEMP_FILE);
        printf("Error: failed while writing records.\n");
        return -1;
    }

    remove(DATA_FILE);                      /* rename() fails on Windows if it exists */
    if (rename(TEMP_FILE, DATA_FILE) != 0)
    {
        printf("Error: could not replace %s.\n", DATA_FILE);
        return -1;
    }
    return count;
}

/* Loads records at startup. A missing file simply means an empty list.
 * Returns the number of records loaded. */
int load_records(STUDENT **head)
{
    FILE *fp;
    char sig[4];
    int count = 0, skipped = 0;
    STUDENT rec, *node, *tail = NULL;

    *head = NULL;
    fp = fopen(DATA_FILE, "rb");
    if (fp == NULL)
        return 0;

    if (fread(sig, sizeof sig, 1, fp) != 1 || memcmp(sig, SIGNATURE, sizeof sig) != 0)
    {
        printf("Warning: %s is not a valid data file. Starting with an empty list.\n",
               DATA_FILE);
        fclose(fp);
        return 0;
    }

    while (fread(&rec.rollno, sizeof rec.rollno, 1, fp) == 1 &&
           fread(rec.name, sizeof rec.name, 1, fp) == 1 &&
           fread(&rec.percentage, sizeof rec.percentage, 1, fp) == 1)
    {
        rec.name[NAME_LEN - 1] = '\0';

        /* reject corrupted or duplicate entries */
        if (rec.rollno <= 0 || rec.name[0] == '\0' ||
            !(rec.percentage >= 0.0f && rec.percentage <= 100.0f) ||
            find_by_rollno(*head, rec.rollno) != NULL)
        {
            skipped++;
            continue;
        }

        node = (STUDENT *)malloc(sizeof(STUDENT));
        if (node == NULL)
        {
            printf("Memory allocation failed while loading records.\n");
            break;
        }
        *node = rec;
        node->next = NULL;

        if (tail == NULL)                   /* keep the saved order */
            *head = node;
        else
            tail->next = node;
        tail = node;
        count++;
    }

    fclose(fp);
    if (skipped > 0)
        printf("Warning: %d invalid record(s) in %s were skipped.\n", skipped, DATA_FILE);
    return count;
}

/* Exit sub-menu. Returns 1 if the program should terminate, 0 to go back. */
int exit_menu(STUDENT **head)
{
    int choice, saved, freed;

    printf("\nS/s : Save and exit\n");
    printf("E/e : Exit without saving\n");
    choice = read_choice("Enter your choice: ");

    switch (choice)
    {
    case 's':
        saved = save_records(*head);
        if (saved < 0)
        {
            printf("Save failed. Returning to the menu so no data is lost.\n");
            return 0;
        }
        printf("%d record(s) saved to %s.\n", saved, DATA_FILE);
        freed = free_list(head);
        printf("%d node(s) freed. Goodbye!\n", freed);
        return 1;

    case 'e':
    case EOF:
        freed = free_list(head);
        printf("\nExiting without saving. %d node(s) freed. Goodbye!\n", freed);
        return 1;

    default:
        printf("Invalid choice. Returning to the main menu.\n");
        return 0;
    }
}

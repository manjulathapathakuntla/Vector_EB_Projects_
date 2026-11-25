# Student Record Management System (Mini Project – II)

A menu-driven C program that keeps student records in a **singly linked list**,
allocates every node with `malloc()`, frees every node with `free()`, and stores
records in **student.dat**.

## Files

| File          | Responsibility                                                     |
|---------------|--------------------------------------------------------------------|
| `student.h`   | `struct student`, constants and all function prototypes            |
| `stud_main.c` | `main()`, menu, program flow, loads `student.dat` at startup       |
| `stud_add.c`  | Add record (smallest unused positive roll number)                  |
| `stud_del.c`  | Delete by roll number / name, delete all records                   |
| `stud_show.c` | Display the list in a table                                        |
| `stud_mod.c`  | Modify by roll number / name / percentage                          |
| `stud_save.c` | Save / load `student.dat`, exit menu (save & exit / exit)          |
| `stud_sort.c` | Sort by name or percentage (merge sort on links), reverse the list |
| `stud_util.c` | Input reading & validation, searching, freeing the list            |
| `Makefile`    | Builds the executable `student`                                    |

## Compile

With `make`:

```
make
```

Or by hand:

```
gcc -Wall -Wextra -std=c99 -o student stud_main.c stud_add.c stud_del.c stud_show.c stud_mod.c stud_save.c stud_sort.c stud_util.c
```

## Run

```
./student          (Linux / WSL / macOS)
student.exe        (Windows, MinGW)
```

`student.dat` is created in the current directory the first time you save (`v`,
or `e` then `s`). If it exists at startup, its records are loaded automatically.

## Menu

| Key | Action                                                     |
|-----|------------------------------------------------------------|
| a/A | Add a new record                                           |
| d/D | Delete by roll number (`r`) or name (`n`)                  |
| s/S | Show all records                                           |
| m/M | Modify by roll number (`r`), name (`n`) or percentage (`p`)|
| v/V | Save records to `student.dat`                              |
| e/E | Exit: `s` = save and exit, `e` = exit without saving       |
| t/T | Sort by name (`n`, A→Z) or percentage (`p`, highest first) |
| l/L | Delete all records from memory (file unchanged until save) |
| r/R | Reverse the list                                           |

## Design notes

- **Roll numbers**: a new record gets the smallest positive number not in use
  (1, 2, 4, 5 present → next is 3). Roll numbers never change on modify.
- **Duplicate names**: delete/modify by name (and modify by percentage) lists
  every match and asks for the roll number of the intended record.
  Name matching ignores case.
- **Sorting** uses merge sort that re-links the existing nodes; **reverse**
  flips the `next` pointers in place. No nodes are copied.
- **Validation**: percentage must be 0.00–100.00, name must be non-empty
  (max 49 characters), roll numbers must be positive; invalid menu input
  just shows the menu again.
- **Memory**: every exit path (save & exit, exit without saving, end of input)
  frees all nodes.
- **File format**: binary – a 4-byte signature `SRM1`, then for each record
  `rollno`, `name[50]`, `percentage`. The `next` pointer is not stored.
  Saving writes to `student.tmp` first, then replaces `student.dat`, so a
  failed save never damages the previous file.

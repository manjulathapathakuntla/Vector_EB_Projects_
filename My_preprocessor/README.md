# my_preprocessor

`my_preprocessor` is a simple **C preprocessor** written in C.

It takes a C source file (`file.c`) and creates an expanded source file (`file.i`). To do this it:

1. removes comments
2. includes header files
3. substitutes macros

## Project details

| | |
|---|---|
| Project | Preprocessor (Mini Project) |
| Task | Develop a preprocessor tool named `my_preprocessor` that processes a C source file by performing comment removal, file inclusion and macro substitution, and generates an extended source file with a `.i` extension |
| Language | C (C99) |
| Type | Software only, no hardware |

---

## 1. Features

### Comment removal
* Removes single-line comments: `// ...`
* Removes multi-line comments: `/* ... */`
* Leaves comment symbols inside strings alone, so `"http://abc"` stays as it is.

### File inclusion
* `#include "file.h"`: the contents of `file.h` are copied in place of this line.
* A header can include another header (nested include).
* Header guards (`#ifndef` / `#define` / `#endif`) work, so a header included twice is copied only once.
* `#include <stdio.h>`: system headers are left unchanged.

### Macro substitution
| Type | Example | Result |
|------|---------|--------|
| Simple macro | `#define MAX 100` → `MAX` | `100` |
| Macro with arguments | `#define SQUARE(x) ((x)*(x))` → `SQUARE(5)` | `((5)*(5))` |
| Nested macro | `SQUARE(SQUARE(2))` | `((((2)*(2)))*(((2)*(2))))` |
| Stringify `#` | `#define STR(x) #x` → `STR(hello)` | `"hello"` |
| Token join `##` | `#define CAT(a,b) a##b` → `CAT(var,1)` | `var1` |
| Multi-line macro | `#define ADD(a,b) \`<br>`((a)+(b))` | works |
| Undefine | `#undef MAX` | `MAX` is no longer replaced |

### Conditional compilation
`#ifdef`, `#ifndef`, `#if`, `#else`, `#endif`

---

## 2. Project files

```
my_preprocessor/
├── preprocessor.h       common header
├── main.c               main()
├── remove_comments.c    Step 1: comment removal
├── file_inclusion.c     Step 2: file inclusion and directives
├── macro.c              Step 3: macro substitution
├── sample_test.c        sample input
├── sample_test.h        sample header
├── sample_test.i        sample output
├── Makefile
└── README.md
```

| File | Main functions | Work |
|------|----------------|------|
| `preprocessor.h` | | Limits, `Macro` structure, function declarations |
| `main.c` | `main()` | Checks the input file and creates the `.i` output file |
| `remove_comments.c` | `remove_comments()` | Step 1: removes comments |
| `file_inclusion.c` | `process_file()`, `do_include()`, `handle_directive()` | Step 2: reads the file line by line, handles `#include` and the other directives |
| `macro.c` | `add_macro()`, `expand_macros()`, `substitute()` | Step 3: stores macros and replaces them in the code |
| `sample_test.c`, `sample_test.h` | | Sample input program and header |
| `sample_test.i` | | Output created by `make run` |

---

## 3. Compile

### Requirements
* `gcc`
* `make`
* Linux, or on Windows: WSL, MSYS2 or MinGW

```sh
make
```

This runs:
```sh
gcc -Wall -Wextra -std=c99 -O2 -c main.c -o main.o
gcc -Wall -Wextra -std=c99 -O2 -c remove_comments.c -o remove_comments.o
gcc -Wall -Wextra -std=c99 -O2 -c file_inclusion.c -o file_inclusion.o
gcc -Wall -Wextra -std=c99 -O2 -c macro.c -o macro.o
gcc -Wall -Wextra -std=c99 -O2 -o my_preprocessor main.o remove_comments.o file_inclusion.o macro.o
```

Without `make`, in one line:
```sh
gcc -Wall -Wextra -std=c99 -O2 -o my_preprocessor main.c remove_comments.c file_inclusion.c macro.c
```

## 4. Run

```sh
make run
```

This builds the program if needed, then runs `./my_preprocessor sample_test.c`.

Output:
```
./my_preprocessor sample_test.c
Done: sample_test.c -> sample_test.i
Generated sample_test.i from sample_test.c
```

The new file `sample_test.i` is created in the same folder as `sample_test.c`. If you change `sample_test.c`, `sample_test.h` or any of the program's source files, run `make run` again and `sample_test.i` is rebuilt.

To run the program on a different file:
```sh
./my_preprocessor file.c
```

To remove the program, the object files and `sample_test.i`:
```sh
make clean
```

### Make commands

| Command | What it does |
|---------|--------------|
| `make` | Compiles the `.c` files into `my_preprocessor` |
| `make run` | Compiles if needed, then creates `sample_test.i` from `sample_test.c` |
| `make clean` | Deletes `my_preprocessor`, the `.o` files and `sample_test.i` |

To check that the output is correct C code:
```sh
gcc -x c sample_test.i -o sample_test
./sample_test
```
Expected:
```
12.560000
100
5
```

---

## 5. Example

**Input: `sample_test.h`**
```c
#ifndef SAMPLE_TEST_H
#define SAMPLE_TEST_H

#define MAX 100
int add(int a, int b);

#endif
```

**Input: `sample_test.c`**
```c
#include <stdio.h>
#include "sample_test.h"
#define PI 3.14
#define SQUARE(x) ((x) * (x))

/* area of circle */
int add(int a, int b) { return a + b; }

int main(void)
{
    int n = MAX;                       // from sample_test.h
    printf("%f\n", PI * SQUARE(2));
    printf("%d\n", n);
    printf("%d\n", add(2, 3));
    return 0;
}
```

**Output: `sample_test.i`**
```c
#include <stdio.h>

int add(int a, int b);

 
int add(int a, int b) { return a + b; }

int main(void)
{
    int n = 100;
    printf("%f\n", 3.14 * ((2) * (2)));
    printf("%d\n", n);
    printf("%d\n", add(2, 3));
    return 0;
}
```

What changed:
* The comments are removed.
* The code from `sample_test.h` is copied in place of `#include "sample_test.h"`.
* `MAX`, `PI` and `SQUARE(2)` are replaced with their values.
* The `#define` lines are gone.

---

## 6. How it works

```
        file.c
          │
          ▼
   ┌──────────────┐
   │  read file   │
   └──────┬───────┘
          ▼
   ┌──────────────┐
   │   remove     │   Step 1
   │   comments   │
   └──────┬───────┘
          ▼
   read line by line
          │
    ┌─────┴───────────────────────┐
    ▼                             ▼
 line starts with #           normal line
    │                             │
    ├─ #include → open that       ▼
    │   file, repeat steps   replace macros   Step 3
    │   (Step 2)                  │
    ├─ #define → save macro       │
    ├─ #undef  → delete macro     │
    └─ #ifdef ... → keep/skip     │
                                  ▼
                               file.i
```

**Step 1: Comment removal.** The program reads the file character by character.
* On `"` or `'`, it copies the whole string as it is.
* On `//`, it skips to the end of the line.
* On `/*`, it skips to `*/`.

**Step 2: File inclusion.** On `#include "x.h"`, the function `process_file()` calls itself for `x.h` (recursion). So the header's code is processed and written at that place.

**Step 3: Macro substitution.**
1. Each `#define` is saved in an array of structures (name, parameters, body).
2. For every word in a normal line, the program checks whether it is a macro.
   * If it is a simple macro, the word is replaced by the macro's body.
   * If it takes arguments, the arguments are read from `( )` and put in place of the parameters.
3. The result is checked again for more macros, so `AREA(r)` → `PI * SQUARE(r)` → `3.14 * ((r)*(r))`.
4. A macro that is being expanded is marked busy, so `#define X X+1` does not loop forever.

---

## 7. Limitations

* `#elif` is not supported.
* `#if` works only with a number or a macro (`#if 0`, `#if DEBUG`).
* System headers like `<stdio.h>` are not expanded.

---

## 8. Hardware / circuit details

Not applicable. This is a software-only project. It runs on any computer with `gcc` and `make`.
---

## Author

**Manjulatha Pathakuntla**

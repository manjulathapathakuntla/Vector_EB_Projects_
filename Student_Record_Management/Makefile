CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -g
TARGET  = student
OBJS    = stud_main.o stud_add.o stud_del.o stud_show.o stud_mod.o \
          stud_save.o stud_sort.o stud_util.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c student.h
	$(CC) $(CFLAGS) -c $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe student.tmp

.PHONY: run clean

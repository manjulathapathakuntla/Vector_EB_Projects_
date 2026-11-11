# Makefile for my_preprocessor
#
#   make          build the program
#   make run      build, then preprocess sample_test.c into sample_test.i
#   make clean    remove the program, object files and generated sample_test.i

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -O2

TARGET  = my_preprocessor
OBJS    = main.o remove_comments.o file_inclusion.o macro.o
DEPS    = preprocessor.h
INPUT   = sample_test.c
HEADER  = sample_test.h
OUTPUT  = sample_test.i

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTPUT): $(TARGET) $(INPUT) $(HEADER)
	./$(TARGET) $(INPUT)

run: $(OUTPUT)
	@echo "Generated $(OUTPUT) from $(INPUT)"

clean:
	rm -f $(TARGET) $(OBJS) $(OUTPUT)

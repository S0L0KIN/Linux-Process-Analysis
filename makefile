## Makefile
CC = gcc
CCFLAGS = -Wall -Werror -Wextra

## Build
all: Assignment2

Assignment2 : Assignment2.o
	${CC} -g -o Assignment2 Assignment2.o

Assignment2.o: Assignment2.c
	${CC} ${CCFLAGS} -c -o Assignment2.o Assignment2.c

## Commands
.PHONY: clean
clean:
	rm -f Assignment2 Assignment2.o

.PHONY: help
help:
	@echo "	 Usage: make [command]"
	@echo "  all         Build Assignment2"
	@echo "  clean       Remove compiled files"
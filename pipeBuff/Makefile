all: pipeBuff

pipeBuff: pipeBuff.c
	mkdir -p bin 2> /dev/null
	gcc -Wall -Wextra -g -O3 -Wconversion -o bin/pipeBuff pipeBuff.c -lpthread

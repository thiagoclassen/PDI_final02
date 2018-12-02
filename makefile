all:
	gcc -o programa main.c pdi.c -lm

run:
	gcc -o programa main.c pdi.c -lm
	clear
	./programa

debug:
	gcc -g main.c pdi.c -lm -Wall -Wextra

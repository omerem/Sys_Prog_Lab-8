all: do
do: main.o
	gcc -g -Wall -m32 -o do main.o
main.o: main.c 
	gcc -g -Wall -m32 -c main.c -o main.o
.PHONY: clean
clean: 
	rm -f *.o do

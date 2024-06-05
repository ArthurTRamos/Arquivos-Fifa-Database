all: main.o operations.o fileFunctions.o registers.o funcoes_fornecidas.o list.o
	gcc main.o operations.o fileFunctions.o registers.o funcoes_fornecidas.o list.o -o trab -std=c99 -Wall

main.o:
	gcc -c main.c -o main.o

operations.o:
	gcc -c operations.c -o operations.o

fileFunctions.o:
	gcc -c fileFunctions.c -o fileFunctions.o

registers.o:
	gcc -c registers.c -o registers.o

funcoes_fornecidas.o:
	gcc -c funcoes_fornecidas.c -o funcoes_fornecidas.o

list.o:
	gcc -c list.c -o list.o

clean:
	rm *.o trab

run:
	./trab

reset: clean all run
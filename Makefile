all:
	gcc -o trab *.c

run:
	./trab

clean:
	rm trab

remove:
	rm *.bin

reset: clean all remove run

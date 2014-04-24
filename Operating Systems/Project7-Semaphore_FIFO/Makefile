main: main.o sem.o fifo.o tas64.S
	gcc -o main.exe main.o fifo.o sem.o tas64.S -Wall -Wextra
main.o: main.c
	gcc -c main.c
fifo.o: fifo.c fifo.h
	gcc -c fifo.c
sem.o: sem.c sem.h
	gcc -c sem.c
debug:
	gcc -g -o Debug.exe main.c sem.c fifo.c tas64.S
clean:
	rm *.o *.exe

main: testbed.o sched.o adjstack.o savectx64.S
	gcc -o testbed.exe testbed.o sched.o adjstack.o savectx64.S -O0 -Wall -Wextra
testbed.o: testbed.c
	gcc -c testbed.c
adjstack.o: adjstack.c
	gcc -c adjstack.c
sched.o: sched.c sched.h
	gcc -c sched.c
debug:
	gcc -g -o Debug.exe testbed.c sched.c adjstack.c savectx64.S
clean:
	rm *.o *.exe

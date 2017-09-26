CC=gcc
CFLAGS=-pthread -Wall
DEPS = queue.h
OBJ = main.o queue.o
LIBS=-lm

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o main trace.dat 

run :
	sudo ./main

trace :
	sudo trace-cmd record -e sched_switch ./main

kernelshark : 
	kernelshark trace.dat



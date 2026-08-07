CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic

simulator: main.o compiler.o memory.o processor.o
	$(CC) $(CFLAGS) -o simulator main.o compiler.o memory.o processor.o

main.o: main.c compiler.h memory.h processor.h
	$(CC) $(CFLAGS) -c main.c

compiler.o: compiler.c compiler.h
	$(CC) $(CFLAGS) -c compiler.c

memory.o: memory.c memory.h
	$(CC) $(CFLAGS) -c memory.c

processor.o: processor.c processor.h memory.h
	$(CC) $(CFLAGS) -c processor.c

clean:
	rm -f *.o simulator simulator.exe program.byte data.byte
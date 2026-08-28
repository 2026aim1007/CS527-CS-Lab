CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -O3

simulator: main.o compiler.o memory.o processor.o os.o
	$(CC) $(CFLAGS) -o simulator main.o compiler.o memory.o processor.o os.o

main.o: main.c os.h
	$(CC) $(CFLAGS) -c main.c

os.o: os.c os.h compiler.h memory.h processor.h
	$(CC) $(CFLAGS) -c os.c

compiler.o: compiler.c compiler.h
	$(CC) $(CFLAGS) -c compiler.c

memory.o: memory.c memory.h processor.h
	$(CC) $(CFLAGS) -c memory.c

processor.o: processor.c processor.h memory.h
	$(CC) $(CFLAGS) -c processor.c

clean:
	rm -f *.o simulator simulator.exe *.byte log.txt
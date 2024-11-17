CC = gcc
CFLAGS = -Wall -pthread
PROGRAMS = philosopher1 philosopher2 philosopher3 barber barber_multi

all: $(PROGRAMS)

philosopher1: philosopher1.c common.h common_threads.h zemaphore.h 
	$(CC) $(CFLAGS) -o philosopher1 philosopher1.c

philosopher2: philosopher2.c common.h common_threads.h zemaphore.h
	$(CC) $(CFLAGS) -o philosopher2 philosopher2.c

philosopher3: philosopher3.c common.h common_threads.h zemaphore.h
	$(CC) $(CFLAGS) -o philosopher3 philosopher3.c

barber: barber.c common.h common_threads.h zemaphore.h
	$(CC) $(CFLAGS) -o barber barber.c

barber_multi: barber_multi.c common.h common_threads.h zemaphore.h
	$(CC) $(CFLAGS) -o barber_multi barber_multi.c

clean:
	rm -f $(PROGRAMS) *.o

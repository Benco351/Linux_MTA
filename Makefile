CC=gcc
CFLAGS=-c -fPIC
LDFLAGS=-shared -o libUtilities.so
LIBS=-L. -lUtilities

all: libUtilities.so my_program_1 my_program_2 my_program_3 my_program_4

libUtilities.so: Utilities.o
	$(CC) $(LDFLAGS) Utilities.o

Utilities.o: Utilities.c Utilities.h
	$(CC) $(CFLAGS) Utilities.c

my_program_1: arrivals.c Q1.o libUtilities.so
	$(CC) -o my_program_1 arrivals.c Q1.o $(LIBS)

Q1.o: Q1.c Q1.h
	$(CC) $(CFLAGS) Q1.c

my_program_2: full_schedule.c Q2.o libUtilities.so
	$(CC) -o my_program_2 full_schedule.c Q2.o $(LIBS)

Q2.o: Q2.c Q2.h
	$(CC) $(CFLAGS) Q2.c

my_program_3: airplane.c Q3.o libUtilities.so
	$(CC) -o my_program_3 airplane.c Q3.o $(LIBS)

Q3.o: Q3.c Q3.h
	$(CC) $(CFLAGS) Q3.c

my_program_4: RefreshDB.o libUtilities.so
	$(CC) -o my_program_4 RefreshDB.o $(LIBS)

RefreshDB.o: RefreshDB.c
	$(CC) $(CFLAGS) RefreshDB.c

clean:
	rm -f *.o *.so my_program_1 my_program_2 my_program_3 my_program_4

CC=gcc
CFLAGS=-c -fPIC
LDFLAGS=-shared -o libUtilities.so
BUILDDIR=build
LIBS=-L./$(BUILDDIR) -lUtilities

SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:%.c=$(BUILDDIR)/%.o)

all: libUtilities.so my_program_1 my_program_2 my_program_3 my_program_4

libUtilities.so: $(BUILDDIR)/Utilities.o
	$(CC) $(LDFLAGS) $< -o $(BUILDDIR)/$@

$(BUILDDIR)/Utilities.o: Utilities.c Utilities.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $< -o $@

my_program_1: arrivals.c Q1.o | $(BUILDDIR)
	$(CC) $< $(BUILDDIR)/Q1.o $(LIBS) -o $@

Q1.o: Q1.c Q1.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $< -o $(BUILDDIR)/$@

my_program_2: full_schedule.c Q2.o | $(BUILDDIR)
	$(CC) $< $(BUILDDIR)/Q2.o $(LIBS) -o $@

Q2.o: Q2.c Q2.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $< -o $(BUILDDIR)/$@

my_program_3: airplane.c Q3.o | $(BUILDDIR)
	$(CC) $< $(BUILDDIR)/Q3.o $(LIBS) -o $@

Q3.o: Q3.c Q3.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $< -o $(BUILDDIR)/$@

RefreshDB.o: RefreshDB.c | $(BUILDDIR)
	$(CC) $(CFLAGS) $< -o $(BUILDDIR)/$@

my_program_4: RefreshDB.o | $(BUILDDIR)
	$(CC) $(BUILDDIR)/$< $(LIBS) -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)/
	rm my_program_*

.PHONY: all clean



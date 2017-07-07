IDIR=./src
CC=gcc
CFLAGS=-I$(IDIR) -std=c11 -pedantic -Wall
ODIR=./src
LIBS=-lconfig

_DEPS = config.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = config.o backnote.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

backnote: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o 


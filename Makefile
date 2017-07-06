IDIR=./src
CC=gcc
CFLAGS=-I$(IDIR) -std=c99 -pedantic -Wall
ODIR=./src

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
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 


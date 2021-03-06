IDIR=./src
CC=gcc
FREETYPEDIR=/usr/include/freetype2/
CFLAGS=-I$(IDIR) -I$(FREETYPEDIR) -std=c11 -pedantic -Wall
ODIR=./src
LIBS=-lconfig -lX11 -lXft -lfreetype -lfontconfig -lcairo -lImlib2

_DEPS = config.h render.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = config.o render.o backnote.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

backnote: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o 


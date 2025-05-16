
IDIR=.
ODIR=../obj
BDIR=../bin

CC=g++

CFLAGS=-std=c++20 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm 
CFLAGSr=-g -ggdb -fpermissive -L/usr/local/lib -Wall -I. -I$(IDIR) -lpq -lm -lhiredis

DEPS = $(wildcard $(IDIR)/*.h)
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, $(ODIR)/%.o, $(SRC))

con2db_OBJ=$(wildcard ../../con2db/*.o)
dbfuncts_OBJ=$(wildcard ../../databases/db_functions/obj/*.o)

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(CFLAGSr)

$(BDIR)/main: $(OBJ) $(DEPS)
	$(CC) -o $@ $(OBJ) $(con2db_OBJ) $(dbfuncts_OBJ) $(CFLAGS) $(CFLAGSr)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(IDIR)/*~ core 
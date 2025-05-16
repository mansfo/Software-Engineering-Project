
IDIR=.
ODIR=../obj
BDIR=../bin

CC=g++
CFLAGS=-std=c++20 -I. -I$(IDIR) -I/usr/include/postgresql -lpq -lm -g -Wall

DEPS = $(wildcard $(IDIR)/*.h)
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, $(ODIR)/%.o, $(SRC))

con2db_OBJ=$(wildcard ../../con2db/*.o)

all:	con2db $(BDIR)/main 
all:	con2redis $(BDIR)/main 
all:	client $(BDIR)/main 
all:	databases $(BDIR)/main 
all:	monitor $(BDIR)/main 
all:	server $(BDIR)/main 
all:	test $(BDIR)/main

con2db:
	$(MAKE) -C ../../con2db

con2redis:
	$(MAKE) -C ../../con2redis/src
	
client:
	$(MAKE) -C ../../client/src

databases:
	$(MAKE) -C ../../databases/db_functions/src

monitor:
	$(MAKE) -C ../../monitor/src

server:
	$(MAKE) -C ../../server/src

test:
	rm -f ../../test/obj/*.o *~ ../../test/src/*~ core
	$(MAKE) -C ../../test/src


$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BDIR)/main: $(OBJ) $(DEPS)
	$(CC) -g -o $@ $(OBJ) $(con2db_OBJ) $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ $(IDIR)/*~ core  ../../client/obj/*.o *~ ../../client/src/*~ core ../../con2redis/obj/*.o *~ ../../con2redis/src/*~ core ../../databases/db_functions/obj/*.o *~ ../../databases/db_functions/src/*~ core ../../monitor/obj/*.o *~ ../../monitor/src/*~ core ../../server/obj/*.o *~ ../../server/src/*~ core ../../test/obj/*.o *~ ../../test/src/*~ core
CC=gcc
CCFLAGS= -g -Wall
CCWITHOUTMAIN = -c
LIBDIR=headers
INCLUDE=-I headers
SRC=src
BUILDDIR=build
TARGET=ramdisk
TESTS=tests


all: dir list.o clean
	$(CC) $(CCFLAGS) $(BUILDDIR)/list.o $(SRC)/main.c -o $(TARGET) $(INCLUDE)

listtest: dir list.o listtest.o clean 

listtest.o: $(TESTS)/list_test.c $(SRC)/list.c
	$(CC) $(CCFLAGS) $(BUILDDIR)/list.o $(TESTS)/list_test.c -o $(TESTS)/list_test.o $(INCLUDE)

hashtabletest.o: $(TESTS)/hash_table_test.c $(SRC)/hashtable.c list.o hashtable.o
	$(CC) $(CCFLAGS) $(BUILDDIR)/list.o $(BUILDDIR)/hashtable.o $(TESTS)/hash_table_test.c -o $(TESTS)/hash_table_test.o $(INCLUDE)

dir: 
	mkdir $(BUILDDIR)

list.o: $(SRC)/list.c $(LIBDIR)/list.h
	$(CC) $(CCFLAGS) $(CCWITHOUTMAIN) $(SRC)/list.c -o $(BUILDDIR)/list.o $(INCLUDE)

hashtable.o: $(SRC)/hashtable.c $(LIBDIR)/hashtable.h
	$(CC) $(CCFLAGS) $(CCWITHOUTMAIN) $(SRC)/hashtable.c -o $(BUILDDIR)/hashtable.o $(INCLUDE)

clean:
	rm -rf $(BUILDDIR)




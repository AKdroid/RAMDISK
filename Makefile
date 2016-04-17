CC=gcc
CCFLAGS= -g -Wall
CCWITHOUTMAIN = -c
LIBDIR=headers
INCLUDE=-I headers
SRC=src
BUILDDIR=build
TARGET=ramdisk
TESTS=tests


all: list.o stack.o memoryblock.o hashtable.o filetable.o
	$(CC) $(CCFLAGS) $(BUILDDIR)/stack.o $(BUILDDIR)/list.o $(BUILDDIR)/memory_block.o $(BUILDDIR)/filetable.o $(BUILDDIR)/hashtable.o $(SRC)/main.c `pkg-config fuse --cflags --libs` -o $(TARGET) $(INCLUDE)

listtest: dir list.o listtest.o clean 

listtest.o: $(TESTS)/list_test.c $(SRC)/list.c
	$(CC) $(CCFLAGS) $(BUILDDIR)/list.o $(TESTS)/list_test.c -o $(TESTS)/list_test.o $(INCLUDE)

hashtabletest.o: $(TESTS)/hash_table_test.c $(SRC)/hashtable.c list.o hashtable.o
	$(CC) $(CCFLAGS) $(BUILDDIR)/list.o $(BUILDDIR)/hashtable.o $(TESTS)/hash_table_test.c -o $(TESTS)/hash_table_test.o $(INCLUDE)

stacktest.o: $(TESTS)/stack_test.c $(SRC)/stack.c stack.o
	$(CC) $(CCFLAGS) $(BUILDDIR)/stack.o $(TESTS)/stack_test.c -o $(TESTS)/stack_test.o $(INCLUDE)

blockmanagertest.o: $(TESTS)/memory_block_test.c $(SRC)/memory_block.c memoryblock.o stack.o
	$(CC) $(CCFLAGS) $(BUILDDIR)/stack.o $(BUILDDIR)/memory_block.o $(TESTS)/memory_block_test.c -o $(TESTS)/memory_block_test.o $(INCLUDE)

dir: 
	mkdir $(BUILDDIR)

list.o: $(SRC)/list.c $(LIBDIR)/list.h
	$(CC) $(CCFLAGS) $(CCWITHOUTMAIN) $(SRC)/list.c -o $(BUILDDIR)/list.o $(INCLUDE)

hashtable.o: $(SRC)/hashtable.c $(LIBDIR)/hashtable.h
	$(CC) $(CCFLAGS) $(CCWITHOUTMAIN) $(SRC)/hashtable.c -o $(BUILDDIR)/hashtable.o $(INCLUDE)

stack.o: $(SRC)/stack.c $(LIBDIR)/stack.h
	$(CC) $(CCFLAGS) $(CCWITHOUTMAIN) $(SRC)/stack.c -o $(BUILDDIR)/stack.o $(INCLUDE)

memoryblock.o: $(SRC)/memory_block.c $(LIBDIR)/memory_block.h
	$(CC) $(CCFLAGS) $(CCWITHOUTMAIN) $(SRC)/memory_block.c -o $(BUILDDIR)/memory_block.o $(INCLUDE)

filetable.o: hashtable.o $(SRC)/filetable.c $(LIBDIR)/filetable.h
	$(CC) $(CCFLAGS) $(CCWITHOUTMAIN) $(BUILDDIR)/list.o $(BUILDDIR)/hashtable.o $(SRC)/filetable.c -o $(BUILDDIR)/filetable.o $(INCLUDE)

clean:
	rm -rf $(BUILDDIR)




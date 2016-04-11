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

dir: 
	mkdir $(BUILDDIR)

list.o: $(SRC)/list.c
	$(CC) $(CCFLAGS) $(CCWITHOUTMAIN) $(SRC)/list.c -o $(BUILDDIR)/list.o $(INCLUDE)

clean:
	rm -rf $(BUILDDIR)




CFLAGS= -g -fpermissive
LDFLAGS= -lpthread -lrt
LDFLAGS1= -lpthread -fpermissive
CC=g++

all: boundedBuffer bridge 

# To make an executable
boundedBuffer: boundedBuffer.o 
	$(CC) $(LDFLAGS) -o boundedBuffer boundedBuffer.o

bridge: bridge.o
	$(CC) $(LDFLAGS1) -o bridge bridge.o

# To make an object from source
.c.o:
	$(CC) $(CFLAGS) -c $*.c

# clean out the dross
clean:
	-rm boundedBuffer bridge *.o


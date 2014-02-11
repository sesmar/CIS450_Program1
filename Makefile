CFLAGS= -g
LDFLAGS= -lpthread -lrt
LDFLAGS1= -lpthread
CC=g++

all: boundedBuffer 

# To make an executable
boundedBuffer: boundedBuffer.o 
	$(CC) $(LDFLAGS) -o boundedBuffer boundedBuffer.o

# To make an object from source
.c.o:
	$(CC) $(CFLAGS) -c $*.c

# clean out the dross
clean:
	-rm boundedBuffer *.o


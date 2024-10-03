TARGET = lbp
CC = gcc
CFLAGS = -Wall -g

$(TARGET): main.o	
	$(CC) $(CFLAGS) -o $(TARGET) main.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

file.o: file.c file.h
	$(CC) $(CFLAGS) -c tree.c

clean:
	rm -f $(TARGET) *.o

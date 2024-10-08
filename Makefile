TARGET = lbp
CC = gcc
CFLAGS = -Wall -g -lm

$(TARGET): main.o	
	$(CC) -o $(TARGET) main.o $(CFLAGS)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)

clean:
	rm -f $(TARGET) *.o

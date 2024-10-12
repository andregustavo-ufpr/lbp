TARGET = LBP
CC = gcc
CFLAGS = -Wall -g -lm

$(TARGET): main.o file.o lbp.o
	$(CC) -o $(TARGET) main.o file.o lbp.o $(CFLAGS)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)

file.o: file.c
	$(CC) -c file.c $(CFLAGS)

lbp.o: lbp.c
	$(CC) -c lbp.c $(CFLAGS)

clean:
	rm -f $(TARGET) *.o

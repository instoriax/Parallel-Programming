CC = mpicxx
CFLAGS = -Wall

matmul: main.o matrix_operations.o
	$(CC) $(CFLAGS) -o matmul main.o matrix_operations.o

main.o: main.cc
	$(CC) $(CFLAGS) -c main.cc

matrix_operations.o: matrix_operations.cpp
	$(CC) $(CFLAGS) -c matrix_operations.cpp

clean:
	rm -f matmul main.o matrix_operations.o
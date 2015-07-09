CC = g++
CFLAGS = -c -Wall -Wextra -O2 -std=c++11

all: seurat

seurat: main.o image.o
	$(CC) -lncurses main.o image.o -o seurat

main.o: src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp

image.o: src/image.cpp
	$(CC) $(CFLAGS) src/image.cpp

clean:
	rm -fv seurat logs.txt *.o

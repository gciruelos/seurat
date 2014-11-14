all: seurat

seurat: main.o image.o bmp.o
		g++ main.o image.o bmp.o -o seurat -lncurses

main.o: main.cpp
		g++ -c main.cpp

image.o: image.cpp
		g++ -c image.cpp

bmp.o: bmp.cpp
		g++ -c bmp.cpp

clean:
		rm -rf *o seurat logs.txt
		









		

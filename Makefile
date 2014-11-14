all: seurat

seurat: main.o image.o bmp.o
		g++ -Wall -O2 main.o image.o bmp.o -o seurat -lncurses

main.o: src/main.cpp
		g++ -Wall -O2 -c src/main.cpp

image.o: src/image.cpp
		g++ -Wall -O2 -c src/image.cpp

bmp.o: src/bmp.cpp
		g++ -Wall -O2 -c src/bmp.cpp

clean:
		rm -rf *o seurat logs.txt
		









		

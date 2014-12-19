all: seurat

seurat:
	g++ -O2 -std=c++11 src/main.cpp src/image.cpp src/bmp.cpp -lncurses -o seurat

clean:
	rm -fv seurat logs.txt

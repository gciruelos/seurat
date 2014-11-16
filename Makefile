all: seurat

seurat:
	g++ -O2 src/main.cpp src/image.cpp src/bmp.cpp -lncurses -o seurat

clean:
	rm seurat logs.txt

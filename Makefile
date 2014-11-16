all: seurat

seurat:
	clang++ src/main.cpp src/image.cpp src/bmp.cpp -lncurses -o seurat


clean:
	rm seurat logs.txt

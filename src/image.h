#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"


#ifndef PIXEL
#include "pixel.h"
#define PIXEL
#endif 

typedef unsigned char color;
typedef std::vector< std::vector<color> > color_matrix;


int rgbto8(pixel);
int rgbto16(pixel);

void dithering(image_matrix, int, color_matrix&);

struct options{
  int colors;
  int dithering_option;

  int width, height;

  int x_i, delta_x;
  int y_i, delta_y;

  bool debug;
  std::ofstream logs;

  std::string filename;
  bool opened;

  bool info;
};



class Image {
public:
  Image(std::string, bool& opened);
  Image() {}

  void prnt_raw_img() const{
    for(int i = 0; i<matrix.size(); i++){
      for (int j = 0; j<matrix[0].size(); j++) {
        pixel p = matrix[i][j];
        std::cout << p.r << p.g << p.b;
      }std::cout << std::endl;
    }
  }

  int height() const {
    return matrix.size();
  }
  int width() const {
    return (matrix[0]).size();
  }

  image_matrix scale(int, int, int, int, int, int) const;
  color_matrix generate_representation(int, int, options*) const;
private:
  image_matrix matrix;
};


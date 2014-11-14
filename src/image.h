#include <iostream>
#include <fstream>

#include "bmp.h"
#include "pixel.h"

class Image {
public:
  Image(char*);
  Image() {}

  void prnt_raw_img() const{
    for(int i = 0; i<matrix.size(); i++){
      for (int j = 0; j<matrix[0].size(); j++) {
        pixel p = matrix[i][j];
        std::cout << p.rgb[0] << p.rgb[1] << p.rgb[2];
      }std::cout << std::endl;
    }
  }

  int height() const {
    return matrix.size();
  }
  int width() const {
    return (matrix[0]).size();
  }

  image_matrix generate_representation(int, int, int, int, int, int) const;

  pixel color(int i, int j) const {
    return matrix[i][j];
  }

private:
  image_matrix matrix;
};

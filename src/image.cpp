#include "image.h"

int image_detect_format(std::istream& is) {

  /* JPG = 0
   * PNG = 1
   * BMP = 2
   * GIF = 3
   */
  unsigned char in;

  std::vector<unsigned char> bytes;

  for(int i = 0; i< 10; i++) {
    is >> in;
    bytes.push_back(in);
  }

  if(bytes[0] == 0xff && bytes[1] == 0xd8) return 0;

  else if(bytes[0] == 0x89 && bytes[1] == 0x50 &&
          bytes[2] == 0x4e && bytes[3] == 0x47 &&
          bytes[4] == 0x0d && bytes[5] == 0x0a &&
          bytes[6] == 0x1a && bytes[7] == 0x0a) return 1;

  else if(bytes[0] == 'B' && bytes[1] == 'M') return 2;

  else return -1;
}


image_matrix Image::generate_representation(int width, int height, int x_i, int delta_x, int y_i, int delta_y)const {

    image_matrix img_repr;
    pixel black;
    black.rgb[0] = 0;
    black.rgb[1] = 0;
    black.rgb[2] = 0;

    float ratio_delta = ((float) delta_x) / delta_y;
    float ratio_image = ((float) width) / height;

    
    if(ratio_image < ratio_delta) { 
      int repr_width = width;
      int repr_height  = (int) ((float) this->height() * repr_width / this->width());

      float scale = (this->height())/((float) repr_height);

      int padding = (height - repr_height)/2;
      bool one_more = (height - repr_height)%2 != 0;


      std::vector<pixel> black_row(repr_width, black);
      if(one_more) img_repr.push_back(black_row);

      for(int i = 0; i<padding; i++){
          img_repr.push_back(black_row);
      }

      for(int i = 0; i<repr_height; i++) {
        std::vector<pixel> row;
        for(int j = 0; j<repr_width; j++) {
          int i_ = (int) i*scale; i_ = i_ * (((float) delta_y)/(matrix.size())) + y_i;
          int j_ = (int) j*scale; j_ = j_ * (((float) delta_x)/(matrix[0].size())) + x_i;
          if (i_ >= matrix.size()) i_ = matrix.size()-1;
          if (j_ >= matrix[0].size()) j_ = matrix[0].size()-1;

          pixel p = matrix[i_][j_];
          row.push_back(p);
        }
        img_repr.push_back(row);
      }

      for(int i = 0; i<padding; i++){
          img_repr.push_back(black_row);
      }
    }
    else {
      int repr_height = height;
      int repr_width  = (int) ((float) this->width() * repr_height / this->height());

      float scale = (this->width())/((float) repr_width);

      int padding = (width - repr_width)/2;
      bool one_more = (width - repr_width)%2 != 0;
      for(int i = 0; i<height; i++) {
        std::vector<pixel> row;
        if(one_more) row.push_back(black);

        for(int j = 0; j<padding; j++) {
          row.push_back(black);
        }

        for(int j = 0; j<repr_width; j++) {
          int i_ = (int) i*scale; i_ = i_ * (((float) delta_y)/(matrix.size())) + y_i;
          int j_ = (int) j*scale; j_ = j_ * (((float) delta_x)/(matrix[0].size())) + x_i;     
          if (i_ >= matrix.size()) i_ = matrix.size()-1;
          if (j_ >= matrix[0].size()) j_ = matrix[0].size()-1;

          pixel p = matrix[i_][j_];
          row.push_back(p);
        }

        for(int j = 0; j<padding; j++) {
          row.push_back(black);
        }

        img_repr.push_back(row);
      }
    }
    return img_repr;
  }


Image::Image(char* filename) {
  std::ifstream ifs(filename);

  int format = image_detect_format(ifs);
  
  if (format == 2)
    read_from_bmp(filename, matrix);
}

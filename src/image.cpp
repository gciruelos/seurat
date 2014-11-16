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

int rgbto8(pixel p) {
  int r = p.r;
  int g = p.g;
  int b = p.b;

  int c = 128;

  if (r < c) {
    if(g < c) {
      if(b < c) return 0; //black
      else        return 4; //blue
    } else {
      if(b < c) return 2; //green
      else        return 6; //cyan
    }
  } else {
    if(g < c) {
      if(b < c) return 1; //red
      else        return 5; //magenta
    } else {
      if(b < c) return 3; //yellow
      else        return 7; //white
    }
  }
}

int rgbto16(pixel p) {
  int code8 = rgbto8(p);
  
  if(p.r+p.g+p.b > 510) return code8+8;
  else return code8;
}

int rgbto2(pixel p){
  if (p.r+p.g+p.b > 382)
    return 7;
  else
    return 0;
}


image_matrix Image::scale(int repr_width, int repr_height, int x_i, int delta_x, int y_i, int delta_y)const {
  //shitty algorithm
  //TODO : Implement bicubic interpolation

  float scale = (this->width())/((float) repr_width);
  image_matrix scaled;

  for(int i = 0; i < repr_height; i++){
    std::vector<pixel> row;
    for(int j = 0; j < repr_width; j++){
      int i_ = (int) i*scale; i_ = i_ * (((float) delta_y)/(matrix.size())) + y_i;
      int j_ = (int) j*scale; j_ = j_ * (((float) delta_x)/(matrix[0].size())) + x_i;
      if (i_ >= matrix.size()) i_ = matrix.size()-1;
      if (j_ >= matrix[0].size()) j_ = matrix[0].size()-1;


      std::vector<pixel> neighbours;

      int scale_ = scale/4;
      for(int a = i_ - scale_; a < i_ + scale_; a++){
        for(int b = j_ - scale_; b < j_ + scale_; b++){
          if (a>0 && a<this->height() && b>0 && b<this->width())
            neighbours.push_back(matrix[a][b]);
        }
      }

      pixel p; p.r = 0; p.g = 0; p.b = 0;
      for(int a  = 0; a < neighbours.size(); a++){
        p.r+= neighbours[a].r;
        p.g+= neighbours[a].g;
        p.b+= neighbours[a].b;
      }
      p.r /= neighbours.size();
      p.g /= neighbours.size();
      p.b /= neighbours.size();


      row.push_back(p);
    }
    scaled.push_back(row);
  }
  return scaled;
}

pixel ntorgb(int n){
  pixel p;
  switch(n){
    case 0:  p.r = 0;   p.g = 0;   p.b = 0;   break; // black
    case 1:  p.r = 255; p.g = 0;   p.b = 0;   break; // red
    case 2:  p.r = 0;   p.g = 255; p.b = 0;   break; // green
    case 3:  p.r = 255; p.g = 255; p.b = 0;   break; // yellow
    case 4:  p.r = 0;   p.g = 0;   p.b = 255; break; // blue
    case 5:  p.r = 255; p.g = 0;   p.b = 255; break; // magenta
    case 6:  p.r = 0;   p.g = 255; p.b = 255; break; // cyan
    case 7:  p.r = 255; p.g = 255; p.b = 255; break; // white
    case 8:  p.r = 0;   p.g = 0;   p.b = 0;   break;
    case 9:  p.r = 128; p.g = 0;   p.b = 0;   break;
    case 10: p.r = 0;   p.g = 128; p.b = 0;   break;
    case 11: p.r = 128; p.g = 128; p.b = 0;   break;
    case 12: p.r = 0;   p.g = 0;   p.b = 128; break;
    case 13: p.r = 128; p.g = 0;   p.b = 128; break;
    case 14: p.r = 0;   p.g = 128; p.b = 128; break;
    case 15: p.r = 128; p.g = 128; p.b = 128; break;
  }
  return p;
}

pixel add(pixel a, pixel b){
  pixel c;
  c.r = a.r+b.r; c.g = a.g+b.g; c.b = a.b+b.b;
  return c;
}
pixel diff(pixel a, pixel b){
  pixel c;
  c.r = a.r-b.r; c.g = a.g-b.g; c.b = a.b-b.b;
  return c;
}
pixel mult(float k, pixel a){
  pixel c;
  c.r = k * a.r; c.g = k * a.g; c.b = k * a.b;
  return c;
}


void dithering(image_matrix m, int colors, color_matrix& result){
  /* Floyd–Steinberg dithering */
 
  for(int i = 0; i < m.size(); i++){
    for(int j = 0; j < m[0].size(); j++){
      pixel old_pixel = m[i][j];
      pixel new_pixel = ntorgb(colors == 2? 
                          rgbto2(old_pixel) : 
                          (colors == 8? rgbto8(old_pixel) : rgbto16(old_pixel)));
      m[i][j] = new_pixel;
      pixel quant_error = diff(old_pixel, new_pixel);
      /*
      
      if (i < m.size()-1){
                               m[i+1][j  ] = add(m[i+1][j  ], mult(5./16, quant_error)); 
        if (j > 0)             m[i+1][j-1] = add(m[i+1][j-1], mult(3./16, quant_error));
        if (j < m[0].size()-1) m[i+1][j+1] = add(m[i+1][j+1], mult(1./16, quant_error));
      }
      if (j < m[0].size()-1) m[i][j+1] =  add(m[i][j+1], mult(7./16, quant_error));
     */ 

      /*         X   7   5 
         3   5   7   5   3
         1   3   5   3   1
     jarvis, judice, and ninke dithering
      */
   /*   
      if (j < m[0].size()-1)  m[i][j+1] =  add(m[i][j+1], mult(7./48, quant_error));
      if (j < m[0].size()-2)  m[i][j+2] =  add(m[i][j+2], mult(5./48, quant_error));
      if (i < m.size()-1){
        if (j > 1)              m[i+1][j-2] =  add(m[i+1][j-2], mult(3./48, quant_error));
        if (j > 0)              m[i+1][j-1] =  add(m[i+1][j-1], mult(5./48, quant_error));
                                m[i+1][j  ] =  add(m[i+1][j  ], mult(7./48, quant_error));
        if (j < m[0].size()-1)  m[i+1][j+1] =  add(m[i+1][j+1], mult(5./48, quant_error));
        if (j < m[0].size()-2)  m[i+1][j+2] =  add(m[i+1][j+2], mult(3./48, quant_error));
      }
      if (i < m.size()-2){
        if (j > 1)              m[i+2][j-2] =  add(m[i+2][j-2], mult(1./48, quant_error));
        if (j > 0)              m[i+2][j-1] =  add(m[i+2][j-1], mult(3./48, quant_error));
                                m[i+2][j  ] =  add(m[i+2][j  ], mult(5./48, quant_error));
        if (j < m[0].size()-1)  m[i+2][j+1] =  add(m[i+2][j+1], mult(3./48, quant_error));
        if (j < m[0].size()-2)  m[i+2][j+2] =  add(m[i+2][j+2], mult(1./48, quant_error));
      }
     */ 
    }
  }

  for(int i = 0; i < m.size(); i++){
    std::vector<color> row;
    for(int j = 0; j < m[0].size(); j++){
      pixel p = m[i][j];
      row.push_back(colors == 2? rgbto2(p) : (colors == 8? rgbto8(p) : rgbto16(p)));
    }
    result.push_back(row);
  } 
  
}

color_matrix Image::generate_representation(int width, int height, int x_i, int delta_x, int y_i, int delta_y, int colors)const {

  color_matrix img_repr;
  
  unsigned char black = 0; 

  float ratio_delta = ((float) delta_x) / delta_y;
  float ratio_image = ((float) width) / height;

  image_matrix scaled;

  if(ratio_image < ratio_delta) { 
    int repr_width = width;
    int repr_height  = (int) ((float) this->height() * repr_width / this->width());

    float scale = (this->height())/((float) repr_height);

    int padding = (height - repr_height)/2;
    bool one_more = (height - repr_height)%2 != 0;

    scaled = this -> scale(repr_width, repr_height, x_i, delta_x, y_i, delta_y);
    color_matrix dithered;
    dithering(scaled, colors, dithered);     
    
    std::vector<color> black_row(repr_width, black);
    if(one_more) img_repr.push_back(black_row);

    for(int i = 0; i<padding; i++){
        img_repr.push_back(black_row);
    }

    for(int i = 0; i<repr_height; i++) {
      std::vector<color> row;
      for(int j = 0; j<repr_width; j++) {
        row.push_back(dithered[i][j]);//can be done faster
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
   
    scaled = this -> scale(repr_width, repr_height, x_i, delta_x, y_i, delta_y);
    color_matrix dithered;
    dithering(scaled, colors, dithered); 
    
    for(int i = 0; i<height; i++) {
      std::vector<color> row;
      if(one_more) row.push_back(black);

      for(int j = 0; j<padding; j++) {
        row.push_back(black);
      }

      for(int j = 0; j<repr_width; j++) {  
        row.push_back(dithered[i][j]);//can be done faster
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
